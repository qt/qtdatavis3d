// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "abstract3dcontroller_p.h"
#include "qabstract3daxis_p.h"
#include "qvalue3daxis_p.h"
#include "qcategory3daxis_p.h"
#include "qabstract3dseries_p.h"
#include "q3dscene_p.h"
#include "qabstract3dinputhandler_p.h"
#include "qtouch3dinputhandler.h"
#include "thememanager_p.h"
#include "q3dtheme_p.h"
#include "qcustom3ditem_p.h"
#include "utils_p.h"
#include <QtCore/QThread>
#include <QtOpenGL/QOpenGLFramebufferObject>
#include <QtCore/QMutexLocker>

QT_BEGIN_NAMESPACE

Abstract3DController::Abstract3DController(QRect initialViewport, Q3DScene *scene,
                                           QObject *parent) :
    QObject(parent),
    m_themeManager(new ThemeManager(this)),
    m_selectionMode(QAbstract3DGraphNG::SelectionItem),
    m_shadowQuality(QAbstract3DGraphNG::ShadowQualityMedium),
    m_useOrthoProjection(false),
    m_aspectRatio(2.0),
    m_horizontalAspectRatio(0.0),
    m_optimizationHints(QAbstract3DGraphNG::OptimizationDefault),
    m_reflectionEnabled(false),
    m_reflectivity(0.5),
    m_locale(QLocale::c()),
    m_scene(scene),
    m_activeInputHandler(0),
    m_axisX(0),
    m_axisY(0),
    m_axisZ(0),
    m_isDataDirty(true),
    m_isCustomDataDirty(true),
    m_isCustomItemDirty(true),
    m_isSeriesVisualsDirty(true),
    m_renderPending(false),
    m_isPolar(false),
    m_radialLabelOffset(1.0f),
    m_measureFps(false),
    m_numFrames(0),
    m_currentFps(0.0),
    m_clickedType(QAbstract3DGraphNG::ElementNone),
    m_selectedLabelIndex(-1),
    m_selectedCustomItemIndex(-1),
    m_margin(-1.0)
{
    if (!m_scene)
        m_scene = new Q3DScene;
    m_scene->setParent(this);

    // Set initial theme
    Q3DTheme *defaultTheme = new Q3DTheme(Q3DTheme::ThemeQt);
    defaultTheme->d_ptr->setDefaultTheme(true);
    setActiveTheme(defaultTheme);

    m_scene->d_ptr->setViewport(initialViewport);
    m_scene->activeLight()->setAutoPosition(true);

    // Create initial default input handler
    QAbstract3DInputHandler *inputHandler;
    inputHandler = new QTouch3DInputHandler();
    inputHandler->d_ptr->m_isDefaultHandler = true;
    setActiveInputHandler(inputHandler);
    connect(m_scene->d_ptr.data(), &Q3DScenePrivate::needRender, this,
            &Abstract3DController::emitNeedRender);
}

Abstract3DController::~Abstract3DController()
{
    delete m_scene;
    delete m_themeManager;
    foreach (QCustom3DItem *item, m_customItems)
        delete item;
    m_customItems.clear();
}

void Abstract3DController::addSeries(QAbstract3DSeries *series)
{
    insertSeries(m_seriesList.size(), series);
}

void Abstract3DController::insertSeries(int index, QAbstract3DSeries *series)
{
    if (series) {
        if (m_seriesList.contains(series)) {
            int oldIndex = m_seriesList.indexOf(series);
            if (index != oldIndex) {
                m_seriesList.removeOne(series);
                if (oldIndex < index)
                    index--;
                m_seriesList.insert(index, series);
            }
        } else {
            int oldSize = m_seriesList.size();
            m_seriesList.insert(index, series);
            series->d_ptr->setController(this);
            QObject::connect(series, &QAbstract3DSeries::visibilityChanged,
                             this, &Abstract3DController::handleSeriesVisibilityChanged);
            series->d_ptr->resetToTheme(*m_themeManager->activeTheme(), oldSize, false);
        }
        if (series->isVisible())
            handleSeriesVisibilityChangedBySender(series);
    }
}

void Abstract3DController::removeSeries(QAbstract3DSeries *series)
{
    if (series && series->d_ptr->m_controller == this) {
        m_seriesList.removeAll(series);
        QObject::disconnect(series, &QAbstract3DSeries::visibilityChanged,
                            this, &Abstract3DController::handleSeriesVisibilityChanged);
        series->d_ptr->setController(0);
        m_isDataDirty = true;
        m_isSeriesVisualsDirty = true;
        emitNeedRender();
    }
}

bool Abstract3DController::hasSeries(QAbstract3DSeries *series)
{
    return m_seriesList.contains(series);
}

QList<QAbstract3DSeries *> Abstract3DController::seriesList()
{
    return m_seriesList;
}

void Abstract3DController::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_activeInputHandler)
        m_activeInputHandler->mouseDoubleClickEvent(event);
}

void Abstract3DController::touchEvent(QTouchEvent *event)
{
    if (m_activeInputHandler)
        m_activeInputHandler->touchEvent(event);
}

void Abstract3DController::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    if (m_activeInputHandler)
        m_activeInputHandler->mousePressEvent(event, mousePos);
}

void Abstract3DController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    if (m_activeInputHandler)
        m_activeInputHandler->mouseReleaseEvent(event, mousePos);
}

void Abstract3DController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    if (m_activeInputHandler)
        m_activeInputHandler->mouseMoveEvent(event, mousePos);
}

#if QT_CONFIG(wheelevent)
void Abstract3DController::wheelEvent(QWheelEvent *event)
{
    if (m_activeInputHandler)
        m_activeInputHandler->wheelEvent(event);
}
#endif

void Abstract3DController::handleThemeColorStyleChanged(Q3DTheme::ColorStyle style)
{
    // Set value for series that have not explicitly set this value
    foreach (QAbstract3DSeries *series, m_seriesList) {
        if (!series->d_ptr->m_themeTracker.colorStyleOverride) {
            series->setColorStyle(style);
            series->d_ptr->m_themeTracker.colorStyleOverride = false;
        }
    }
    markSeriesVisualsDirty();
}

void Abstract3DController::handleThemeBaseColorsChanged(const QList<QColor> &colors)
{
    int colorIdx = 0;
    // Set value for series that have not explicitly set this value
    foreach (QAbstract3DSeries *series, m_seriesList) {
        if (!series->d_ptr->m_themeTracker.baseColorOverride) {
            series->setBaseColor(colors.at(colorIdx));
            series->d_ptr->m_themeTracker.baseColorOverride = false;
        }
        if (++colorIdx >= colors.size())
            colorIdx = 0;
    }
    markSeriesVisualsDirty();
}

void Abstract3DController::handleThemeBaseGradientsChanged(const QList<QLinearGradient> &gradients)
{
    int gradientIdx = 0;
    // Set value for series that have not explicitly set this value
    foreach (QAbstract3DSeries *series, m_seriesList) {
        if (!series->d_ptr->m_themeTracker.baseGradientOverride) {
            series->setBaseGradient(gradients.at(gradientIdx));
            series->d_ptr->m_themeTracker.baseGradientOverride = false;
        }
        if (++gradientIdx >= gradients.size())
            gradientIdx = 0;
    }
    markSeriesVisualsDirty();
}

void Abstract3DController::handleThemeSingleHighlightColorChanged(const QColor &color)
{
    // Set value for series that have not explicitly set this value
    foreach (QAbstract3DSeries *series, m_seriesList) {
        if (!series->d_ptr->m_themeTracker.singleHighlightColorOverride) {
            series->setSingleHighlightColor(color);
            series->d_ptr->m_themeTracker.singleHighlightColorOverride = false;
        }
    }
    markSeriesVisualsDirty();
}

void Abstract3DController::handleThemeSingleHighlightGradientChanged(
        const QLinearGradient &gradient)
{
    // Set value for series that have not explicitly set this value
    foreach (QAbstract3DSeries *series, m_seriesList) {
        if (!series->d_ptr->m_themeTracker.singleHighlightGradientOverride) {
            series->setSingleHighlightGradient(gradient);
            series->d_ptr->m_themeTracker.singleHighlightGradientOverride = false;
        }
    }
    markSeriesVisualsDirty();
}

void Abstract3DController::handleThemeMultiHighlightColorChanged(const QColor &color)
{
    // Set value for series that have not explicitly set this value
    foreach (QAbstract3DSeries *series, m_seriesList) {
        if (!series->d_ptr->m_themeTracker.multiHighlightColorOverride) {
            series->setMultiHighlightColor(color);
            series->d_ptr->m_themeTracker.multiHighlightColorOverride = false;
        }
    }
    markSeriesVisualsDirty();
}

void Abstract3DController::handleThemeMultiHighlightGradientChanged(const QLinearGradient &gradient)
{
    // Set value for series that have not explicitly set this value
    foreach (QAbstract3DSeries *series, m_seriesList) {
        if (!series->d_ptr->m_themeTracker.multiHighlightGradientOverride) {
            series->setMultiHighlightGradient(gradient);
            series->d_ptr->m_themeTracker.multiHighlightGradientOverride = false;
        }
    }
    markSeriesVisualsDirty();
}

void Abstract3DController::handleThemeTypeChanged(Q3DTheme::Theme theme)
{
    Q_UNUSED(theme);

    if (!m_qml)
        return;

    // Changing theme type is logically equivalent of changing the entire theme
    // object, so reset all attached series to the new theme.
    bool force = true;//m_qml->isReady(); // TODO: Is this needed?
    Q3DTheme *activeTheme = m_themeManager->activeTheme();
    for (int i = 0; i < m_seriesList.size(); i++)
        m_seriesList.at(i)->d_ptr->resetToTheme(*activeTheme, i, force);

    markSeriesVisualsDirty();
}

void Abstract3DController::setAxisX(QAbstract3DAxis *axis)
{
    // Setting null axis will always create new default axis
    if (!axis || axis != m_axisX) {
        setAxisHelper(QAbstract3DAxis::AxisOrientationX, axis, &m_axisX);
        emit axisXChanged(m_axisX);
    }
}

QAbstract3DAxis *Abstract3DController::axisX() const
{
    return m_axisX;
}

void Abstract3DController::setAxisY(QAbstract3DAxis *axis)
{
    // Setting null axis will always create new default axis
    if (!axis || axis != m_axisY) {
        setAxisHelper(QAbstract3DAxis::AxisOrientationY, axis, &m_axisY);
        emit axisYChanged(m_axisY);
    }
}

QAbstract3DAxis *Abstract3DController::axisY() const
{
    return m_axisY;
}

void Abstract3DController::setAxisZ(QAbstract3DAxis *axis)
{
    // Setting null axis will always create new default axis
    if (!axis || axis != m_axisZ) {
        setAxisHelper(QAbstract3DAxis::AxisOrientationZ, axis, &m_axisZ);
        emit axisZChanged(m_axisZ);
    }
}

QAbstract3DAxis *Abstract3DController::axisZ() const
{
    return m_axisZ;
}

void Abstract3DController::addAxis(QAbstract3DAxis *axis)
{
    Q_ASSERT(axis);
    Abstract3DController *owner = qobject_cast<Abstract3DController *>(axis->parent());
    if (owner != this) {
        Q_ASSERT_X(!owner, "addAxis", "Axis already attached to a graph.");
        axis->setParent(this);
    }
    if (!m_axes.contains(axis))
        m_axes.append(axis);
}

void Abstract3DController::releaseAxis(QAbstract3DAxis *axis)
{
    if (axis && m_axes.contains(axis)) {
        // Clear the default status from released default axes
        if (axis->d_ptr->isDefaultAxis())
            axis->d_ptr->setDefaultAxis(false);

        // If the axis is in use, replace it with a temporary one
        switch (axis->orientation()) {
        case QAbstract3DAxis::AxisOrientationX:
            setAxisX(0);
            break;
        case QAbstract3DAxis::AxisOrientationY:
            setAxisY(0);
            break;
        case QAbstract3DAxis::AxisOrientationZ:
            setAxisZ(0);
            break;
        default:
            break;
        }

        m_axes.removeAll(axis);
        axis->setParent(0);
    }
}

QList<QAbstract3DAxis *> Abstract3DController::axes() const
{
    return m_axes;
}

void Abstract3DController::addInputHandler(QAbstract3DInputHandler *inputHandler)
{
    Q_ASSERT(inputHandler);
    Abstract3DController *owner = qobject_cast<Abstract3DController *>(inputHandler->parent());
    if (owner != this) {
        Q_ASSERT_X(!owner, "addInputHandler",
                   "Input handler already attached to another component.");
        inputHandler->setParent(this);
    }

    if (!m_inputHandlers.contains(inputHandler))
        m_inputHandlers.append(inputHandler);
}

void Abstract3DController::releaseInputHandler(QAbstract3DInputHandler *inputHandler)
{
    if (inputHandler && m_inputHandlers.contains(inputHandler)) {
        // Clear the default status from released default input handler
        if (inputHandler->d_ptr->m_isDefaultHandler)
            inputHandler->d_ptr->m_isDefaultHandler = false;

        // If the input handler is in use, remove it
        if (m_activeInputHandler == inputHandler)
            setActiveInputHandler(0);

        m_inputHandlers.removeAll(inputHandler);
        inputHandler->setParent(0);
    }
}

void Abstract3DController::setActiveInputHandler(QAbstract3DInputHandler *inputHandler)
{
    if (inputHandler == m_activeInputHandler)
        return;

    // If existing input handler is the default input handler, delete it
    if (m_activeInputHandler) {
        if (m_activeInputHandler->d_ptr->m_isDefaultHandler) {
            m_inputHandlers.removeAll(m_activeInputHandler);
            delete m_activeInputHandler;
        } else {
            // Disconnect the old input handler
            m_activeInputHandler->setScene(0);
            QObject::disconnect(m_activeInputHandler, 0, this, 0);
        }
    }

    // Assume ownership and connect to this controller's scene
    if (inputHandler)
        addInputHandler(inputHandler);

    m_activeInputHandler = inputHandler;
    if (m_activeInputHandler) {
        m_activeInputHandler->setScene(m_scene);

        // Connect the input handler
        QObject::connect(m_activeInputHandler, &QAbstract3DInputHandler::inputViewChanged, this,
                         &Abstract3DController::handleInputViewChanged);
        QObject::connect(m_activeInputHandler, &QAbstract3DInputHandler::positionChanged, this,
                         &Abstract3DController::handleInputPositionChanged);
    }

    // Notify change of input handler
    emit activeInputHandlerChanged(m_activeInputHandler);
}

QAbstract3DInputHandler *Abstract3DController::activeInputHandler()
{
    return m_activeInputHandler;
}

QList<QAbstract3DInputHandler *> Abstract3DController::inputHandlers() const
{
    return m_inputHandlers;
}

void Abstract3DController::addTheme(Q3DTheme *theme)
{
    m_themeManager->addTheme(theme);
}

void Abstract3DController::releaseTheme(Q3DTheme *theme)
{
    Q3DTheme *oldTheme = m_themeManager->activeTheme();

    m_themeManager->releaseTheme(theme);

    if (oldTheme != m_themeManager->activeTheme())
        emit activeThemeChanged(m_themeManager->activeTheme());
}

QList<Q3DTheme *> Abstract3DController::themes() const
{
    return m_themeManager->themes();
}

void Abstract3DController::setActiveTheme(Q3DTheme *theme, bool force)
{
    if (theme != m_themeManager->activeTheme()) {
        m_themeManager->setActiveTheme(theme);
        m_changeTracker.themeChanged = true;
        // Default theme can be created by theme manager, so ensure we have correct theme
        Q3DTheme *newActiveTheme = m_themeManager->activeTheme();
        // Reset all attached series to the new theme
        for (int i = 0; i < m_seriesList.size(); i++)
            m_seriesList.at(i)->d_ptr->resetToTheme(*newActiveTheme, i, force);
        markSeriesVisualsDirty();
        emit activeThemeChanged(newActiveTheme);
    }
}

Q3DTheme *Abstract3DController::activeTheme() const
{
    return m_themeManager->activeTheme();
}

void Abstract3DController::setSelectionMode(QAbstract3DGraphNG::SelectionFlags mode)
{
    if (mode != m_selectionMode) {
        m_selectionMode = mode;
        m_changeTracker.selectionModeChanged = true;
        emit selectionModeChanged(mode);
        emitNeedRender();
    }
}

QAbstract3DGraphNG::SelectionFlags Abstract3DController::selectionMode() const
{
    return m_selectionMode;
}

void Abstract3DController::setShadowQuality(QAbstract3DGraphNG::ShadowQuality quality)
{
    if (!m_useOrthoProjection)
        doSetShadowQuality(quality);
}

void Abstract3DController::doSetShadowQuality(QAbstract3DGraphNG::ShadowQuality quality)
{
    if (quality != m_shadowQuality) {
        m_shadowQuality = quality;
        m_changeTracker.shadowQualityChanged = true;
        emit shadowQualityChanged(m_shadowQuality);
        emitNeedRender();
    }
}

QAbstract3DGraphNG::ShadowQuality Abstract3DController::shadowQuality() const
{
    return m_shadowQuality;
}

void Abstract3DController::setOptimizationHints(QAbstract3DGraphNG::OptimizationHints hints)
{
    if (hints != m_optimizationHints) {
        m_optimizationHints = hints;
        m_changeTracker.optimizationHintChanged = true;
        m_isDataDirty = true;
        emit optimizationHintsChanged(hints);
        emitNeedRender();
    }
}

QAbstract3DGraphNG::OptimizationHints Abstract3DController::optimizationHints() const
{
    return m_optimizationHints;
}

bool Abstract3DController::shadowsSupported() const
{
    return !isOpenGLES();
}

bool Abstract3DController::isSlicingActive() const
{
    return m_scene->isSlicingActive();
}

void Abstract3DController::setSlicingActive(bool isSlicing)
{
    m_scene->setSlicingActive(isSlicing);
}

Q3DScene *Abstract3DController::scene()
{
    return m_scene;
}

void Abstract3DController::markDataDirty()
{
    m_isDataDirty = true;

    markSeriesItemLabelsDirty();
    emitNeedRender();
}

void Abstract3DController::markSeriesVisualsDirty()
{
    m_isSeriesVisualsDirty = true;
    emitNeedRender();
}

int Abstract3DController::addCustomItem(QCustom3DItem *item)
{
    if (!item)
        return -1;

    int index = m_customItems.indexOf(item);

    if (index != -1)
        return index;

    item->setParent(this);
    connect(item->d_ptr.data(), &QCustom3DItemPrivate::needUpdate,
            this, &Abstract3DController::updateCustomItem);
    m_customItems.append(item);
    item->d_ptr->resetDirtyBits();
    m_isCustomDataDirty = true;
    emitNeedRender();
    return m_customItems.size() - 1;
}

void Abstract3DController::deleteCustomItems()
{
    foreach (QCustom3DItem *item, m_customItems)
        delete item;
    m_customItems.clear();
    m_isCustomDataDirty = true;
    emitNeedRender();
}

void Abstract3DController::deleteCustomItem(QCustom3DItem *item)
{
    if (!item)
        return;

    m_customItems.removeOne(item);
    delete item;
    item = 0;
    m_isCustomDataDirty = true;
    emitNeedRender();
}

void Abstract3DController::deleteCustomItem(const QVector3D &position)
{
    // Get the item for the position
    foreach (QCustom3DItem *item, m_customItems) {
        if (item->position() == position)
            deleteCustomItem(item);
    }
}

void Abstract3DController::releaseCustomItem(QCustom3DItem *item)
{
    if (item && m_customItems.contains(item)) {
        disconnect(item->d_ptr.data(), &QCustom3DItemPrivate::needUpdate,
                   this, &Abstract3DController::updateCustomItem);
        m_customItems.removeOne(item);
        item->setParent(0);
        m_isCustomDataDirty = true;
        emitNeedRender();
    }
}

QList<QCustom3DItem *> Abstract3DController::customItems() const
{
    return m_customItems;
}

void Abstract3DController::updateCustomItem()
{
    m_isCustomItemDirty = true;
    emitNeedRender();
}

void Abstract3DController::handleAxisTitleChanged(const QString &title)
{
    Q_UNUSED(title);
    handleAxisTitleChangedBySender(sender());
}

void Abstract3DController::handleAxisTitleChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXTitleChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYTitleChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZTitleChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";

    markSeriesItemLabelsDirty();
    emitNeedRender();
}

void Abstract3DController::handleAxisLabelsChanged()
{
    handleAxisLabelsChangedBySender(sender());
}

void Abstract3DController::handleAxisLabelsChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXLabelsChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYLabelsChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZLabelsChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";

    markSeriesItemLabelsDirty();
    emitNeedRender();
}

void Abstract3DController::handleAxisRangeChanged(float min, float max)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
    handleAxisRangeChangedBySender(sender());
}

void Abstract3DController::handleAxisRangeChangedBySender(QObject *sender)
{
    if (sender == m_axisX) {
        m_isDataDirty = true;
        m_changeTracker.axisXRangeChanged = true;
    } else if (sender == m_axisY) {
        m_isDataDirty = true;
        m_changeTracker.axisYRangeChanged = true;
    } else if (sender == m_axisZ) {
        m_isDataDirty = true;
        m_changeTracker.axisZRangeChanged = true;
    } else {
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    }
    emitNeedRender();
}

void Abstract3DController::handleAxisSegmentCountChanged(int count)
{
    Q_UNUSED(count);
    handleAxisSegmentCountChangedBySender(sender());
}

void Abstract3DController::handleAxisSegmentCountChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXSegmentCountChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYSegmentCountChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZSegmentCountChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    emitNeedRender();
}

void Abstract3DController::handleAxisSubSegmentCountChanged(int count)
{
    Q_UNUSED(count);
    handleAxisSubSegmentCountChangedBySender(sender());
}

void Abstract3DController::handleAxisSubSegmentCountChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXSubSegmentCountChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYSubSegmentCountChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZSubSegmentCountChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    emitNeedRender();
}

void Abstract3DController::handleAxisAutoAdjustRangeChanged(bool autoAdjust)
{
    QObject *sender = QObject::sender();
    if (sender != m_axisX && sender != m_axisY && sender != m_axisZ)
        return;

    QAbstract3DAxis *axis = static_cast<QAbstract3DAxis*>(sender);
    handleAxisAutoAdjustRangeChangedInOrientation(axis->orientation(), autoAdjust);
}

void Abstract3DController::handleAxisLabelFormatChanged(const QString &format)
{
    Q_UNUSED(format);
    handleAxisLabelFormatChangedBySender(sender());
}

void Abstract3DController::handleAxisReversedChanged(bool enable)
{
    Q_UNUSED(enable);
    handleAxisReversedChangedBySender(sender());
}

void Abstract3DController::handleAxisFormatterDirty()
{
    handleAxisFormatterDirtyBySender(sender());
}

void Abstract3DController::handleAxisLabelAutoRotationChanged(float angle)
{
    Q_UNUSED(angle);
    handleAxisLabelAutoRotationChangedBySender(sender());
}

void Abstract3DController::handleAxisTitleVisibilityChanged(bool visible)
{
    Q_UNUSED(visible);
    handleAxisTitleVisibilityChangedBySender(sender());
}

void Abstract3DController::handleAxisTitleFixedChanged(bool fixed)
{
    Q_UNUSED(fixed);
    handleAxisTitleFixedChangedBySender(sender());
}

void Abstract3DController::handleInputViewChanged(QAbstract3DInputHandler::InputView view)
{
    // When in automatic slicing mode, input view change to primary disables slice mode
    if (m_selectionMode.testFlag(QAbstract3DGraphNG::SelectionSlice)
            && view == QAbstract3DInputHandler::InputViewOnPrimary) {
        setSlicingActive(false);
    }

    emitNeedRender();
}

void Abstract3DController::handleInputPositionChanged(const QPoint &position)
{
    Q_UNUSED(position);
    emitNeedRender();
}

void Abstract3DController::handleSeriesVisibilityChanged(bool visible)
{
    Q_UNUSED(visible);

    handleSeriesVisibilityChangedBySender(sender());
}

void Abstract3DController::handleRequestShadowQuality(QAbstract3DGraphNG::ShadowQuality quality)
{
    setShadowQuality(quality);
}

void Abstract3DController::setMeasureFps(bool enable)
{
    if (m_measureFps != enable) {
        m_measureFps = enable;
        m_currentFps = 0.0;

        if (enable) {
            m_frameTimer.start();
            m_numFrames = -1;
            emitNeedRender();
        }
        emit measureFpsChanged(enable);
    }
}

void Abstract3DController::handleAxisLabelFormatChangedBySender(QObject *sender)
{
    // Label format changing needs to dirty the data so that labels are reset.
    if (sender == m_axisX) {
        m_isDataDirty = true;
        m_changeTracker.axisXLabelFormatChanged = true;
    } else if (sender == m_axisY) {
        m_isDataDirty = true;
        m_changeTracker.axisYLabelFormatChanged = true;
    } else if (sender == m_axisZ) {
        m_isDataDirty = true;
        m_changeTracker.axisZLabelFormatChanged = true;
    } else {
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    }
    emitNeedRender();
}

void Abstract3DController::handleAxisReversedChangedBySender(QObject *sender)
{
    // Reversing change needs to dirty the data so item positions are recalculated
    if (sender == m_axisX) {
        m_isDataDirty = true;
        m_changeTracker.axisXReversedChanged = true;
    } else if (sender == m_axisY) {
        m_isDataDirty = true;
        m_changeTracker.axisYReversedChanged = true;
    } else if (sender == m_axisZ) {
        m_isDataDirty = true;
        m_changeTracker.axisZReversedChanged = true;
    } else {
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    }
    emitNeedRender();
}

void Abstract3DController::handleAxisFormatterDirtyBySender(QObject *sender)
{
    // Sender is QValue3DAxisPrivate
    QValue3DAxis *valueAxis = static_cast<QValue3DAxisPrivate *>(sender)->qptr();
    if (valueAxis == m_axisX) {
        m_isDataDirty = true;
        m_changeTracker.axisXFormatterChanged = true;
    } else if (valueAxis == m_axisY) {
        m_isDataDirty = true;
        m_changeTracker.axisYFormatterChanged = true;
    } else if (valueAxis == m_axisZ) {
        m_isDataDirty = true;
        m_changeTracker.axisZFormatterChanged = true;
    } else {
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
    }
    emitNeedRender();
}

void Abstract3DController::handleAxisLabelAutoRotationChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXLabelAutoRotationChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYLabelAutoRotationChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZLabelAutoRotationChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";

    emitNeedRender();
}

void Abstract3DController::handleAxisTitleVisibilityChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXTitleVisibilityChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYTitleVisibilityChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZTitleVisibilityChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";

    emitNeedRender();
}

void Abstract3DController::handleAxisTitleFixedChangedBySender(QObject *sender)
{
    if (sender == m_axisX)
        m_changeTracker.axisXTitleFixedChanged = true;
    else if (sender == m_axisY)
        m_changeTracker.axisYTitleFixedChanged = true;
    else if (sender == m_axisZ)
        m_changeTracker.axisZTitleFixedChanged = true;
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";

    emitNeedRender();
}

void Abstract3DController::handleSeriesVisibilityChangedBySender(QObject *sender)
{
    QAbstract3DSeries *series = static_cast<QAbstract3DSeries *>(sender);
    series->d_ptr->m_changeTracker.visibilityChanged = true;

    m_isDataDirty = true;
    m_isSeriesVisualsDirty = true;

    adjustAxisRanges();

    emitNeedRender();
}

void Abstract3DController::markSeriesItemLabelsDirty()
{
    for (int i = 0; i < m_seriesList.size(); i++)
        m_seriesList.at(i)->d_ptr->markItemLabelDirty();
}

bool Abstract3DController::isOpenGLES() const
{
    return Utils::isOpenGLES();
}

void Abstract3DController::setAxisHelper(QAbstract3DAxis::AxisOrientation orientation,
                                         QAbstract3DAxis *axis, QAbstract3DAxis **axisPtr)
{
    // Setting null axis indicates using default axis
    if (!axis)
        axis = createDefaultAxis(orientation);

    // If old axis is default axis, delete it
    QAbstract3DAxis *oldAxis = *axisPtr;
    if (oldAxis) {
        if (oldAxis->d_ptr->isDefaultAxis()) {
            m_axes.removeAll(oldAxis);
            delete oldAxis;
            oldAxis = 0;
        } else {
            // Disconnect the old axis from use
            QObject::disconnect(oldAxis, 0, this, 0);
            oldAxis->d_ptr->setOrientation(QAbstract3DAxis::AxisOrientationNone);
        }
    }

    // Assume ownership
    addAxis(axis);

    // Connect the new axis
    *axisPtr = axis;

    axis->d_ptr->setOrientation(orientation);

    QObject::connect(axis, &QAbstract3DAxis::titleChanged,
                     this, &Abstract3DController::handleAxisTitleChanged);
    QObject::connect(axis, &QAbstract3DAxis::labelsChanged,
                     this, &Abstract3DController::handleAxisLabelsChanged);
    QObject::connect(axis, &QAbstract3DAxis::rangeChanged,
                     this, &Abstract3DController::handleAxisRangeChanged);
    QObject::connect(axis, &QAbstract3DAxis::autoAdjustRangeChanged,
                     this, &Abstract3DController::handleAxisAutoAdjustRangeChanged);
    QObject::connect(axis, &QAbstract3DAxis::labelAutoRotationChanged,
                     this, &Abstract3DController::handleAxisLabelAutoRotationChanged);
    QObject::connect(axis, &QAbstract3DAxis::titleVisibilityChanged,
                     this, &Abstract3DController::handleAxisTitleVisibilityChanged);
    QObject::connect(axis, &QAbstract3DAxis::titleFixedChanged,
                     this, &Abstract3DController::handleAxisTitleFixedChanged);

    if (orientation == QAbstract3DAxis::AxisOrientationX)
        m_changeTracker.axisXTypeChanged = true;
    else if (orientation == QAbstract3DAxis::AxisOrientationY)
        m_changeTracker.axisYTypeChanged = true;
    else if (orientation == QAbstract3DAxis::AxisOrientationZ)
        m_changeTracker.axisZTypeChanged = true;

    handleAxisTitleChangedBySender(axis);
    handleAxisLabelsChangedBySender(axis);
    handleAxisRangeChangedBySender(axis);
    handleAxisAutoAdjustRangeChangedInOrientation(axis->orientation(),
                                                  axis->isAutoAdjustRange());
    handleAxisLabelAutoRotationChangedBySender(axis);
    handleAxisTitleVisibilityChangedBySender(axis);
    handleAxisTitleFixedChangedBySender(axis);

    if (axis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(axis);
        QObject::connect(valueAxis, &QValue3DAxis::segmentCountChanged,
                         this, &Abstract3DController::handleAxisSegmentCountChanged);
        QObject::connect(valueAxis, &QValue3DAxis::subSegmentCountChanged,
                         this, &Abstract3DController::handleAxisSubSegmentCountChanged);
        QObject::connect(valueAxis, &QValue3DAxis::labelFormatChanged,
                         this, &Abstract3DController::handleAxisLabelFormatChanged);
        QObject::connect(valueAxis, &QValue3DAxis::reversedChanged,
                         this, &Abstract3DController::handleAxisReversedChanged);
        QObject::connect(valueAxis->dptr(), &QValue3DAxisPrivate::formatterDirty,
                         this, &Abstract3DController::handleAxisFormatterDirty);

        handleAxisSegmentCountChangedBySender(valueAxis);
        handleAxisSubSegmentCountChangedBySender(valueAxis);
        handleAxisLabelFormatChangedBySender(valueAxis);
        handleAxisReversedChangedBySender(valueAxis);
        handleAxisFormatterDirtyBySender(valueAxis->dptr());

        valueAxis->formatter()->setLocale(m_locale);
    }
}

QAbstract3DAxis *Abstract3DController::createDefaultAxis(
        QAbstract3DAxis::AxisOrientation orientation)
{
    Q_UNUSED(orientation);

    // The default default axis is a value axis. If the graph type has a different default axis
    // for some orientation, this function needs to be overridden.
    QAbstract3DAxis *defaultAxis = createDefaultValueAxis();
    return defaultAxis;
}

QValue3DAxis *Abstract3DController::createDefaultValueAxis()
{
    // Default value axis has single segment, empty label format, and auto scaling
    QValue3DAxis *defaultAxis = new QValue3DAxis;
    defaultAxis->d_ptr->setDefaultAxis(true);

    return defaultAxis;
}

QCategory3DAxis *Abstract3DController::createDefaultCategoryAxis()
{
    // Default category axis has no labels
    QCategory3DAxis *defaultAxis = new QCategory3DAxis;
    defaultAxis->d_ptr->setDefaultAxis(true);
    return defaultAxis;
}

void Abstract3DController::startRecordingRemovesAndInserts()
{
    // Default implementation does nothing
}

void Abstract3DController::emitNeedRender()
{
    if (!m_renderPending) {
        emit needRender();
        m_renderPending = true;
    }
}

int Abstract3DController::selectedLabelIndex() const
{
    int index = m_selectedLabelIndex;
    QAbstract3DAxis *axis = selectedAxis();
    if (axis && axis->labels().size() <= index)
        index = -1;
    return index;
}

QAbstract3DAxis *Abstract3DController::selectedAxis() const
{
    QAbstract3DAxis *axis = 0;
    QAbstract3DGraphNG::ElementType type = m_clickedType;
    switch (type) {
    case QAbstract3DGraphNG::ElementAxisXLabel:
        axis = axisX();
        break;
    case QAbstract3DGraphNG::ElementAxisYLabel:
        axis = axisY();
        break;
    case QAbstract3DGraphNG::ElementAxisZLabel:
        axis = axisZ();
        break;
    default:
        axis = 0;
        break;
    }

    return axis;
}

int Abstract3DController::selectedCustomItemIndex() const
{
    int index = m_selectedCustomItemIndex;
    if (m_customItems.size() <= index)
        index = -1;
    return index;
}

QCustom3DItem *Abstract3DController::selectedCustomItem() const
{
    QCustom3DItem *item = 0;
    int index = selectedCustomItemIndex();
    if (index >= 0)
        item = m_customItems[index];
    return item;
}

QAbstract3DGraphNG::ElementType Abstract3DController::selectedElement() const
{
    return m_clickedType;
}

void Abstract3DController::setOrthoProjection(bool enable)
{
    if (enable != m_useOrthoProjection) {
        m_useOrthoProjection = enable;
        m_changeTracker.projectionChanged = true;
        emit orthoProjectionChanged(m_useOrthoProjection);
        // If changed to ortho, disable shadows
        if (m_useOrthoProjection)
            doSetShadowQuality(QAbstract3DGraphNG::ShadowQualityNone);
        emitNeedRender();
    }
}

bool Abstract3DController::isOrthoProjection() const
{
    return m_useOrthoProjection;
}

void Abstract3DController::setAspectRatio(qreal ratio)
{
    if (m_aspectRatio != ratio) {
        m_aspectRatio = ratio;
        m_changeTracker.aspectRatioChanged = true;
        emit aspectRatioChanged(m_aspectRatio);
        m_isDataDirty = true;
        emitNeedRender();
    }
}

qreal Abstract3DController::aspectRatio()
{
    return m_aspectRatio;
}

void Abstract3DController::setHorizontalAspectRatio(qreal ratio)
{
    if (m_horizontalAspectRatio != ratio) {
        m_horizontalAspectRatio = ratio;
        m_changeTracker.horizontalAspectRatioChanged = true;
        emit horizontalAspectRatioChanged(m_horizontalAspectRatio);
        m_isDataDirty = true;
        emitNeedRender();
    }
}

qreal Abstract3DController::horizontalAspectRatio() const
{
    return m_horizontalAspectRatio;
}

void Abstract3DController::setReflection(bool enable)
{
    if (m_reflectionEnabled != enable) {
        m_reflectionEnabled = enable;
        m_changeTracker.reflectionChanged = true;
        emit reflectionChanged(m_reflectionEnabled);
        emitNeedRender();
    }
}

bool Abstract3DController::reflection() const
{
    return m_reflectionEnabled;
}

void Abstract3DController::setReflectivity(qreal reflectivity)
{
    if (m_reflectivity != reflectivity) {
        m_reflectivity = reflectivity;
        m_changeTracker.reflectivityChanged = true;
        emit reflectivityChanged(m_reflectivity);
        emitNeedRender();
    }
}

qreal Abstract3DController::reflectivity() const
{
    return m_reflectivity;
}

void Abstract3DController::setPolar(bool enable)
{
    if (enable != m_isPolar) {
        m_isPolar = enable;
        m_changeTracker.polarChanged = true;
        m_isDataDirty = true;
        emit polarChanged(m_isPolar);
        emitNeedRender();
    }
}

bool Abstract3DController::isPolar() const
{
    return m_isPolar;
}

void Abstract3DController::setRadialLabelOffset(float offset)
{
    if (m_radialLabelOffset != offset) {
        m_radialLabelOffset = offset;
        m_changeTracker.radialLabelOffsetChanged = true;
        emit radialLabelOffsetChanged(m_radialLabelOffset);
        emitNeedRender();
    }
}

float Abstract3DController::radialLabelOffset() const
{
    return m_radialLabelOffset;
}

void Abstract3DController::setLocale(const QLocale &locale)
{
    if (m_locale != locale) {
        m_locale = locale;

        // Value axis formatters need to be updated
        QValue3DAxis *axis = qobject_cast<QValue3DAxis *>(m_axisX);
        if (axis)
            axis->formatter()->setLocale(m_locale);
        axis = qobject_cast<QValue3DAxis *>(m_axisY);
        if (axis)
            axis->formatter()->setLocale(m_locale);
        axis = qobject_cast<QValue3DAxis *>(m_axisZ);
        if (axis)
            axis->formatter()->setLocale(m_locale);
        emit localeChanged(m_locale);
    }
}

QLocale Abstract3DController::locale() const
{
    return m_locale;
}

QVector3D Abstract3DController::queriedGraphPosition() const
{
    return m_queriedGraphPosition;
}

void Abstract3DController::setMargin(qreal margin)
{
    if (m_margin != margin) {
        m_margin = margin;
        m_changeTracker.marginChanged = true;
        emit marginChanged(margin);
        emitNeedRender();
    }
}

qreal Abstract3DController::margin() const
{
    return m_margin;
}


QT_END_NAMESPACE

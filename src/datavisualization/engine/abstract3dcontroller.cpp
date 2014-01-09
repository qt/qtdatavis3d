/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
**
** Licensees holding valid Qt Enterprise licenses may use this file in
** accordance with the Qt Enterprise License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
**
****************************************************************************/

#include "abstract3dcontroller_p.h"
#include "camerahelper_p.h"
#include "qabstract3daxis_p.h"
#include "qvalue3daxis.h"
#include "qcategory3daxis.h"
#include "abstract3drenderer_p.h"
#include "q3dcamera.h"
#include "q3dlight.h"
#include "qabstractdataproxy_p.h"
#include "qabstract3dinputhandler_p.h"
#include "qtouch3dinputhandler.h"
#include "qabstract3dseries_p.h"
#include "thememanager_p.h"
#include "q3dscene_p.h"

#include <QThread>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Abstract3DController::Abstract3DController(QRect initialViewport, Q3DScene *scene, QObject *parent) :
    QObject(parent),
    m_themeManager(new ThemeManager(this)),
    m_selectionMode(QDataVis::SelectionItem),
    m_shadowQuality(QDataVis::ShadowQualityMedium),
    m_scene(scene),
    m_activeInputHandler(0),
    m_axisX(0),
    m_axisY(0),
    m_axisZ(0),
    m_renderer(0),
    m_isDataDirty(true),
    m_isSeriesVisibilityDirty(true),
    m_isSeriesVisualsDirty(true),
    m_renderPending(false)
{
    if (!m_scene)
        m_scene = new Q3DScene;

    // Set initial theme
    setActiveTheme(new Q3DTheme(Q3DTheme::ThemeQt));

    m_scene->d_ptr->setViewport(initialViewport);

    // Populate the scene
    m_scene->activeLight()->setPosition(defaultLightPos);

    // Create initial default input handler
    QAbstract3DInputHandler *inputHandler;
    inputHandler = new QTouch3DInputHandler();
    inputHandler->d_ptr->m_isDefaultHandler = true;
    setActiveInputHandler(inputHandler);
    connect(inputHandler, &QAbstract3DInputHandler::inputStateChanged, this,
            &Abstract3DController::handleInputStateChanged);
    connect(inputHandler, &QAbstract3DInputHandler::positionChanged, this,
            &Abstract3DController::handleInputPositionChanged);
    connect(m_scene->d_ptr.data(), &Q3DScenePrivate::needRender, this,
            &Abstract3DController::emitNeedRender);
}

Abstract3DController::~Abstract3DController()
{
    // Renderer can be in another thread, don't delete it directly in that case
    if (m_renderer && m_renderer->thread() != QThread::currentThread())
        m_renderer->deleteLater();
    else
        delete m_renderer;
    delete m_scene;
    delete m_themeManager;
}

/**
 * @brief setRenderer Sets the renderer to be used. isInitialized returns true from this point onwards.
 * @param renderer Renderer to be used.
 */
void Abstract3DController::setRenderer(Abstract3DRenderer *renderer)
{
    m_renderer = renderer;
}

void Abstract3DController::addSeries(QAbstract3DSeries *series)
{
    if (series && !m_seriesList.contains(series)) {
        int oldSize = m_seriesList.size();
        m_seriesList.append(series);
        series->d_ptr->setController(this);
        QObject::connect(series, &QAbstract3DSeries::visibilityChanged,
                         this, &Abstract3DController::handleSeriesVisibilityChanged);
        if (series->isVisible())
            handleSeriesVisibilityChangedBySender(series);
        series->d_ptr->resetToTheme(*m_themeManager->activeTheme(), oldSize, false);
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
        m_isSeriesVisibilityDirty = true;
        emitNeedRender();
    }
}

QList<QAbstract3DSeries *> Abstract3DController::seriesList()
{
    return m_seriesList;
}

/**
 * @brief synchDataToRenderer Called on the render thread while main GUI thread is blocked before rendering.
 */
void Abstract3DController::synchDataToRenderer()
{
    // If we don't have a renderer, don't do anything
    if (!m_renderer)
        return;

    m_renderer->updateScene(m_scene);

    m_renderer->updateTheme(m_themeManager->activeTheme());

    if (m_changeTracker.shadowQualityChanged) {
        m_renderer->updateShadowQuality(m_shadowQuality);
        m_changeTracker.shadowQualityChanged = false;
    }

    if (m_changeTracker.selectionModeChanged) {
        m_renderer->updateSelectionMode(m_selectionMode);
        m_changeTracker.selectionModeChanged = false;
    }

    if (m_changeTracker.axisXTypeChanged) {
        m_renderer->updateAxisType(QAbstract3DAxis::AxisOrientationX, m_axisX->type());
        m_changeTracker.axisXTypeChanged = false;
    }

    if (m_changeTracker.axisYTypeChanged) {
        m_renderer->updateAxisType(QAbstract3DAxis::AxisOrientationY, m_axisY->type());
        m_changeTracker.axisYTypeChanged = false;
    }

    if (m_changeTracker.axisZTypeChanged) {
        m_renderer->updateAxisType(QAbstract3DAxis::AxisOrientationZ, m_axisZ->type());
        m_changeTracker.axisZTypeChanged = false;
    }

    if (m_changeTracker.axisXTitleChanged) {
        m_renderer->updateAxisTitle(QAbstract3DAxis::AxisOrientationX, m_axisX->title());
        m_changeTracker.axisXTitleChanged = false;
    }

    if (m_changeTracker.axisYTitleChanged) {
        m_renderer->updateAxisTitle(QAbstract3DAxis::AxisOrientationY, m_axisY->title());
        m_changeTracker.axisYTitleChanged = false;
    }

    if (m_changeTracker.axisZTitleChanged) {
        m_renderer->updateAxisTitle(QAbstract3DAxis::AxisOrientationZ, m_axisZ->title());
        m_changeTracker.axisZTitleChanged = false;
    }

    if (m_changeTracker.axisXLabelsChanged) {
        m_renderer->updateAxisLabels(QAbstract3DAxis::AxisOrientationX, m_axisX->labels());
        m_changeTracker.axisXLabelsChanged = false;
    }

    if (m_changeTracker.axisYLabelsChanged) {
        m_renderer->updateAxisLabels(QAbstract3DAxis::AxisOrientationY, m_axisY->labels());
        m_changeTracker.axisYLabelsChanged = false;
    }
    if (m_changeTracker.axisZLabelsChanged) {
        m_renderer->updateAxisLabels(QAbstract3DAxis::AxisOrientationZ, m_axisZ->labels());
        m_changeTracker.axisZLabelsChanged = false;
    }

    if (m_changeTracker.axisXRangeChanged) {
        m_renderer->updateAxisRange(QAbstract3DAxis::AxisOrientationX, m_axisX->min(),
                                    m_axisX->max());
        m_changeTracker.axisXRangeChanged = false;
    }

    if (m_changeTracker.axisYRangeChanged) {
        m_renderer->updateAxisRange(QAbstract3DAxis::AxisOrientationY, m_axisY->min(),
                                    m_axisY->max());
        m_changeTracker.axisYRangeChanged = false;
    }

    if (m_changeTracker.axisZRangeChanged) {
        m_renderer->updateAxisRange(QAbstract3DAxis::AxisOrientationZ, m_axisZ->min(),
                                    m_axisZ->max());
        m_changeTracker.axisZRangeChanged = false;
    }

    if (m_changeTracker.axisXSegmentCountChanged) {
        m_changeTracker.axisXSegmentCountChanged = false;
        if (m_axisX->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisX = static_cast<QValue3DAxis *>(m_axisX);
            m_renderer->updateAxisSegmentCount(QAbstract3DAxis::AxisOrientationX,
                                               valueAxisX->segmentCount());
        }
    }

    if (m_changeTracker.axisYSegmentCountChanged) {
        m_changeTracker.axisYSegmentCountChanged = false;
        if (m_axisY->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisY = static_cast<QValue3DAxis *>(m_axisY);
            m_renderer->updateAxisSegmentCount(QAbstract3DAxis::AxisOrientationY,
                                               valueAxisY->segmentCount());
        }
    }

    if (m_changeTracker.axisZSegmentCountChanged) {
        m_changeTracker.axisZSegmentCountChanged = false;
        if (m_axisZ->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisZ = static_cast<QValue3DAxis *>(m_axisZ);
            m_renderer->updateAxisSegmentCount(QAbstract3DAxis::AxisOrientationZ,
                                               valueAxisZ->segmentCount());
        }
    }

    if (m_changeTracker.axisXSubSegmentCountChanged) {
        m_changeTracker.axisXSubSegmentCountChanged = false;
        if (m_axisX->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisX = static_cast<QValue3DAxis *>(m_axisX);
            m_renderer->updateAxisSubSegmentCount(QAbstract3DAxis::AxisOrientationX,
                                                  valueAxisX->subSegmentCount());
        }
    }

    if (m_changeTracker.axisYSubSegmentCountChanged) {
        m_changeTracker.axisYSubSegmentCountChanged = false;
        if (m_axisY->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisY = static_cast<QValue3DAxis *>(m_axisY);
            m_renderer->updateAxisSubSegmentCount(QAbstract3DAxis::AxisOrientationY,
                                                  valueAxisY->subSegmentCount());
        }
    }

    if (m_changeTracker.axisZSubSegmentCountChanged) {
        m_changeTracker.axisZSubSegmentCountChanged = false;
        if (m_axisZ->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisZ = static_cast<QValue3DAxis *>(m_axisZ);
            m_renderer->updateAxisSubSegmentCount(QAbstract3DAxis::AxisOrientationZ,
                                                  valueAxisZ->subSegmentCount());
        }
    }

    if (m_changeTracker.axisXLabelFormatChanged) {
        m_changeTracker.axisXLabelFormatChanged = false;
        if (m_axisX->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisX = static_cast<QValue3DAxis *>(m_axisX);
            m_renderer->updateAxisLabelFormat(QAbstract3DAxis::AxisOrientationX,
                                              valueAxisX->labelFormat());
        }
    }

    if (m_changeTracker.axisYLabelFormatChanged) {
        m_changeTracker.axisYLabelFormatChanged = false;
        if (m_axisY->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisY = static_cast<QValue3DAxis *>(m_axisY);
            m_renderer->updateAxisLabelFormat(QAbstract3DAxis::AxisOrientationY,
                                              valueAxisY->labelFormat());
        }
    }

    if (m_changeTracker.axisZLabelFormatChanged) {
        m_changeTracker.axisZLabelFormatChanged = false;
        if (m_axisZ->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisZ = static_cast<QValue3DAxis *>(m_axisZ);
            m_renderer->updateAxisLabelFormat(QAbstract3DAxis::AxisOrientationZ,
                                              valueAxisZ->labelFormat());
        }
    }

    if (m_isSeriesVisibilityDirty || m_isSeriesVisualsDirty) {
        m_renderer->updateSeries(m_seriesList, m_isSeriesVisibilityDirty);
        m_isSeriesVisibilityDirty = false;
        m_isSeriesVisualsDirty = false;
    }

    if (m_isDataDirty) {
        // Series list supplied above in updateSeries() is used to access the data,
        // so no data needs to be passed in updateData()
        m_renderer->updateData();
        m_isDataDirty = false;
    }
}

void Abstract3DController::render(const GLuint defaultFboHandle)
{
    m_renderPending = false;

    // If not initialized, do nothing.
    if (!m_renderer)
        return;

    m_renderer->render(defaultFboHandle);

#ifdef DISPLAY_RENDER_SPEED
    // To get meaningful framerate, don't just do render on demand.
    emitNeedRender();
#endif
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

void Abstract3DController::wheelEvent(QWheelEvent *event)
{
    if (m_activeInputHandler)
        m_activeInputHandler->wheelEvent(event);
}

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

void Abstract3DController::handleThemeSingleHighlightGradientChanged(const QLinearGradient &gradient)
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
    Q_UNUSED(theme)

    // Changing theme type is logically equivalent of changing the entire theme
    // object, so reset all attached series to the new theme.

    Q3DTheme *activeTheme = m_themeManager->activeTheme();
    for (int i = 0; i < m_seriesList.size(); i++)
        m_seriesList.at(i)->d_ptr->resetToTheme(*activeTheme, i, true);
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

QAbstract3DAxis *Abstract3DController::axisX()
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

QAbstract3DAxis *Abstract3DController::axisY()
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

QAbstract3DAxis *Abstract3DController::axisZ()
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
        Q_ASSERT_X(!owner, "addInputHandler", "Input handler already attached to another component.");
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
            // Disconnect the old input handler from the scene
            m_activeInputHandler->setScene(0);
        }
    }

    // Assume ownership and connect to this controller's scene
    if (inputHandler)
        addInputHandler(inputHandler);

    m_activeInputHandler = inputHandler;
    if (m_activeInputHandler)
        m_activeInputHandler->setScene(m_scene);

    // Notify change of input handler
    emit activeInputHandlerChanged(m_activeInputHandler);
}

QAbstract3DInputHandler* Abstract3DController::activeInputHandler()
{
    return m_activeInputHandler;
}

QList<QAbstract3DInputHandler *> Abstract3DController::inputHandlers() const
{
    return m_inputHandlers;
}

int Abstract3DController::zoomLevel()
{
    return m_scene->activeCamera()->zoomLevel();
}

void Abstract3DController::setZoomLevel(int zoomLevel)
{
    m_scene->activeCamera()->setZoomLevel(zoomLevel);

    m_changeTracker.zoomLevelChanged = true;
    emitNeedRender();
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

void Abstract3DController::setActiveTheme(Q3DTheme *theme)
{
    if (theme != m_themeManager->activeTheme()) {
        m_themeManager->setActiveTheme(theme);
        m_changeTracker.themeChanged = true;
        // Default theme can be created by theme manager, so ensure we have correct theme
        Q3DTheme *newActiveTheme = m_themeManager->activeTheme();
        // Reset all attached series to the new theme
        for (int i = 0; i < m_seriesList.size(); i++)
            m_seriesList.at(i)->d_ptr->resetToTheme(*newActiveTheme, i, true);
        markSeriesVisualsDirty();
        emit activeThemeChanged(newActiveTheme);
    }
}

Q3DTheme *Abstract3DController::activeTheme() const
{
    return m_themeManager->activeTheme();
}

void Abstract3DController::setSelectionMode(QDataVis::SelectionFlags mode)
{
    if (mode != m_selectionMode) {
        m_selectionMode = mode;
        m_changeTracker.selectionModeChanged = true;
        emit selectionModeChanged(mode);
        emitNeedRender();
    }
}

QDataVis::SelectionFlags Abstract3DController::selectionMode() const
{
    return m_selectionMode;
}

void Abstract3DController::setShadowQuality(QDataVis::ShadowQuality quality)
{
    if (quality != m_shadowQuality) {
        m_shadowQuality = quality;
        m_changeTracker.shadowQualityChanged = true;
        emit shadowQualityChanged(m_shadowQuality);
        emitNeedRender();
    }
}

QDataVis::ShadowQuality Abstract3DController::shadowQuality() const
{
    return m_shadowQuality;
}

bool Abstract3DController::isSlicingActive() const
{
    return m_scene->isSlicingActive();
}

void Abstract3DController::setSlicingActive(bool isSlicing)
{
    m_scene->setSlicingActive(isSlicing);
    emitNeedRender();
}

Q3DScene *Abstract3DController::scene()
{
    return m_scene;
}

void Abstract3DController::markDataDirty()
{
    m_isDataDirty = true;
    emitNeedRender();
}

void Abstract3DController::markSeriesVisualsDirty()
{
    m_isSeriesVisualsDirty = true;
    emitNeedRender();
}

void Abstract3DController::handleAxisTitleChanged(const QString &title)
{
    Q_UNUSED(title)
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
    emitNeedRender();
}

void Abstract3DController::handleAxisRangeChanged(float min, float max)
{
    Q_UNUSED(min)
    Q_UNUSED(max)
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
    Q_UNUSED(count)
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
    Q_UNUSED(count)
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
    Q_UNUSED(format)
    handleAxisLabelFormatChangedBySender(sender());
}

void Abstract3DController::handleInputStateChanged(QAbstract3DInputHandler::InputState state)
{
    // When in automatic slicing mode, input state change to overview disables slice mode
    if (m_selectionMode.testFlag(QDataVis::SelectionSlice)
            && state == QAbstract3DInputHandler::InputStateOnPrimaryView) {
        setSlicingActive(false);
    }

    m_changeTracker.inputStateChanged = true;
    emitNeedRender();
}

void Abstract3DController::handleInputPositionChanged(const QPoint &position)
{
    Q_UNUSED(position)

    m_changeTracker.inputPositionChanged = true;
    emitNeedRender();
}

void Abstract3DController::handleSeriesVisibilityChanged(bool visible)
{
    Q_UNUSED(visible)

    handleSeriesVisibilityChangedBySender(sender());
}

void Abstract3DController::handleRequestShadowQuality(QDataVis::ShadowQuality quality)
{
    setShadowQuality(quality);
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

void Abstract3DController::handleSeriesVisibilityChangedBySender(QObject *sender)
{
    Q_UNUSED(sender)

    m_isDataDirty = true;
    m_isSeriesVisibilityDirty = true;
    emitNeedRender();
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

    if (axis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(axis);
        QObject::connect(valueAxis, &QValue3DAxis::segmentCountChanged,
                         this, &Abstract3DController::handleAxisSegmentCountChanged);
        QObject::connect(valueAxis, &QValue3DAxis::subSegmentCountChanged,
                         this, &Abstract3DController::handleAxisSubSegmentCountChanged);
        QObject::connect(valueAxis, &QValue3DAxis::labelFormatChanged,
                         this, &Abstract3DController::handleAxisLabelFormatChanged);

        handleAxisSegmentCountChangedBySender(valueAxis);
        handleAxisSubSegmentCountChangedBySender(valueAxis);
        handleAxisLabelFormatChangedBySender(valueAxis);
    }
}

QAbstract3DAxis *Abstract3DController::createDefaultAxis(QAbstract3DAxis::AxisOrientation orientation)
{
    Q_UNUSED(orientation)

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

void Abstract3DController::emitNeedRender()
{
    if (!m_renderPending) {
        emit needRender();
        m_renderPending = true;
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE

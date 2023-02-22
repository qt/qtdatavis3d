// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickdatavisitem_p.h"
#include "declarativetheme_p.h"
#include "declarativescene_p.h"
#include "qvalue3daxis_p.h"
#include "qcategory3daxis_p.h"
#include "q3dscene_p.h"
#include "abstract3dcontroller_p.h"
#include "utils_p.h"

#include <QtGui/QGuiApplication>

#include <QtQuick3D/private/qquick3dperspectivecamera_p.h>
#include <QtQuick3D/private/qquick3dorthographiccamera_p.h>
#include <QtQuick3D/private/qquick3dcustommaterial_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3ddirectionallight_p.h>
#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick/private/qquickitem_p.h>

#if defined(Q_OS_IOS)
#include <QtCore/QTimer>
#endif

#if defined(Q_OS_MACOS)
#include <qpa/qplatformnativeinterface.h>
#endif

QT_BEGIN_NAMESPACE

static QHash<QQuickDataVisItem *, QQuickWindow *> graphWindowList;

QQuickDataVisItem::QQuickDataVisItem(QQuickItem *parent) :
    QQuick3DViewport(parent),
    m_controller(0),
    m_renderMode(RenderIndirect),
    m_samples(0),
    m_windowSamples(0),
    m_initialisedSize(0, 0)
{
    m_nodeMutex = QSharedPointer<QMutex>::create();

    auto sceneManager = QQuick3DObjectPrivate::get(rootNode())->sceneManager;
    connect(sceneManager.data(), &QQuick3DSceneManager::windowChanged, this, &QQuickDataVisItem::handleWindowChanged);
    // Set contents to false in case we are in qml designer to make component look nice
    m_runningInDesigner = QGuiApplication::applicationDisplayName() == QLatin1String("Qml2Puppet");
    setFlag(ItemHasContents/*, !m_runningInDesigner*/); // Is this relevant anymore?

    // Accept touchevents
    setAcceptTouchEvents(true);
}

QQuickDataVisItem::~QQuickDataVisItem()
{
    disconnect(this, 0, this, 0);
    checkWindowList(0);

    // Make sure not deleting locked mutex
    QMutexLocker locker(&m_mutex);
    locker.unlock();

    m_nodeMutex.clear();
}

void QQuickDataVisItem::setRenderingMode(QQuickDataVisItem::RenderingMode mode)
{
    if (mode == m_renderMode)
        return;

    RenderingMode previousMode = m_renderMode;

    m_renderMode = mode;

    m_initialisedSize = QSize(0, 0);
    setFlag(ItemHasContents/*, !m_runningInDesigner*/);

    // TODO - Need to check if the mode is set properly
    switch (mode) {
    case RenderDirectToBackground:
        // Intentional flowthrough
    case RenderDirectToBackground_NoClear:
        update();
        setRenderMode(QQuick3DViewport::Underlay);
        if (previousMode == RenderIndirect) {
                checkWindowList(window());
                setAntialiasing(m_windowSamples > 0);
                if (m_windowSamples != m_samples)
                    emit msaaSamplesChanged(m_windowSamples);
        }
        break;
    case RenderIndirect:
        update();
        setRenderMode(QQuick3DViewport::Offscreen);
        break;
    }

    updateWindowParameters();

    emit renderingModeChanged(mode);
}

QQuickDataVisItem::RenderingMode QQuickDataVisItem::renderingMode() const
{
    return m_renderMode;
}

void QQuickDataVisItem::keyPressEvent(QKeyEvent *ev)
{
    ev->ignore();
    setFlag(ItemHasContents);
    update();
}

void QQuickDataVisItem::handleMousePressedEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button()) {
        if (m_sliceEnabled && m_controller->isSlicingActive()) {
            m_sliceEnabled = false;
            m_sliceActivatedChanged = true;
            return;
        }
        auto selectionMode = m_controller->selectionMode();
        if (selectionMode.testFlag(QAbstract3DGraphNG::SelectionSlice)
                && (selectionMode.testFlag(QAbstract3DGraphNG::SelectionColumn)
                    != selectionMode.testFlag(QAbstract3DGraphNG::SelectionRow))) {
            m_sliceEnabled = true;
        }
    }
}

void QQuickDataVisItem::handleFpsChanged()
{
    int fps = renderStats()->fps();
    emit currentFpsChanged(fps);
}

void QQuickDataVisItem::componentComplete()
{
    QQuick3DViewport::componentComplete();

    auto url = QUrl(QStringLiteral("defaultMeshes/backgroundMesh"));
    m_background = new QQuick3DModel();
    m_backgroundScale = new QQuick3DNode();
    m_backgroundRotation = new QQuick3DNode();

    m_backgroundScale->setParent(rootNode());
    m_backgroundScale->setParentItem(rootNode());

    m_backgroundRotation->setParent(m_backgroundScale);
    m_backgroundRotation->setParentItem(m_backgroundScale);

    m_background->setObjectName("Background");
    m_background->setParent(m_backgroundRotation);
    m_background->setParentItem(m_backgroundRotation);

    m_background->setSource(url);

    m_backgroundBB = new QQuick3DModel();
    m_backgroundBB->setObjectName("BackgroundBB");
    m_backgroundBB->setParent(m_background);
    m_backgroundBB->setParentItem(m_background);
    m_backgroundBB->setSource(QUrl(QStringLiteral("defaultMeshes/barMeshFull")));
    m_backgroundBB->setPickable(true);

    setUpCamera();
    setUpLight();

    // Create repeaters for each axis X, Y, Z
    m_repeaterX = createRepeater();
    m_repeaterY = createRepeater();
    m_repeaterZ = createRepeater();

    auto delegateModelX = createRepeaterDelegateComponent(QStringLiteral(":/axis/AxisLabel"));
    auto delegateModelY = createRepeaterDelegateComponent(QStringLiteral(":/axis/AxisLabel"));
    auto delegateModelZ = createRepeaterDelegateComponent(QStringLiteral(":/axis/AxisLabel"));

    m_repeaterX->setDelegate(delegateModelX);
    m_repeaterY->setDelegate(delegateModelY);
    m_repeaterZ->setDelegate(delegateModelZ);

    // title labels for axes
    m_titleLabelX = createTitleLabel();
    m_titleLabelY = createTitleLabel();
    m_titleLabelZ = createTitleLabel();

    // Testing gridline

    // X lines
    m_segmentLineRepeaterX = createRepeater();

    auto segmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    m_segmentLineRepeaterX->setDelegate(segmentLineDelegate);

    m_subsegmentLineRepeaterX = createRepeater();
    auto subsegmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    m_subsegmentLineRepeaterX->setDelegate(subsegmentLineDelegate);

    // Y lines
    m_segmentLineRepeaterY = createRepeater();
    segmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    m_segmentLineRepeaterY->setDelegate(segmentLineDelegate);

    m_subsegmentLineRepeaterY = createRepeater();
    subsegmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    m_subsegmentLineRepeaterY->setDelegate(subsegmentLineDelegate);

    // Z lines
    m_segmentLineRepeaterZ = createRepeater();

    segmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    m_segmentLineRepeaterZ->setDelegate(segmentLineDelegate);

    m_subsegmentLineRepeaterZ = createRepeater();
    subsegmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    m_subsegmentLineRepeaterZ->setDelegate(subsegmentLineDelegate);

    m_itemLabel = createTitleLabel();

    auto axis = m_controller->axisX();
    int segmentCount = 0;
    int subSegmentCount = 0;
    int gridLineCount = 0;
    int subGridLineCount = 0;
    if (axis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(axis);
        segmentCount = valueAxis->segmentCount();
        subSegmentCount = valueAxis->subSegmentCount();
        gridLineCount = 2 * (segmentCount + 1);
        subGridLineCount = 2 * (segmentCount * (subSegmentCount - 1));
    } else if (axis->type() & QAbstract3DAxis::AxisTypeCategory) {
        gridLineCount = axis->labels().size();
    }
    m_segmentLineRepeaterX->setModel(gridLineCount);
    m_subsegmentLineRepeaterX->setModel(subGridLineCount);
    m_repeaterX->setModel(axis->labels().size());
    m_controller->handleAxisLabelsChangedBySender(m_controller->axisX());

    axis = m_controller->axisY();
    if (axis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(axis);
        segmentCount = valueAxis->segmentCount();
        subSegmentCount = valueAxis->subSegmentCount();
        gridLineCount = 2 * (segmentCount + 1);
        subGridLineCount = 2 * (segmentCount * (subSegmentCount - 1));
    } else if (axis->type() & QAbstract3DAxis::AxisTypeCategory) {
        gridLineCount = axis->labels().size();
    }
    m_segmentLineRepeaterY->setModel(gridLineCount);
    m_subsegmentLineRepeaterY->setModel(subGridLineCount);
    m_repeaterY->setModel(2 * axis->labels().size());
    m_controller->handleAxisLabelsChangedBySender(m_controller->axisY());

    axis = m_controller->axisZ();
    if (axis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(axis);
        segmentCount = valueAxis->segmentCount();
        subSegmentCount = valueAxis->subSegmentCount();
        gridLineCount = 2 * (segmentCount + 1);
        subGridLineCount = 2 * (segmentCount * (subSegmentCount - 1));
    } else if (axis->type() & QAbstract3DAxis::AxisTypeCategory) {
        gridLineCount = axis->labels().size();
    }
    m_segmentLineRepeaterZ->setModel(gridLineCount);
    m_subsegmentLineRepeaterZ->setModel(subGridLineCount);
    m_repeaterZ->setModel(axis->labels().size());
    m_controller->handleAxisLabelsChangedBySender(m_controller->axisZ());
}

QQuick3DDirectionalLight *QQuickDataVisItem::light() const
{
    return m_light;
}

void QQuickDataVisItem::setTheme(Q3DTheme *theme)
{
    m_controller->setActiveTheme(theme, isComponentComplete());
}

Q3DTheme *QQuickDataVisItem::theme() const
{
    return m_controller->activeTheme();
}

void QQuickDataVisItem::clearSelection()
{
    m_controller->clearSelection();
}

bool QQuickDataVisItem::hasSeries(QAbstract3DSeries *series)
{
    return m_controller->hasSeries(series);
}

void QQuickDataVisItem::setSelectionMode(SelectionFlags mode)
{
    int intmode = int(mode);
    m_controller->setSelectionMode(QAbstract3DGraphNG::SelectionFlags(intmode));
}

QQuickDataVisItem::SelectionFlags QQuickDataVisItem::selectionMode() const
{
    int intmode = int(m_controller->selectionMode());
    return SelectionFlags(intmode);
}

void QQuickDataVisItem::setShadowQuality(ShadowQuality quality)
{
    m_controller->setShadowQuality(QAbstract3DGraphNG::ShadowQuality(quality));
}

QQuickDataVisItem::ShadowQuality QQuickDataVisItem::shadowQuality() const
{
    return ShadowQuality(m_controller->shadowQuality());
}

bool QQuickDataVisItem::shadowsSupported() const
{
    return m_controller->shadowsSupported();
}

int QQuickDataVisItem::addCustomItem(QCustom3DItem *item)
{
    return m_controller->addCustomItem(item);
}

void QQuickDataVisItem::removeCustomItems()
{
    m_controller->deleteCustomItems();
}

void QQuickDataVisItem::removeCustomItem(QCustom3DItem *item)
{
    m_controller->deleteCustomItem(item);
}

void QQuickDataVisItem::removeCustomItemAt(const QVector3D &position)
{
    m_controller->deleteCustomItem(position);
}

void QQuickDataVisItem::releaseCustomItem(QCustom3DItem *item)
{
    m_controller->releaseCustomItem(item);
}

int QQuickDataVisItem::selectedLabelIndex() const
{
    return m_controller->selectedLabelIndex();
}

QAbstract3DAxis *QQuickDataVisItem::selectedAxis() const
{
    return m_controller->selectedAxis();
}

int QQuickDataVisItem::selectedCustomItemIndex() const
{
    return m_controller->selectedCustomItemIndex();
}

QCustom3DItem *QQuickDataVisItem::selectedCustomItem() const
{
    return m_controller->selectedCustomItem();
}

QQmlListProperty<QCustom3DItem> QQuickDataVisItem::customItemList()
{
    return QQmlListProperty<QCustom3DItem>(this, this,
                                           &QQuickDataVisItem::appendCustomItemFunc,
                                           &QQuickDataVisItem::countCustomItemFunc,
                                           &QQuickDataVisItem::atCustomItemFunc,
                                           &QQuickDataVisItem::clearCustomItemFunc);
}

void QQuickDataVisItem::appendCustomItemFunc(QQmlListProperty<QCustom3DItem> *list,
                                               QCustom3DItem *item)
{
    QQuickDataVisItem *decl = reinterpret_cast<QQuickDataVisItem *>(list->data);
    decl->addCustomItem(item);
}

qsizetype QQuickDataVisItem::countCustomItemFunc(QQmlListProperty<QCustom3DItem> *list)
{
    Q_UNUSED(list);
    return 0;
//    return reinterpret_cast<QQuickDataVisItem *>(list->data)->m_controller->m_customItems.size();
}

QCustom3DItem *QQuickDataVisItem::atCustomItemFunc(QQmlListProperty<QCustom3DItem> *list,
                                                     qsizetype index)
{
    Q_UNUSED(list);
    Q_UNUSED(index);
    return new QCustom3DItem();
//    return reinterpret_cast<QQuickDataVisItem *>(list->data)->m_controller->m_customItems.at(index);
}

void QQuickDataVisItem::clearCustomItemFunc(QQmlListProperty<QCustom3DItem> *list)
{
    QQuickDataVisItem *decl = reinterpret_cast<QQuickDataVisItem *>(list->data);
    decl->removeCustomItems();
}

void QQuickDataVisItem::setSharedController(Abstract3DController *controller)
{
    Q_ASSERT(controller);
    m_controller = controller;
//    m_controller->m_qml = this;

    if (!m_controller->isOpenGLES())
        setMsaaSamples(4);

    // Reset default theme, as the default C++ theme is Q3DTheme, not DeclarativeTheme3D.
    DeclarativeTheme3D *defaultTheme = new DeclarativeTheme3D;
    defaultTheme->d_ptr->setDefaultTheme(true);
    defaultTheme->setType(Q3DTheme::ThemeQt);
    m_controller->setActiveTheme(defaultTheme);

    QObject::connect(m_controller.data(), &Abstract3DController::shadowQualityChanged, this,
                     &QQuickDataVisItem::handleShadowQualityChange);
    QObject::connect(m_controller.data(), &Abstract3DController::activeInputHandlerChanged, this,
                     &QQuickDataVisItem::inputHandlerChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::activeThemeChanged, this,
                     &QQuickDataVisItem::themeChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::selectionModeChanged, this,
                     &QQuickDataVisItem::handleSelectionModeChange);
    QObject::connect(m_controller.data(), &Abstract3DController::elementSelected, this,
                     &QQuickDataVisItem::handleSelectedElementChange);

    QObject::connect(m_controller.data(), &Abstract3DController::axisXChanged, this,
                     &QQuickDataVisItem::handleAxisXChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::axisYChanged, this,
                     &QQuickDataVisItem::handleAxisYChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::axisZChanged, this,
                     &QQuickDataVisItem::handleAxisZChanged);

    QObject::connect(m_controller.data(), &Abstract3DController::measureFpsChanged, this,
                     &QQuickDataVisItem::measureFpsChanged);
//    QObject::connect(m_controller.data(), &Abstract3DController::currentFpsChanged, this,
//                     &QQuickDataVisItem::currentFpsChanged);

    QObject::connect(m_controller.data(), &Abstract3DController::orthoProjectionChanged, this,
                     &QQuickDataVisItem::orthoProjectionChanged);

    QObject::connect(m_controller.data(), &Abstract3DController::aspectRatioChanged, this,
                     &QQuickDataVisItem::aspectRatioChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::optimizationHintsChanged, this,
                     &QQuickDataVisItem::handleOptimizationHintChange);
    QObject::connect(m_controller.data(), &Abstract3DController::polarChanged, this,
                     &QQuickDataVisItem::polarChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::radialLabelOffsetChanged, this,
                     &QQuickDataVisItem::radialLabelOffsetChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::horizontalAspectRatioChanged, this,
                     &QQuickDataVisItem::horizontalAspectRatioChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::reflectionChanged, this,
                     &QQuickDataVisItem::reflectionChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::reflectivityChanged, this,
                     &QQuickDataVisItem::reflectivityChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::localeChanged, this,
                     &QQuickDataVisItem::localeChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::queriedGraphPositionChanged, this,
                     &QQuickDataVisItem::queriedGraphPositionChanged);
    QObject::connect(m_controller.data(), &Abstract3DController::marginChanged, this,
                     &QQuickDataVisItem::marginChanged);
}

void QQuickDataVisItem::synchData()
{
    m_controller->m_renderPending = false;

    bool axisFormatterChanged = false;
    if (m_controller->m_changeTracker.axisXFormatterChanged) {
        m_controller->m_changeTracker.axisXFormatterChanged = false;
        QAbstract3DAxis *axisX = m_controller->axisX();
        if (axisX->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisX = static_cast<QValue3DAxis *>(axisX);
            valueAxisX->recalculate();
        }
        axisFormatterChanged = true;
    }

    if (m_controller->m_changeTracker.axisYFormatterChanged) {
        m_controller->m_changeTracker.axisYFormatterChanged = false;
        QAbstract3DAxis *axisY = m_controller->axisY();
        if (axisY->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisY = static_cast<QValue3DAxis *>(axisY);
            valueAxisY->recalculate();
        }
        axisFormatterChanged = true;
    }

    if (m_controller->m_changeTracker.axisZFormatterChanged) {
        m_controller->m_changeTracker.axisZFormatterChanged = false;
        QAbstract3DAxis *axisZ = m_controller->axisZ();
        if (axisZ->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisZ = static_cast<QValue3DAxis *>(axisZ);
            valueAxisZ->recalculate();
        }
        axisFormatterChanged = true;
    }

    if (m_controller->m_changeTracker.shadowQualityChanged) {
        updateShadowQuality(shadowQuality());
        m_controller->m_changeTracker.shadowQualityChanged = false;
    }

    if (m_controller->m_changeTracker.axisYRangeChanged) {
        updateAxisRange(m_controller->m_axisY->min(), m_controller->m_axisY->max());
        m_controller->m_changeTracker.axisYRangeChanged = false;
    }

    if (m_controller->m_changeTracker.axisYReversedChanged) {
        m_controller->m_changeTracker.axisYReversedChanged = false;
        if (m_controller->m_axisY->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisY = static_cast<QValue3DAxis *>(m_controller->m_axisY);
            updateAxisReversed(valueAxisY->reversed());
        }
    }

    QVector3D forward = camera()->forward();
    auto targetRotation = cameraTarget()->rotation();
    bool viewFlipped = false;
    if (m_yFlipped != (targetRotation.x() > 0)) {
        m_yFlipped = (targetRotation.x() > 0);
        viewFlipped = true;
    }
    if (m_xFlipped != (forward.x() > 0)) {
        m_xFlipped = (forward.x() > 0);
        viewFlipped = true;
    }
    if (m_zFlipped != (forward.z() >= 0)) {
        m_zFlipped = (forward.z() >= 0);
        viewFlipped = true;
    }

    if (axisFormatterChanged || viewFlipped) {
        updateGrid();
        updateLabels();
    }

    QMatrix4x4 modelMatrix;
    m_backgroundScale->setScale(m_scaleWithBackground + m_backgroundScaleMargin);

    QVector3D rotVec;
    if (!m_yFlipped) {
        rotVec = QVector3D(0, 270, 0);
        if (m_xFlipped && m_zFlipped)
            rotVec.setY(90);
        else if (!m_xFlipped && m_zFlipped)
            rotVec.setY(0);
        else if (m_xFlipped && !m_zFlipped)
            rotVec.setY(180);
    } else {
        rotVec = QVector3D(0, 180, 180);
        if (m_xFlipped && m_zFlipped)
            rotVec.setY(0);
        else if (!m_xFlipped && m_zFlipped)
            rotVec.setY(270);
        else if (m_xFlipped && !m_zFlipped)
            rotVec.setY(90);
    }

    auto rotation = Utils::calculateRotation(rotVec);
    if (m_yFlipped) {
        m_backgroundRotation->setRotation(rotation);
    } else {
        modelMatrix.rotate(rotation);
        m_backgroundRotation->setRotation(rotation);
    }

    if (m_controller->graphPositionQueryPending())
        graphPositionAt(m_controller->scene()->graphPositionQuery());

    updateCamera();

    Q3DTheme *theme = m_controller->activeTheme();

    if (m_controller->m_changeTracker.themeChanged) {
        environment()->setClearColor(theme->windowColor());

        m_controller->m_changeTracker.themeChanged = false;
    }

    if (theme->d_ptr->m_dirtyBits.lightStrengthDirty) {
        light()->setBrightness(theme->lightStrength() * .2f);
        theme->d_ptr->m_dirtyBits.lightStrengthDirty = false;
    }

    if (theme->d_ptr->m_dirtyBits.ambientLightStrengthDirty) {
        float ambientStrength = theme->ambientLightStrength();
        QColor ambientColor = QColor::fromRgbF(ambientStrength, ambientStrength, ambientStrength);
        light()->setAmbientColor(ambientColor);
        theme->d_ptr->m_dirtyBits.ambientLightStrengthDirty = false;
    }

    if (theme->d_ptr->m_dirtyBits.lightColorDirty) {
        light()->setColor(theme->lightColor());
        theme->d_ptr->m_dirtyBits.lightColorDirty = false;
    }

    // label Adjustments
    if (m_repeaterX->count() && m_repeaterZ->count()) {
        if (theme->d_ptr->m_dirtyBits.labelBackgroundColorDirty) {
            QColor labelBackgroundColor = theme->labelBackgroundColor();
            changeLabelBackgroundColor(m_repeaterX, labelBackgroundColor);
            changeLabelBackgroundColor(m_repeaterY, labelBackgroundColor);
            changeLabelBackgroundColor(m_repeaterZ, labelBackgroundColor);
            m_titleLabelX->setProperty("backgroundColor", labelBackgroundColor);
            m_titleLabelY->setProperty("backgroundColor", labelBackgroundColor);
            m_titleLabelZ->setProperty("backgroundColor", labelBackgroundColor);
            m_itemLabel->setProperty("backgroundColor", labelBackgroundColor);
            theme->d_ptr->m_dirtyBits.labelBackgroundColorDirty = false;
        }

        if (theme->d_ptr->m_dirtyBits.labelBackgroundEnabledDirty) {
            bool enabled = theme->isLabelBackgroundEnabled();
            changeLabelBackgroundEnabled(m_repeaterX, enabled);
            changeLabelBackgroundEnabled(m_repeaterY, enabled);
            changeLabelBackgroundEnabled(m_repeaterZ, enabled);
            m_titleLabelX->setProperty("backgroundEnabled", enabled);
            m_titleLabelY->setProperty("backgroundEnabled", enabled);
            m_titleLabelZ->setProperty("backgroundEnabled", enabled);
            m_itemLabel->setProperty("backgroundEnabled", enabled);
            theme->d_ptr->m_dirtyBits.labelBackgroundEnabledDirty = false;
        }

        if (theme->d_ptr->m_dirtyBits.labelBorderEnabledDirty) {
            bool enabled = theme->isLabelBorderEnabled();
            changeLabelBorderEnabled(m_repeaterX, enabled);
            changeLabelBorderEnabled(m_repeaterY, enabled);
            changeLabelBorderEnabled(m_repeaterZ, enabled);
            m_titleLabelX->setProperty("borderEnabled", enabled);
            m_titleLabelY->setProperty("borderEnabled", enabled);
            m_titleLabelZ->setProperty("borderEnabled", enabled);
            m_itemLabel->setProperty("borderEnabled", enabled);
            theme->d_ptr->m_dirtyBits.labelBorderEnabledDirty = false;
        }

        if (theme->d_ptr->m_dirtyBits.labelTextColorDirty) {
            QColor labelTextColor = theme->labelTextColor();
            changeLabelTextColor(m_repeaterX, labelTextColor);
            changeLabelTextColor(m_repeaterY, labelTextColor);
            changeLabelTextColor(m_repeaterZ, labelTextColor);
            m_titleLabelX->setProperty("labelTextColor", labelTextColor);
            m_titleLabelY->setProperty("labelTextColor", labelTextColor);
            m_titleLabelZ->setProperty("labelTextColor", labelTextColor);
            m_itemLabel->setProperty("labelTextColor", labelTextColor);
            theme->d_ptr->m_dirtyBits.labelTextColorDirty = false;
        }

        if (theme->d_ptr->m_dirtyBits.fontDirty) {
            auto font = theme->font();
            changeLabelFont(m_repeaterX, font);
            changeLabelFont(m_repeaterY, font);
            changeLabelFont(m_repeaterZ, font);
            m_titleLabelX->setProperty("labelFont", font);
            m_titleLabelY->setProperty("labelFont", font);
            m_titleLabelZ->setProperty("labelFont", font);
            m_itemLabel->setProperty("labelFont", font);
            updateLabels();
            theme->d_ptr->m_dirtyBits.fontDirty = false;
        }

        // Grid and background adjustments
        if (theme->d_ptr->m_dirtyBits.backgroundColorDirty) {
            QQmlListReference materialsRef(m_background, "materials");
            QQuick3DPrincipledMaterial *bgMat;
            if (!materialsRef.size()) {
                bgMat = new QQuick3DPrincipledMaterial();
                bgMat->setParent(this);
                bgMat->setRoughness(.3f);
                bgMat->setEmissiveFactor(QVector3D(.075f, .075f, .075f));
                materialsRef.append(bgMat);
            } else {
                bgMat = static_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0));
            }
            bgMat->setBaseColor(theme->backgroundColor());
            theme->d_ptr->m_dirtyBits.backgroundColorDirty = false;
        }

        if (theme->d_ptr->m_dirtyBits.backgroundEnabledDirty) {
            m_background->setVisible(theme->isBackgroundEnabled());
            theme->d_ptr->m_dirtyBits.backgroundEnabledDirty = false;
        }

        if (theme->d_ptr->m_dirtyBits.gridEnabledDirty) {
            bool enabled = theme->isGridEnabled();
            m_segmentLineRepeaterX->setVisible(enabled);
            m_segmentLineRepeaterY->setVisible(enabled);
            m_segmentLineRepeaterZ->setVisible(enabled);

            m_subsegmentLineRepeaterX->setVisible(enabled);
            m_subsegmentLineRepeaterY->setVisible(enabled);
            m_subsegmentLineRepeaterZ->setVisible(enabled);
            theme->d_ptr->m_dirtyBits.gridEnabledDirty = false;
        }

        if (theme->d_ptr->m_dirtyBits.gridLineColorDirty) {
            QColor gridLineColor = theme->gridLineColor();
            changeGridLineColor(m_segmentLineRepeaterX, gridLineColor);
            changeGridLineColor(m_subsegmentLineRepeaterX, gridLineColor);
            changeGridLineColor(m_segmentLineRepeaterY, gridLineColor);
            changeGridLineColor(m_subsegmentLineRepeaterY, gridLineColor);
            changeGridLineColor(m_segmentLineRepeaterZ, gridLineColor);
            changeGridLineColor(m_subsegmentLineRepeaterZ, gridLineColor);
            theme->d_ptr->m_dirtyBits.gridLineColorDirty = false;
        }

        if (theme->d_ptr->m_dirtyBits.singleHighlightColorDirty) {
            updateSingleHighlightColor();
            theme->d_ptr->m_dirtyBits.singleHighlightColorDirty = false;
        }
    }

    // Other adjustments
    if (theme->d_ptr->m_dirtyBits.windowColorDirty) {
        environment()->setClearColor(theme->windowColor());
        theme->d_ptr->m_dirtyBits.windowColorDirty = false;
    }

    if (m_controller->m_changedSeriesList.size()) {
        updateGraph();
        m_controller->m_changedSeriesList.clear();
    }

    if (m_controller->m_isSeriesVisualsDirty) {
        updateGraph();
        m_controller->m_isSeriesVisualsDirty = false;
    }

    if (m_controller->m_isDataDirty) {
        updateGraph();
        m_controller->m_isDataDirty = false;
    }

    if (m_sliceActivatedChanged) {
        updateSliceGraph();
        m_sliceActivatedChanged = false;
    }
}

void QQuickDataVisItem::updateGrid()
{
    int gridLineCountX = segmentLineRepeaterX()->count();
    int subGridLineCountX = subsegmentLineRepeaterX()->count();
    int gridLineCountY = segmentLineRepeaterY()->count() / 2;
    int subGridLineCountY = subsegmentLineRepeaterY()->count() / 2;
    int gridLineCountZ = segmentLineRepeaterZ()->count();
    int subGridLineCountZ = subsegmentLineRepeaterZ()->count();

    if (!m_isFloorGridInRange) {
        gridLineCountX /= 2;
        subGridLineCountX /= 2;
        gridLineCountZ /= 2;
        subGridLineCountZ /= 2;
    }

    auto backgroundScale = m_scaleWithBackground + m_backgroundScaleMargin;
    QVector3D scaleX(backgroundScale.x() * lineLengthScaleFactor(), lineWidthScaleFactor(), lineWidthScaleFactor());
    QVector3D scaleY(lineWidthScaleFactor(), backgroundScale.y() * lineLengthScaleFactor(), lineWidthScaleFactor());
    QVector3D scaleZ(backgroundScale.z() * lineLengthScaleFactor(), lineWidthScaleFactor(), lineWidthScaleFactor());

    auto axisX = m_controller->axisX();
    auto axisY = m_controller->axisY();
    auto axisZ = m_controller->axisZ();

    const bool xFlipped = isXFlipped();
    const bool yFlipped = isYFlipped();
    const bool zFlipped = isZFlipped();

    QQuaternion lineRotation(.0f, .0f, .0f, .0f);
    QVector3D rotation(90.0f, 0.0f, 0.0f);

    // Floor horizontal line
    float linePosX = 0.0f;
    float linePosY = backgroundScale.y();
    float linePosZ = 0.0f;
    float scale = m_scaleWithBackground.z();
    float translate = m_scaleWithBackground.z();

    if (!yFlipped) {
        linePosY *= -1.0f;
        rotation.setZ(180.0f);
    }
    lineRotation = Utils::calculateRotation(rotation);
    for (int i = 0; i < subGridLineCountZ; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterZ()->objectAt(i));
        if (axisZ->type() == QAbstract3DAxis::AxisTypeValue) {
            linePosZ = static_cast<QValue3DAxis *>(axisZ)->subGridPositionAt(i) * -scale * 2.0f + translate;
        } else if (axisZ->type() == QAbstract3DAxis::AxisTypeCategory) {
            linePosZ = calculateCategoryGridLinePosition(axisZ, i);
            linePosY = calculateCategoryGridLinePosition(axisY, i);
        }
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineRotation);
    }

    for (int i  = 0; i < gridLineCountZ; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterZ()->objectAt(i));
        if (axisZ->type() == QAbstract3DAxis::AxisTypeValue) {
            linePosZ = static_cast<QValue3DAxis *>(axisZ)->gridPositionAt(i) * -scale * 2.0f + translate;
        } else if (axisZ->type() == QAbstract3DAxis::AxisTypeCategory) {
            linePosZ = calculateCategoryGridLinePosition(axisZ, i);
            linePosY = calculateCategoryGridLinePosition(axisY, i);
        }
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineRotation);
    }

    // Side vertical line
    linePosX = -backgroundScale.x();
    linePosY = 0.0f;
    rotation = QVector3D(0.0f, 90.0f, 0.0f);
    if (xFlipped) {
        linePosX *= -1.0f;
        rotation.setY(-90.0f);
    }
    lineRotation = Utils::calculateRotation(rotation);
    if (m_hasVerticalSegmentLine) {
        for (int i  = 0; i < subGridLineCountZ; i++) {
            QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterZ()->objectAt(i + subGridLineCountZ));
            if (axisZ->type() == QAbstract3DAxis::AxisTypeValue) {
                linePosZ = static_cast<QValue3DAxis *>(axisZ)->subGridPositionAt(i) * scale * 2.0f - translate;
            } else if (axisZ->type() == QAbstract3DAxis::AxisTypeCategory) {
                linePosX = calculateCategoryGridLinePosition(axisZ, i);
                linePosY = calculateCategoryGridLinePosition(axisY, i);
            }
            positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
            lineNode->setRotation(lineRotation);
        }
        for (int i  = 0; i < gridLineCountZ; i++) {
            QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterZ()->objectAt(i + gridLineCountZ));
            if (axisZ->type() == QAbstract3DAxis::AxisTypeValue) {
                linePosZ = static_cast<QValue3DAxis *>(axisZ)->gridPositionAt(i) * scale * 2.0f - translate;
            } else if (axisZ->type() == QAbstract3DAxis::AxisTypeCategory) {
                linePosX = calculateCategoryGridLinePosition(axisZ, i);
                linePosY = calculateCategoryGridLinePosition(axisY, i);
            }
            positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
            lineNode->setRotation(lineRotation);
        }
    }

    // Side horizontal line
    linePosZ = 0.0f;
    scale = translate = m_scaleWithBackground.y();
    rotation = QVector3D(180.0f, -90.0f, 0.0f);
    if (xFlipped)
        rotation.setY(90.0f);
    lineRotation = Utils::calculateRotation(rotation);
    for (int i  = 0; i < gridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterY()->objectAt(i));
        if (axisY->type() == QAbstract3DAxis::AxisTypeValue)
            linePosY = static_cast<QValue3DAxis *>(axisY)->gridPositionAt(i) * scale * 2.0f - translate;
        else if (axisY->type() == QAbstract3DAxis::AxisTypeCategory)
            linePosY = calculateCategoryGridLinePosition(axisY, i);
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineRotation);
    }

    for (int i = 0; i < subGridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterY()->objectAt(i));
        if (axisY->type() == QAbstract3DAxis::AxisTypeValue)
            linePosY = static_cast<QValue3DAxis *>(axisY)->subGridPositionAt(i) * scale * 2.0f - translate;
        else if (axisY->type() == QAbstract3DAxis::AxisTypeCategory)
            linePosY = calculateCategoryGridLinePosition(axisY, i);
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineRotation);
    }

    // Floor vertical line
    linePosZ = 0.0f;
    linePosY = -backgroundScale.y();
    rotation = QVector3D(-90.0f, 90.0f, 0.0f);
    if (yFlipped) {
        linePosY *= -1.0f;
        rotation.setZ(180.0f);
    }
    scale = translate = m_scaleWithBackground.x();
    lineRotation = Utils::calculateRotation(rotation);
    for (int i = 0; i < subGridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterX()->objectAt(i));
        if (axisX->type() == QAbstract3DAxis::AxisTypeValue) {
            linePosX = static_cast<QValue3DAxis *>(axisX)->subGridPositionAt(i) * scale * 2.0f - translate;
        } else if (axisX->type() == QAbstract3DAxis::AxisTypeCategory) {
            linePosX = calculateCategoryGridLinePosition(axisX, i);
            linePosY = calculateCategoryGridLinePosition(axisY, i);
        }
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineRotation);
    }
    for (int i  = 0; i < gridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterX()->objectAt(i));
        if (axisX->type() == QAbstract3DAxis::AxisTypeValue) {
            linePosX = static_cast<QValue3DAxis *>(axisX)->gridPositionAt(i) * scale * 2.0f - translate;
        } else if (axisX->type() == QAbstract3DAxis::AxisTypeCategory) {
            linePosX = calculateCategoryGridLinePosition(axisX, i);
            linePosY = calculateCategoryGridLinePosition(axisY, i);
        }
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineRotation);
    }

    // Back horizontal line
    linePosX = 0.0f;
    linePosZ = -backgroundScale.z();
    rotation = QVector3D(0.0f, 0.0f, 0.0f);
    if (zFlipped) {
        linePosZ *= -1.0f;
        rotation.setX(180.0f);
    }
    lineRotation = Utils::calculateRotation(rotation);
    scale = translate = m_scaleWithBackground.y();
    for (int i = 0; i < subGridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterY()->objectAt(i + subGridLineCountY));
        if (axisY->type() == QAbstract3DAxis::AxisTypeValue)
            linePosY = static_cast<QValue3DAxis *>(axisY)->subGridPositionAt(i) * scale * 2.0f - translate;
        else if (axisY->type() == QAbstract3DAxis::AxisTypeCategory)
            linePosY = calculateCategoryGridLinePosition(axisY, i);
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineRotation);
    }

    for (int i  = 0; i < gridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterY()->objectAt(i + gridLineCountY));
        if (axisY->type() == QAbstract3DAxis::AxisTypeValue)
            linePosY = static_cast<QValue3DAxis *>(axisY)->gridPositionAt(i) * scale * 2.0f - translate;
        else if (axisY->type() == QAbstract3DAxis::AxisTypeCategory)
            linePosY = calculateCategoryGridLinePosition(axisY, i);
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineRotation);
    }

    // Back vertical line
    linePosY = 0.0f;
    scale = translate = m_scaleWithBackground.x();
    rotation = QVector3D(0.0f, 0.0f, 0.0f);
    if (zFlipped)
        rotation.setY(180.0f);
    lineRotation = Utils::calculateRotation(rotation);
    if (m_hasVerticalSegmentLine) {
        for (int i  = 0; i < gridLineCountX; i++) {
            QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterX()->objectAt(i + gridLineCountX));
            if (axisX->type() == QAbstract3DAxis::AxisTypeValue) {
                linePosX = static_cast<QValue3DAxis *>(axisX)->gridPositionAt(i) * scale * 2.0f - translate;
            } else if (axisX->type() == QAbstract3DAxis::AxisTypeCategory) {
                linePosX = calculateCategoryGridLinePosition(axisX, i);
                linePosY = calculateCategoryGridLinePosition(axisY, i);
            }
            positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
            lineNode->setRotation(lineRotation);
        }

        for (int i  = 0; i < subGridLineCountX; i++) {
            QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterX()->objectAt(i + subGridLineCountX));
            if (axisX->type() == QAbstract3DAxis::AxisTypeValue) {
                linePosX = static_cast<QValue3DAxis *>(axisX)->subGridPositionAt(i) * scale * 2.0f - translate;
            } else if (axisX->type() == QAbstract3DAxis::AxisTypeCategory) {
                linePosX = calculateCategoryGridLinePosition(axisX, i);
                linePosY = calculateCategoryGridLinePosition(axisY, i);
            }
            positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
            lineNode->setRotation(lineRotation);
        }
    }
}

void QQuickDataVisItem::updateLabels()
{
    auto axisX = m_controller->axisX();

    auto labels = axisX->labels();
    float labelAutoAngle = axisX->labelAutoRotation();
    float labelAngleFraction = labelAutoAngle / 90.0f;
    float fractionCamX = m_controller->scene()->activeCamera()->xRotation() * labelAngleFraction;
    float fractionCamY = m_controller->scene()->activeCamera()->yRotation() * labelAngleFraction;

    QVector3D labelRotation = QVector3D(0.0f, 0.0f, 0.0f);

    float xPos = 0.0f;
    float yPos = 0.0f;
    float zPos = 0.0f;

    const bool xFlipped = isXFlipped();
    const bool yFlipped = isYFlipped();
    const bool zFlipped = isZFlipped();

    auto backgroundScale = m_scaleWithBackground + m_backgroundScaleMargin;

    if (labelAutoAngle == 0.0f) {
        labelRotation = QVector3D(-90.0f, 90.0f, 0.0f);
        if (xFlipped)
            labelRotation.setY(-90.0f);
        if (yFlipped) {
            if (xFlipped)
                labelRotation.setY(-90.0f);
            else
                labelRotation.setY(90.0f);
            labelRotation.setX(90.0f);
        }
    } else {
        if (xFlipped)
            labelRotation.setY(-90.0f);
        else
            labelRotation.setY(90.0f);
        if (yFlipped) {
            if (zFlipped) {
                if (xFlipped) {
                    labelRotation.setX(90.0f - (2.0f * labelAutoAngle - fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(90.0f - (2.0f * labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                }
            } else {
                if (xFlipped) {
                    labelRotation.setX(90.0f + fractionCamX
                                       * -(labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(90.0f - fractionCamX
                                       * (-labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                }
            }
        } else {
            if (zFlipped) {
                if (xFlipped) {
                    labelRotation.setX(-90.0f + (2.0f * labelAutoAngle - fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(-90.0f + (2.0f * labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                }
            } else {
                if (xFlipped) {
                    labelRotation.setX(-90.0f - fractionCamX
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(-90.0f + fractionCamX
                                       * -(labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                }
            }
        }
    }
    auto totalRotation = Utils::calculateRotation(labelRotation);

    float scale = backgroundScale.x() - m_backgroundScaleMargin.x();;
    float translate = backgroundScale.x() - m_backgroundScaleMargin.x();
    float textPadding = 12.0f;

    auto pointSize = m_controller->activeTheme()->font().pointSizeF();
    auto scaleFactor = m_labelScale.x() * m_labelFontScaleFactor / pointSize
            + m_labelScale.x() * m_fontScaleFactor;
    QVector3D fontScaled = QVector3D(scaleFactor, scaleFactor, 0.0f);

    m_itemLabel->setScale(fontScaled);

    float labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisX->labels()))) + textPadding;
    QFontMetrics fm(m_controller->activeTheme()->font());
    float labelHeight = fm.height() + textPadding;

    auto adjustment = labelsMaxWidth * scaleFactor * .5f;
    zPos = backgroundScale.z() + adjustment + m_labelMargin;
    adjustment *= qAbs(qSin(qDegreesToRadians(labelRotation.z())));
    yPos = backgroundScale.y() + adjustment;

    if (!yFlipped)
        yPos *= -1.0f;

    if (zFlipped)
        zPos *= -1.0f;

    auto labelTrans = QVector3D(0.0f, yPos, zPos);

    if (axisX->type() == QAbstract3DAxis::AxisTypeValue) {
        auto valueAxisX = static_cast<QValue3DAxis *>(axisX);
        for (int i = 0; i < repeaterX()->count(); i++) {
            auto obj = static_cast<QQuick3DNode *>(repeaterX()->objectAt(i));
            labelTrans.setX(valueAxisX->labelPositionAt(i) * scale * 2.0f - translate);
            obj->setScale(fontScaled);
            obj->setPosition(labelTrans);
            obj->setRotation(totalRotation);
            obj->setProperty("labelText", labels[i]);
            obj->setProperty("labelWidth", labelsMaxWidth);
            obj->setProperty("labelHeight", labelHeight);
        }
    } else if (axisX->type() == QAbstract3DAxis::AxisTypeCategory) {
        for (int i = 0; i < repeaterX()->count(); i++) {
            labelTrans = calculateCategoryLabelPosition(axisX, labelTrans, i);
            auto obj = static_cast<QQuick3DNode *>(repeaterX()->objectAt(i));
            obj->setScale(fontScaled);
            obj->setPosition(labelTrans);
            obj->setRotation(totalRotation);
            obj->setProperty("labelText", labels[i]);
            obj->setProperty("labelWidth", labelsMaxWidth);
            obj->setProperty("labelHeight", labelHeight);
        }
    }

    if (titleLabelX()->visible()) {
        float x = labelTrans.x();
        labelTrans.setX(0.0f);
        updateXTitle(labelRotation, labelTrans, totalRotation, labelsMaxWidth, labelHeight, fontScaled);
        labelTrans.setX(x);
    }

    auto axisY = m_controller->axisY();
    labels = axisY->labels();
    labelAutoAngle = axisY->labelAutoRotation();
    labelAngleFraction = labelAutoAngle / 90.0f;
    fractionCamX = m_controller->scene()->activeCamera()->xRotation() * labelAngleFraction;
    fractionCamY = m_controller->scene()->activeCamera()->yRotation() * labelAngleFraction;

    QVector3D sideLabelRotation(0.0f, -90.0f, 0.0f);
    QVector3D backLabelRotation(0.0f, 0.0f, 0.0f);

    if (labelAutoAngle == 0.0f) {
        if (!xFlipped)
            sideLabelRotation.setY(90.0f);
        if (zFlipped)
            backLabelRotation.setY(180.f);
    } else {
        // Orient side labels somewhat towards the camera
        if (xFlipped) {
            if (zFlipped)
                backLabelRotation.setY(180.0f + (2.0f * labelAutoAngle) - fractionCamX);
            else
                backLabelRotation.setY(-fractionCamX);
            sideLabelRotation.setY(-90.0f + labelAutoAngle - fractionCamX);
        } else {
            if (zFlipped)
                backLabelRotation.setY(180.0f - (2.0f * labelAutoAngle) - fractionCamX);
            else
                backLabelRotation.setY(-fractionCamX);
            sideLabelRotation.setY(90.0f - labelAutoAngle - fractionCamX);
        }
    }

    backLabelRotation.setX(-fractionCamY);
    sideLabelRotation.setX(-fractionCamY);

    totalRotation = Utils::calculateRotation(sideLabelRotation);
    scale = translate = backgroundScale.y() - m_backgroundScaleMargin.y();

    labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisY->labels()))) + textPadding;

    adjustment = labelsMaxWidth * scaleFactor * .5f + m_labelMargin;
    xPos = backgroundScale.x();
    if (!xFlipped)
        xPos *= -1.0f;
    labelTrans.setX(xPos);
    zPos = backgroundScale.z() + adjustment;
    if (zFlipped)
        zPos *= -1.0f;
    labelTrans.setZ(zPos);

    for (int i = 0; i < repeaterY()->count() / 2; i++) {
        auto obj = static_cast<QQuick3DNode *>(repeaterY()->objectAt(i));
        labelTrans.setY(static_cast<QValue3DAxis *>(axisY)->labelPositionAt(i) * scale * 2.0f - translate);
        obj->setScale(fontScaled);
        obj->setPosition(labelTrans);
        obj->setRotation(totalRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
        obj->setProperty("labelHeight", labelHeight);
    }

    auto sideLabelTrans = labelTrans;
    auto totalSideLabelRotation = totalRotation;

    auto axisZ = m_controller->axisZ();
    labels = axisZ->labels();
    labelAutoAngle = axisZ->labelAutoRotation();
    labelAngleFraction = labelAutoAngle / 90.0f;
    fractionCamX = m_controller->scene()->activeCamera()->xRotation() * labelAngleFraction;
    fractionCamY = m_controller->scene()->activeCamera()->yRotation() * labelAngleFraction;

    if (labelAutoAngle == 0.0f) {
        labelRotation = QVector3D(90.0f, 0.0f, 0.0f);
        if (zFlipped)
            labelRotation.setY(180.0f);
        if (yFlipped) {
            if (zFlipped)
                labelRotation.setY(180.0f);
            else
                labelRotation.setY(0.0f);
            labelRotation.setX(90.0f);
        } else {
            labelRotation.setX(-90.0f);
        }
    } else {
        if (zFlipped)
            labelRotation.setY(180.0f);
        else
            labelRotation.setY(0.0f);
        if (yFlipped) {
            if (zFlipped) {
                if (xFlipped) {
                    labelRotation.setX(90.0f - (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(90.0f + (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                }
            } else {
                if (xFlipped) {
                    labelRotation.setX(90.0f + (labelAutoAngle - fractionCamX)
                                       * -(labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(90.0f - (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                }
            }
        } else {
            if (zFlipped) {
                if (xFlipped) {
                    labelRotation.setX(-90.0f + (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(-90.0f - (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                }
            } else {
                if (xFlipped) {
                    labelRotation.setX(-90.0f - (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(-90.0f + (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                }
            }
        }
    }

    totalRotation = Utils::calculateRotation(labelRotation);

    scale = translate = backgroundScale.z() - m_backgroundScaleMargin.z();
    labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisZ->labels()))) + textPadding ;
    adjustment = labelsMaxWidth * scaleFactor * .5f;

    xPos = backgroundScale.x() + adjustment + m_labelMargin;
    if (xFlipped)
        xPos *= -1.0f;

    adjustment *= qAbs(qSin(qDegreesToRadians(labelRotation.z())));
    yPos = backgroundScale.y() + adjustment;
    if (!yFlipped)
        yPos *= -1.0f;

    labelTrans = QVector3D(xPos, yPos, 0.0f);

    if (axisZ->type() == QAbstract3DAxis::AxisTypeValue) {
        auto valueAxisZ = static_cast<QValue3DAxis *>(axisZ);
        for (int i = 0; i < repeaterZ()->count(); i++) {
            auto obj = static_cast<QQuick3DNode *>(repeaterZ()->objectAt(i));
            labelTrans.setZ(valueAxisZ->labelPositionAt(i) * scale * -2.0f + translate);
            obj->setScale(fontScaled);
            obj->setPosition(labelTrans);
            obj->setRotation(totalRotation);
            obj->setProperty("labelText", labels[i]);
            obj->setProperty("labelWidth", labelsMaxWidth);
            obj->setProperty("labelHeight", labelHeight);
        }
    } else if (axisZ->type() == QAbstract3DAxis::AxisTypeCategory) {
        for (int i = 0; i < repeaterZ()->count(); i++) {
            labelTrans = calculateCategoryLabelPosition(axisZ, labelTrans, i);
            auto obj = static_cast<QQuick3DNode *>(repeaterZ()->objectAt(i));
            obj->setScale(fontScaled);
            obj->setPosition(labelTrans);
            obj->setRotation(totalRotation);
            obj->setProperty("labelText", labels[i]);
            obj->setProperty("labelWidth", labelsMaxWidth);
            obj->setProperty("labelHeight", labelHeight);
        }
    }

    if (titleLabelZ()->visible()) {
        float z = labelTrans.z();
        labelTrans.setZ(0.0f);
        updateZTitle(labelRotation, labelTrans, totalRotation, labelsMaxWidth, labelHeight, fontScaled);
        labelTrans.setZ(z);
    }

    labels = axisY->labels();
    totalRotation = Utils::calculateRotation(backLabelRotation);
    scale = translate = backgroundScale.y() - m_backgroundScaleMargin.y();
    labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisY->labels()))) + textPadding;
    adjustment = labelsMaxWidth * scaleFactor * .5f + m_labelMargin;

    xPos = backgroundScale.x() + adjustment;
    if (xFlipped)
        xPos *= -1.0f;
    labelTrans.setX(xPos);

    zPos = -backgroundScale.z();
    if (zFlipped)
        zPos *= -1.0f;
    labelTrans.setZ(zPos);

    for (int i = 0; i < repeaterY()->count() / 2; i++) {
        auto obj = static_cast<QQuick3DNode *>(repeaterY()->objectAt(i + (repeaterY()->count() / 2)));
        labelTrans.setY(static_cast<QValue3DAxis *>(axisY)->labelPositionAt(i) * scale * 2.0f - translate);
        obj->setScale(fontScaled);
        obj->setPosition(labelTrans);
        obj->setRotation(totalRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
        obj->setProperty("labelHeight", labelHeight);
    }

    auto backLabelTrans = labelTrans;
    auto totalBackLabelRotation = totalRotation;
    if (titleLabelY()->visible()) {
        updateYTitle(sideLabelRotation, backLabelRotation,
                     sideLabelTrans,    backLabelTrans,
                     totalSideLabelRotation, totalBackLabelRotation, labelsMaxWidth, labelHeight, fontScaled);
    }

}

QQuick3DNode *QQuickDataVisItem::itemSelectionLabel() const
{
    return m_itemSelectionLabel;
}

void QQuickDataVisItem::setItemSelectionLabel(QQuick3DNode *newItemSelectionLabel)
{
    m_itemSelectionLabel = newItemSelectionLabel;
}

void QQuickDataVisItem::positionAndScaleLine(QQuick3DNode *lineNode, QVector3D scale, QVector3D position)
{
    lineNode->setScale(scale);
    lineNode->setPosition(position);
}

void QQuickDataVisItem::graphPositionAt(const QPoint &point)
{
    bool isHitted = false;
    auto results = pickAll(point.x(), point.y());
    for (auto &result : results) {
        if (auto hit = result.objectHit()) {
            isHitted = true;
            m_controller->setQueriedGraphPosition(QVector3D(
                        result.scenePosition().x(),
                        result.scenePosition().y(),
                        result.scenePosition().z()
                        ));
            if (m_backgroundBB != hit) {
                m_controller->setQueriedGraphPosition(hit->position());
                break;
            }
        }
    }

    if (!isHitted)
        m_controller->setQueriedGraphPosition(QVector3D(0,0,0));

    emit queriedGraphPositionChanged(m_controller->queriedGraphPosition());
    emit m_controller->queriedGraphPositionChanged(m_controller->queriedGraphPosition());
    m_controller->setGraphPositionQueryPending(false);
    scene()->setGraphPositionQuery(Q3DScene::invalidSelectionPoint());
}

void QQuickDataVisItem::updateShadowQuality(ShadowQuality quality)
{
    if (quality != QQuickDataVisItem::ShadowQualityNone) {
        light()->setCastsShadow(true);

        QQuick3DAbstractLight::QSSGShadowMapQuality shadowMapQuality;
        switch (quality) {
        case QQuickDataVisItem::ShadowQualityLow:
        case QQuickDataVisItem::ShadowQualitySoftLow:
            shadowMapQuality = QQuick3DAbstractLight::QSSGShadowMapQuality::ShadowMapQualityLow;
            break;
        case QQuickDataVisItem::ShadowQualityMedium:
        case QQuickDataVisItem::ShadowQualitySoftMedium:
            shadowMapQuality = QQuick3DAbstractLight::QSSGShadowMapQuality::ShadowMapQualityMedium;
            break;
        case QQuickDataVisItem::ShadowQualityHigh:
        case QQuickDataVisItem::ShadowQualitySoftHigh:
            shadowMapQuality = QQuick3DAbstractLight::QSSGShadowMapQuality::ShadowMapQualityHigh;
            break;
        default:
            shadowMapQuality = QQuick3DAbstractLight::QSSGShadowMapQuality::ShadowMapQualityMedium;
            break;
        }
        light()->setShadowMapQuality(shadowMapQuality);
    } else {
        light()->setCastsShadow(false);
    }
}

void QQuickDataVisItem::updateAxisRange(float min, float max)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
}

void QQuickDataVisItem::updateAxisReversed(bool enable)
{
    Q_UNUSED(enable);
}

int QQuickDataVisItem::findLabelsMaxWidth(const QStringList &labels)
{
    int labelWidth = 0;
    QFontMetrics labelFM(m_controller->activeTheme()->font());

    for (const auto &label : std::as_const(labels)) {
        auto width = labelFM.horizontalAdvance(label);
        if (labelWidth < width)
            labelWidth = width;
    }
    return labelWidth;
}

QVector3D QQuickDataVisItem::calculateCategoryLabelPosition(QAbstract3DAxis *axis, QVector3D labelPosition, int index)
{
    Q_UNUSED(axis);
    Q_UNUSED(index);
    return labelPosition;
}

float QQuickDataVisItem::calculateCategoryGridLinePosition(QAbstract3DAxis *axis, int index)
{
    Q_UNUSED(axis);
    Q_UNUSED(index);
    return 0.0f;
}

void QQuickDataVisItem::updateXTitle(const QVector3D &labelRotation, const QVector3D &labelTrans,
                                     const QQuaternion &totalRotation, float labelsMaxWidth,
                                     float labelHeight, const QVector3D &scale)
{
    float scaledFontSize = (0.05 + m_controller->activeTheme()->font().pointSizeF()) / 500.0f;
    float scaleFactor = scaledFontSize / 115.0f;
    float titleOffset;

    bool radial = false;
    if (radial)
        titleOffset = -2.0f * (m_labelMargin + scaledFontSize);
    else
        titleOffset = 2.0f * (m_labelMargin + (labelsMaxWidth * scaleFactor));

    float zRotation = 0.0f;
    float yRotation = 0.0f;
    float xRotation = -90.0f + labelRotation.z();
    float offsetRotation = labelRotation.z();
    float extraRotation = -90.0f;
    if (m_yFlipped) {
        zRotation = 180.0f;
        if (m_zFlipped) {
            titleOffset = -titleOffset;
            if (m_xFlipped) {
                offsetRotation = -offsetRotation;
                extraRotation = -extraRotation;
            } else {
                xRotation = -90.0f - labelRotation.z();
            }
        } else {
            yRotation = 180.0f;
            if (m_xFlipped) {
                offsetRotation = -offsetRotation;
                xRotation = -90.0f - labelRotation.z();
            } else {
                extraRotation = -extraRotation;
            }
        }
    } else {
        if (m_zFlipped) {
            titleOffset = -titleOffset;
            if (m_xFlipped) {
                yRotation = 180.0f;
                offsetRotation = -offsetRotation;
            } else {
                yRotation = 180.0f;
                xRotation = -90.0f - labelRotation.z();
                extraRotation = -extraRotation;
            }
            if (m_yFlipped) {
                extraRotation = -extraRotation;
                if (m_xFlipped)
                    xRotation = 90.0f + labelRotation.z();
                else
                    xRotation = 90.0f - labelRotation.z();
            }
        } else {
            if (m_xFlipped) {
                offsetRotation = -offsetRotation;
                xRotation = -90.0f - labelRotation.z();
                extraRotation = -extraRotation;
            }
            if (m_yFlipped) {
                xRotation = 90.0f + labelRotation.z();
                extraRotation = -extraRotation;
                if (m_xFlipped)
                    xRotation = 90.0f - labelRotation.z();
            }
        }
    }

    if (offsetRotation == 180.0f || offsetRotation == -180.0f)
        offsetRotation = 0.0f;

    QQuaternion offsetRotator = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, offsetRotation);
    QVector3D titleOffsetVector =
            offsetRotator.rotatedVector(QVector3D(0.0f, 0.0f, titleOffset));

    QQuaternion titleRotation;
    if (m_controller->axisX()->isTitleFixed()) {
        titleRotation = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, zRotation)
                * QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yRotation)
                * QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, xRotation);
    } else {
        titleRotation = totalRotation
                * QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, extraRotation);
    }

    m_titleLabelX->setScale(scale);
    m_titleLabelX->setPosition(labelTrans + titleOffsetVector);
    m_titleLabelX->setRotation(titleRotation);
    m_titleLabelX->setProperty("labelWidth", labelsMaxWidth);
    m_titleLabelX->setProperty("LabelHeight", labelHeight);
}

void QQuickDataVisItem::updateYTitle(const QVector3D &sideLabelRotation,
                                     const QVector3D &backLabelRotation,
                                     const QVector3D &sideLabelTrans,
                                     const QVector3D &backLabelTrans,
                                     const QQuaternion &totalSideRotation,
                                     const QQuaternion &totalBackRotation,
                                     float labelsMaxWidth,
                                     float labelHeight,
                                     const QVector3D &scale)
{
    float scaledFontSize = (0.05 + m_controller->activeTheme()->font().pointSizeF()) / 500.0f;
    float scaleFactor = scaledFontSize / 115.0f;
    float titleOffset = 2.0f * (m_labelMargin + (labelsMaxWidth * scaleFactor));

    QQuaternion zRightAngleRotation = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, 90.0f);
    float yRotation;
    QVector3D titleTrans;
    QQuaternion totalRotation;
    if (m_xFlipped == m_zFlipped) {
        yRotation = backLabelRotation.y();
        titleTrans = backLabelTrans;
        totalRotation = totalBackRotation;
    } else {
        yRotation = sideLabelRotation.y();
        titleTrans = sideLabelTrans;
        totalRotation = totalSideRotation;
    }

    QQuaternion offsetRotator = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yRotation);
    QVector3D titleOffsetVector =
            offsetRotator.rotatedVector(QVector3D(-titleOffset, 0.0f, 0.0f));

    QQuaternion titleRotation;
    if (m_controller->axisY()->isTitleFixed()) {
        titleRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yRotation)
                * zRightAngleRotation;
    } else {
        titleRotation = totalRotation * zRightAngleRotation;
    }

    m_titleLabelY->setScale(scale);
    m_titleLabelY->setPosition(titleTrans + titleOffsetVector);
    m_titleLabelY->setRotation(titleRotation);
    m_titleLabelY->setProperty("labelWidth", labelsMaxWidth);
    m_titleLabelY->setProperty("LabelHeight", labelHeight);
}

void QQuickDataVisItem::updateZTitle(const QVector3D &labelRotation, const QVector3D &labelTrans,
                                     const QQuaternion &totalRotation, float labelsMaxWidth,
                                     float labelHeight, const QVector3D &scale)
{
    float scaledFontSize = (0.05 + m_controller->activeTheme()->font().pointSizeF()) / 500.0f;
    float scaleFactor = scaledFontSize / 115.0f;
    float titleOffset = 2.0f * (m_labelMargin + (labelsMaxWidth * scaleFactor));
    float zRotation = labelRotation.z();
    float yRotation = -90.0f;
    float xRotation = -90.0f;
    float extraRotation = 90.0f;

    if (m_yFlipped) {

        xRotation = -xRotation;
        if (m_zFlipped) {
            if (m_xFlipped) {
                titleOffset = -titleOffset;
                zRotation = -zRotation;
                extraRotation = -extraRotation;
            } else {
                zRotation = -zRotation;
                yRotation = -yRotation;
            }
        } else {
            if (m_xFlipped) {
                titleOffset = -titleOffset;
            } else {
                extraRotation = -extraRotation;
                yRotation = -yRotation;
            }
        }
    } else {
        if (m_zFlipped) {
            zRotation = -zRotation;
            if (m_xFlipped) {
                titleOffset = -titleOffset;
            } else {
                extraRotation = -extraRotation;
                yRotation = -yRotation;
            }
        } else {
            if (m_xFlipped) {
                titleOffset = -titleOffset;
                extraRotation = -extraRotation;
            } else {
                yRotation = -yRotation;
            }
        }
        if (m_yFlipped) {
            xRotation = -xRotation;
            extraRotation = -extraRotation;
        }
    }

    float offsetRotation = zRotation;
    if (offsetRotation == 180.0f || offsetRotation == -180.0f)
        offsetRotation = 0.0f;

    QQuaternion offsetRotator = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, offsetRotation);
    QVector3D titleOffsetVector =
            offsetRotator.rotatedVector(QVector3D(titleOffset, 0.0f, 0.0f));

    QQuaternion titleRotation;
    if (m_controller->axisZ()->isTitleFixed()) {
        titleRotation = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, zRotation)
                * QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, yRotation)
                * QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, xRotation);
    } else {
        titleRotation = totalRotation
                * QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, extraRotation);
    }

    m_titleLabelZ->setScale(scale);
    m_titleLabelZ->setPosition(labelTrans + titleOffsetVector);
    m_titleLabelZ->setRotation(titleRotation);
    m_titleLabelZ->setProperty("labelWidth", labelsMaxWidth);
    m_titleLabelZ->setProperty("LabelHeight", labelHeight);
}

void QQuickDataVisItem::updateCamera()
{
    float zoom = 720.f / m_controller->scene()->activeCamera()->zoomLevel();
    camera()->setZ(zoom);
    cameraTarget()->setPosition(m_controller->scene()->activeCamera()->target());
    auto rotation = QVector3D(
                -m_controller->scene()->activeCamera()->yRotation(),
                -m_controller->scene()->activeCamera()->xRotation(),
                0);
    cameraTarget()->setEulerRotation(rotation);

    if (m_itemLabel->visible())
        m_itemLabel->setEulerRotation(rotation);
}

int QQuickDataVisItem::msaaSamples() const
{
    if (m_renderMode == RenderIndirect)
        return m_samples;
    else
        return m_windowSamples;
}

void QQuickDataVisItem::setMsaaSamples(int samples)
{
    if (m_renderMode != RenderIndirect) {
        qWarning("Multisampling cannot be adjusted in this render mode");
    } else {
        if (m_controller->isOpenGLES()) {
            if (samples > 0)
                qWarning("Multisampling is not supported in OpenGL ES2");
        } else if (m_samples != samples) {
            m_samples = samples;
            setAntialiasing(m_samples > 0);
            auto sceneEnv = environment();
            sceneEnv->setAntialiasingMode(m_samples > 0
                                          ? QQuick3DSceneEnvironment::QQuick3DEnvironmentAAModeValues::MSAA
                                          : QQuick3DSceneEnvironment::QQuick3DEnvironmentAAModeValues::NoAA);
            switch (m_samples) {
            case 0:
                // no-op
                break;
            case 2:
                sceneEnv->setAntialiasingQuality(
                            QQuick3DSceneEnvironment::QQuick3DEnvironmentAAQualityValues::Medium);
                break;
            case 4:
                sceneEnv->setAntialiasingQuality(
                            QQuick3DSceneEnvironment::QQuick3DEnvironmentAAQualityValues::High);
                break;
            case 8:
                sceneEnv->setAntialiasingQuality(
                            QQuick3DSceneEnvironment::QQuick3DEnvironmentAAQualityValues::VeryHigh);
                break;
            default:
                qWarning("Invalid multisampling sample number, using 4x instead");
                sceneEnv->setAntialiasingQuality(
                            QQuick3DSceneEnvironment::QQuick3DEnvironmentAAQualityValues::High);
                m_samples = 4;
                break;
            }
            emit msaaSamplesChanged(m_samples);
            update();
        }
    }
}

Declarative3DScene *QQuickDataVisItem::scene() const
{
    return static_cast<Declarative3DScene *>(m_controller->scene());
}

void QQuickDataVisItem::handleWindowChanged(/*QQuickWindow *window*/)
{
    auto window = QQuick3DObjectPrivate::get(rootNode())->sceneManager->window();
    checkWindowList(window);
    if (!window)
        return;

#if defined(Q_OS_MACOS)
    bool previousVisibility = window->isVisible();
    // Enable touch events for Mac touchpads
    window->setVisible(true);
    typedef void * (*EnableTouch)(QWindow*, bool);
    EnableTouch enableTouch =
            (EnableTouch)QGuiApplication::platformNativeInterface()->nativeResourceFunctionForIntegration("registertouchwindow");
    if (enableTouch)
        enableTouch(window, true);
    window->setVisible(previousVisibility);
#endif

    connect(window, &QObject::destroyed, this, &QQuickDataVisItem::windowDestroyed);

    int oldWindowSamples = m_windowSamples;
    m_windowSamples = window->format().samples();
    if (m_windowSamples < 0)
        m_windowSamples = 0;

    connect(window, &QQuickWindow::beforeSynchronizing,
            this, &QQuickDataVisItem::synchData);

    if (m_renderMode == RenderDirectToBackground_NoClear
            || m_renderMode == RenderDirectToBackground) {
        setAntialiasing(m_windowSamples > 0);
        if (m_windowSamples != oldWindowSamples)
            emit msaaSamplesChanged(m_windowSamples);
    }

    connect(m_controller.data(), &Abstract3DController::needRender, window, &QQuickWindow::update);
    updateWindowParameters();

#if defined(Q_OS_IOS)
    // Scenegraph render cycle in iOS sometimes misses update after beforeSynchronizing signal.
    // This ensures we don't end up displaying the graph without any data, in case update is
    // skipped after synchDataToRenderer.
    QTimer::singleShot(0, window, SLOT(update()));
#endif
}

void QQuickDataVisItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);

    m_cachedGeometry = newGeometry;

    updateWindowParameters();
}

void QQuickDataVisItem::itemChange(ItemChange change, const ItemChangeData  &value)
{
    QQuick3DViewport::itemChange(change, value);
    updateWindowParameters();
}

void QQuickDataVisItem::updateWindowParameters()
{
    const QMutexLocker locker(&m_mutex);

    // Update the device pixel ratio, window size and bounding box
    QQuickWindow *win = window();
    if (win && !m_controller.isNull()) {
        Q3DScene *scene = m_controller->scene();
        if (win->devicePixelRatio() != scene->devicePixelRatio()) {
            scene->setDevicePixelRatio(win->devicePixelRatio());
            win->update();
        }

        bool directRender = m_renderMode == RenderDirectToBackground
                || m_renderMode == RenderDirectToBackground_NoClear;
        QSize windowSize;

        if (directRender)
            windowSize = win->size();
        else
            windowSize = m_cachedGeometry.size().toSize();

        if (windowSize != scene->d_ptr->windowSize()) {
            scene->d_ptr->setWindowSize(windowSize);
            win->update();
        }

        if (directRender) {
            // Origin mapping is needed when rendering directly to background
            QPointF point = QQuickItem::mapToScene(QPointF(0.0, 0.0));
            scene->d_ptr->setViewport(QRect(point.x() + 0.5f, point.y() + 0.5f,
                                            m_cachedGeometry.width() + 0.5f,
                                            m_cachedGeometry.height() + 0.5f));
        } else {
            // No translation needed when rendering to FBO
            scene->d_ptr->setViewport(QRect(0.0, 0.0, m_cachedGeometry.width() + 0.5f,
                                            m_cachedGeometry.height() + 0.5f));
        }
    }
}

void QQuickDataVisItem::handleSelectionModeChange(QAbstract3DGraphNG::SelectionFlags mode)
{
    int intmode = int(mode);
    emit selectionModeChanged(SelectionFlags(intmode));
}

void QQuickDataVisItem::handleShadowQualityChange(QAbstract3DGraphNG::ShadowQuality quality)
{
    emit shadowQualityChanged(ShadowQuality(quality));
}

void QQuickDataVisItem::handleSelectedElementChange(QAbstract3DGraphNG::ElementType type)
{
    emit selectedElementChanged(ElementType(type));
}

void QQuickDataVisItem::handleOptimizationHintChange(QAbstract3DGraphNG::OptimizationHints hints)
{
    int intHints = int(hints);
    emit optimizationHintsChanged(OptimizationHints(intHints));
}

QAbstract3DInputHandler *QQuickDataVisItem::inputHandler() const
{
    return m_controller->activeInputHandler();
}

void QQuickDataVisItem::setInputHandler(QAbstract3DInputHandler *inputHandler)
{
    m_controller->setActiveInputHandler(inputHandler);
}

void QQuickDataVisItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_controller->mouseDoubleClickEvent(event);
}

void QQuickDataVisItem::touchEvent(QTouchEvent *event)
{
    m_controller->touchEvent(event);
    window()->update();
}

void QQuickDataVisItem::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    handleMousePressedEvent(event);
    m_controller->mousePressEvent(event, mousePos);
}

void QQuickDataVisItem::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    m_controller->mouseReleaseEvent(event, mousePos);
}

void QQuickDataVisItem::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    m_controller->mouseMoveEvent(event, mousePos);
}

#if QT_CONFIG(wheelevent)
void QQuickDataVisItem::wheelEvent(QWheelEvent *event)
{
    m_controller->wheelEvent(event);
}
#endif

void QQuickDataVisItem::checkWindowList(QQuickWindow *window)
{
    QQuickWindow *oldWindow = graphWindowList.value(this);
    graphWindowList[this] = window;

    if (oldWindow != window && oldWindow) {
        QObject::disconnect(oldWindow, &QObject::destroyed, this,
                            &QQuickDataVisItem::windowDestroyed);
        QObject::disconnect(oldWindow, &QQuickWindow::beforeSynchronizing, this,
                            &QQuickDataVisItem::synchData);
        if (!m_controller.isNull()) {
            QObject::disconnect(m_controller.data(), &Abstract3DController::needRender,
                                oldWindow, &QQuickWindow::update);
        }
    }

    QList<QQuickWindow *> windowList;

    foreach (QQuickDataVisItem *graph, graphWindowList.keys()) {
        if (graph->m_renderMode == RenderDirectToBackground
                || graph->m_renderMode == RenderDirectToBackground_NoClear) {
            windowList.append(graphWindowList.value(graph));
        }
    }

    if (!window) {
        graphWindowList.remove(this);
        return;
    }
}

void QQuickDataVisItem::setMeasureFps(bool enable)
{
    m_controller->setMeasureFps(enable);
    if (enable)
        QObject::connect(renderStats(), &QQuick3DRenderStats::fpsChanged, this, &QQuickDataVisItem::handleFpsChanged);
    else
        QObject::disconnect(renderStats(), 0, this, 0);

}

bool QQuickDataVisItem::measureFps() const
{
    return m_controller->measureFps();
}

int QQuickDataVisItem::currentFps() const
{
    return m_controller->currentFps();
}

void QQuickDataVisItem::setOrthoProjection(bool enable)
{
    m_controller->setOrthoProjection(enable);
}

bool QQuickDataVisItem::isOrthoProjection() const
{
    return m_controller->isOrthoProjection();
}

QQuickDataVisItem::ElementType QQuickDataVisItem::selectedElement() const
{
    return ElementType(m_controller->selectedElement());
}

void QQuickDataVisItem::setAspectRatio(qreal ratio)
{
    m_controller->setAspectRatio(ratio);
}

qreal QQuickDataVisItem::aspectRatio() const
{
    return m_controller->aspectRatio();
}

void QQuickDataVisItem::setOptimizationHints(OptimizationHints hints)
{
    int intmode = int(hints);
    m_controller->setOptimizationHints(QAbstract3DGraphNG::OptimizationHints(intmode));
}

QQuickDataVisItem::OptimizationHints QQuickDataVisItem::optimizationHints() const
{
    int intmode = int(m_controller->optimizationHints());
    return OptimizationHints(intmode);
}

void QQuickDataVisItem::setPolar(bool enable)
{
    m_controller->setPolar(enable);
}

bool QQuickDataVisItem::isPolar() const
{
    return m_controller->isPolar();
}

void QQuickDataVisItem::setRadialLabelOffset(float offset)
{
    m_controller->setRadialLabelOffset(offset);
}

float QQuickDataVisItem::radialLabelOffset() const
{
    return m_controller->radialLabelOffset();
}

void QQuickDataVisItem::setHorizontalAspectRatio(qreal ratio)
{
    m_controller->setHorizontalAspectRatio(ratio);
}

qreal QQuickDataVisItem::horizontalAspectRatio() const
{
    return m_controller->horizontalAspectRatio();
}

void QQuickDataVisItem::setReflection(bool enable)
{
    m_controller->setReflection(enable);
}

bool QQuickDataVisItem::isReflection() const
{
    return m_controller->reflection();
}

void QQuickDataVisItem::setReflectivity(qreal reflectivity)
{
    m_controller->setReflectivity(reflectivity);
}

qreal QQuickDataVisItem::reflectivity() const
{
    return m_controller->reflectivity();
}

void QQuickDataVisItem::setLocale(const QLocale &locale)
{
    m_controller->setLocale(locale);
}

QLocale QQuickDataVisItem::locale() const
{
    return m_controller->locale();
}

QVector3D QQuickDataVisItem::queriedGraphPosition() const
{
    return m_controller->queriedGraphPosition();
}

void QQuickDataVisItem::setMargin(qreal margin)
{
    m_controller->setMargin(margin);
}

qreal QQuickDataVisItem::margin() const
{
    return m_controller->margin();
}

QQuick3DNode *QQuickDataVisItem::rootNode() const
{
    return QQuick3DViewport::scene();
}

void QQuickDataVisItem::changeLabelBackgroundColor(QQuick3DRepeater *repeater, const QColor &color)
{
    int count = repeater->count();
    for (int i = 0; i < count; i++) {
        auto label = static_cast<QQuick3DNode *>(repeater->objectAt(i));
        label->setProperty("backgroundColor", color);
    }
}

void QQuickDataVisItem::changeLabelBackgroundEnabled(QQuick3DRepeater *repeater, const bool &enabled)
{
    int count = repeater->count();
    for (int i = 0; i < count; i++) {
        auto label = static_cast<QQuick3DNode *>(repeater->objectAt(i));
        label->setProperty("backgroundEnabled", enabled);
    }
}

void QQuickDataVisItem::changeLabelBorderEnabled(QQuick3DRepeater *repeater, const bool &enabled)
{
    int count = repeater->count();
    for (int i = 0; i < count; i++) {
        auto label = static_cast<QQuick3DNode *>(repeater->objectAt(i));
        label->setProperty("borderEnabled", enabled);
    }
}

void QQuickDataVisItem::changeLabelTextColor(QQuick3DRepeater *repeater, const QColor &color)
{
    int count = repeater->count();
    for (int i = 0; i < count; i++) {
        auto label = static_cast<QQuick3DNode *>(repeater->objectAt(i));
        label->setProperty("labelTextColor", color);
    }
}

void QQuickDataVisItem::changeLabelFont(QQuick3DRepeater *repeater, const QFont &font)
{
    int count = repeater->count();
    for (int i = 0; i < count; i++) {
        auto label = static_cast<QQuick3DNode *>(repeater->objectAt(i));
        label->setProperty("labelFont", font);
    }
}

void QQuickDataVisItem::changeGridLineColor(QQuick3DRepeater *repeater, const QColor &color)
{
    for (int i = 0; i < repeater->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(repeater->objectAt(i));
        lineNode->setProperty("lineColor", color);
    }
}

void QQuickDataVisItem::updateTitleLabels()
{
    if (m_controller->m_changeTracker.axisXTitleVisibilityChanged) {
        m_titleLabelX->setVisible(m_controller->axisX()->isTitleVisible());
        m_controller->m_changeTracker.axisXTitleVisibilityChanged = false;
    }

    if (m_controller->m_changeTracker.axisYTitleVisibilityChanged) {
        m_titleLabelY->setVisible(m_controller->axisY()->isTitleVisible());
        m_controller->m_changeTracker.axisYTitleVisibilityChanged = false;
    }

    if (m_controller->m_changeTracker.axisZTitleVisibilityChanged) {
        m_titleLabelZ->setVisible(m_controller->axisZ()->isTitleVisible());
        m_controller->m_changeTracker.axisZTitleVisibilityChanged = false;
    }

    if (m_controller->m_changeTracker.axisXTitleChanged) {
        m_titleLabelX->setProperty("labelText", m_controller->axisX()->title());
        m_controller->m_changeTracker.axisXTitleChanged = false;
    }

    if (m_controller->m_changeTracker.axisYTitleChanged) {
        m_titleLabelY->setProperty("labelText", m_controller->axisY()->title());
        m_controller->m_changeTracker.axisYTitleChanged = false;
    }

    if (m_controller->m_changeTracker.axisZTitleChanged) {
        m_titleLabelZ->setProperty("labelText", m_controller->axisZ()->title());
        m_controller->m_changeTracker.axisZTitleChanged = false;
    }
}

void QQuickDataVisItem::updateSliceGraph()
{
    if (!m_sliceEnabled) {
        m_controller->setSlicingActive(false);
        setWidth(width() * 5.f);
        setHeight(height() * 5.f);
        m_sliceView->setVisible(false);
    } else {
        float pixelRatio = QQuick3DObjectPrivate::get(rootNode())->sceneManager->window()->devicePixelRatio();
        float magnification = 100.0f * pixelRatio + 50.0f;
        QQuick3DOrthographicCamera *camera = static_cast<QQuick3DOrthographicCamera *>(sliceView()->camera());
        camera->setHorizontalMagnification(magnification);
        camera->setVerticalMagnification(magnification);
        QQuickItem *anchor = QQuickItemPrivate::get(this)->anchors()->fill();
        if (anchor)
            QQuickItemPrivate::get(this)->anchors()->resetFill();
        setWidth(width() * .2f);
        setHeight(height() * .2f);
        m_sliceView->setVisible(true);
        updateSliceGrid();
        updateSliceLabels();
    }
}

void QQuickDataVisItem::windowDestroyed(QObject *obj)
{
    // Remove destroyed window from window lists
    QQuickWindow *win = static_cast<QQuickWindow *>(obj);
    QQuickWindow *oldWindow = graphWindowList.value(this);

    if (win == oldWindow)
        graphWindowList.remove(this);
}

QQmlComponent *QQuickDataVisItem::createRepeaterDelegateComponent(const QString &fileName)
{
    QQmlComponent component(qmlEngine(this), fileName);
    return qobject_cast<QQmlComponent *>(component.create());
}

QQuick3DRepeater *QQuickDataVisItem::createRepeater()
{
    auto engine = qmlEngine(this);
    QQmlComponent repeaterComponent(engine);
    repeaterComponent.setData("import QtQuick3D; Repeater3D{}",QUrl());
    auto repeater = qobject_cast<QQuick3DRepeater *>(repeaterComponent.create());
    repeater->setParent(rootNode());
    repeater->setParentItem(rootNode());
    return repeater;
}

QQuick3DNode *QQuickDataVisItem::createTitleLabel()
{
    QQmlComponent comp(qmlEngine(this), QStringLiteral(":/axis/ItemLabel"));
    auto titleLabel = qobject_cast<QQuick3DNode *>(comp.create());
    titleLabel->setParent(rootNode());
    titleLabel->setParentItem(rootNode());
    titleLabel->setVisible(false);
    titleLabel->setScale(m_labelScale);
    return titleLabel;
}

QQuick3DCustomMaterial *QQuickDataVisItem::createQmlCustomMaterial(const QString &fileName)
{
    QQmlComponent component(qmlEngine(this), fileName);
    QQuick3DCustomMaterial *material = qobject_cast<QQuick3DCustomMaterial *>(component.create());
    return material;
}

QQuick3DPrincipledMaterial *QQuickDataVisItem::createPrincipledMaterial()
{
    QQmlComponent component(qmlEngine(this));
    component.setData("import QtQuick3D; PrincipledMaterial{}", QUrl());
    return qobject_cast<QQuick3DPrincipledMaterial *>(component.create());
}

bool QQuickDataVisItem::event(QEvent *event)
{
    return QQuickItem::event(event);
}

void QQuickDataVisItem::createSliceView()
{
    if (m_sliceView)
        return;

    m_sliceView = new QQuick3DViewport();
    m_sliceView->setParent(parent());
    m_sliceView->setParentItem(parentItem());
    m_sliceView->setVisible(false);
    m_sliceView->setWidth(width());
    m_sliceView->setHeight(height());

    auto scene = m_sliceView->scene();

    auto camera = new QQuick3DOrthographicCamera(scene);
    camera->setPosition(QVector3D(.0f, .0f, 10.0f));
    m_sliceView->setCamera(camera);

    auto light = new QQuick3DDirectionalLight(scene);
    light->setParent(camera);
    light->setParentItem(camera);

    m_sliceHorizontalGridRepeater = createRepeater();
    m_sliceHorizontalGridRepeater->setParent(scene);
    m_sliceHorizontalGridRepeater->setParentItem(scene);
    auto gridDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    m_sliceHorizontalGridRepeater->setDelegate(gridDelegate);

    m_sliceVerticalGridRepeater = createRepeater();
    m_sliceVerticalGridRepeater->setParent(scene);
    m_sliceVerticalGridRepeater->setParentItem(scene);
    m_sliceVerticalGridRepeater->setDelegate(gridDelegate);

    m_sliceHorizontalLabelRepeater = createRepeater();
    m_sliceHorizontalLabelRepeater->setParent(scene);
    m_sliceHorizontalLabelRepeater->setParentItem(scene);
    auto labelDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/AxisLabel"));
    m_sliceHorizontalLabelRepeater->setDelegate(labelDelegate);

    m_sliceVerticalLabelRepeater = createRepeater();
    m_sliceVerticalLabelRepeater->setParent(scene);
    m_sliceVerticalLabelRepeater->setParentItem(scene);
    m_sliceVerticalLabelRepeater->setDelegate(labelDelegate);

    m_sliceHorizontalTitleLabel = createTitleLabel();
    m_sliceHorizontalTitleLabel->setParent(scene);
    m_sliceHorizontalTitleLabel->setParentItem(scene);
    m_sliceHorizontalTitleLabel->setVisible(true);

    m_sliceVerticalTitleLabel = createTitleLabel();
    m_sliceVerticalTitleLabel->setParent(scene);
    m_sliceVerticalTitleLabel->setParentItem(scene);
    m_sliceVerticalTitleLabel->setVisible(true);
}

void QQuickDataVisItem::updateSliceGrid()
{
    QAbstract3DAxis *horizontalAxis = nullptr;
    QAbstract3DAxis *verticalAxis = m_controller->axisY();
    auto backgroundScale = m_scaleWithBackground + m_backgroundScaleMargin;
    float scale;
    float translate;

    QVector3D horizontalScale = QVector3D(.0f, .0f, .0f);
    QVector3D verticalScale = QVector3D(lineWidthScaleFactor(),
                                        backgroundScale.y() * lineLengthScaleFactor(),
                                        lineWidthScaleFactor());
    auto selectionMode = m_controller->selectionMode();
    if (selectionMode.testFlag(QAbstract3DGraphNG::SelectionRow)) {
        horizontalAxis = m_controller->axisX();
        horizontalScale = QVector3D(backgroundScale.x() * lineLengthScaleFactor(),
                                    lineWidthScaleFactor(),
                                    lineWidthScaleFactor());
        scale = m_scaleWithBackground.x();
        translate = m_scaleWithBackground.x();
    } else if (selectionMode.testFlag(QAbstract3DGraphNG::SelectionColumn)) {
        horizontalAxis = m_controller->axisZ();
        horizontalScale = QVector3D(backgroundScale.z() * lineLengthScaleFactor(),
                                    lineWidthScaleFactor(),
                                    lineWidthScaleFactor());
        scale = m_scaleWithBackground.z();
        translate = m_scaleWithBackground.z();
    }

    if (horizontalAxis == nullptr) {
        qWarning("Invalid axis type");
        return;
    }

    if (horizontalAxis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(horizontalAxis);
        m_sliceVerticalGridRepeater->setModel(valueAxis->gridSize()
                                              + valueAxis->subGridSize());
    } else if (horizontalAxis->type() & QAbstract3DAxis::AxisTypeCategory) {
        m_sliceVerticalGridRepeater->setModel(horizontalAxis->labels().size());
    }

    if (verticalAxis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(verticalAxis);
        m_sliceHorizontalGridRepeater->setModel(valueAxis->gridSize()
                                              + valueAxis->subGridSize());
    } else if (horizontalAxis->type() & QAbstract3DAxis::AxisTypeCategory) {
        m_sliceHorizontalGridRepeater->setModel(verticalAxis->labels().size());
    }

    float linePosX = .0f;
    float linePosY = .0f;
    float linePosZ = .0f;

    if (horizontalAxis->type() == QAbstract3DAxis::AxisTypeValue) {
        for (int i = 0; i < m_sliceVerticalGridRepeater->count(); i++) {
            QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(m_sliceVerticalGridRepeater->objectAt(i));
            auto axis = static_cast<QValue3DAxis *>(horizontalAxis);
            if (i % 2 == 0)
                linePosX = axis->gridPositionAt(i / 2) * scale * 2.0f - translate;
            else
                linePosX = axis->subGridPositionAt(i / 2) * scale * 2.0f - translate;
            lineNode->setProperty("lineColor", QColor(0, 0, 0));
            positionAndScaleLine(lineNode, verticalScale, QVector3D(linePosX, linePosY, linePosZ));
        }
    }

    linePosX = 0;
    scale = m_scaleWithBackground.y();
    translate = m_scaleWithBackground.y();

    for (int i = 0; i < m_sliceHorizontalGridRepeater->count(); i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(m_sliceHorizontalGridRepeater->objectAt(i));
        auto axis = static_cast<QValue3DAxis *>(verticalAxis);
        if (horizontalAxis->type() == QAbstract3DAxis::AxisTypeValue) {
            if (i % 2 == 0)
                linePosY = axis->gridPositionAt(i / 2) * scale * 2.0f - translate;
            else
                linePosY = axis->subGridPositionAt(i / 2) * scale * 2.0f - translate;
        } else if (horizontalAxis->type() == QAbstract3DAxis::AxisTypeCategory) {
            linePosY = axis->gridPositionAt(i) * scale * 2.0f - translate;
        }
        lineNode->setProperty("lineColor", QColor(0, 0, 0));
        positionAndScaleLine(lineNode, horizontalScale, QVector3D(linePosX, linePosY, linePosZ));
    }
}

void QQuickDataVisItem::updateSliceLabels()
{
    QAbstract3DAxis *horizontalAxis = nullptr;
    QAbstract3DAxis *verticalAxis = m_controller->axisY();
    auto backgroundScale = m_scaleWithBackground + m_backgroundScaleMargin;
    float scale;
    float translate;
    float scaleFactor;
    auto pointSize = m_controller->activeTheme()->font().pointSizeF();
    auto selectionMode = m_controller->selectionMode();

    if (selectionMode.testFlag(QAbstract3DGraphNG::SelectionRow)) {
        horizontalAxis = m_controller->axisX();
        scale = backgroundScale.x() - m_backgroundScaleMargin.x();
        translate = backgroundScale.x() - m_backgroundScaleMargin.x();
        scaleFactor = m_labelScale.x() * m_labelFontScaleFactor / pointSize
                + m_labelScale.x() * m_fontScaleFactor;
    } else if (selectionMode.testFlag(QAbstract3DGraphNG::SelectionColumn)) {
        horizontalAxis = m_controller->axisZ();
        scale = m_scaleWithBackground.z() - m_backgroundScaleMargin.z();
        translate = m_scaleWithBackground.z() - m_backgroundScaleMargin.z();
        scaleFactor = m_labelScale.z() * m_labelFontScaleFactor / pointSize
                + m_labelScale.z() * m_fontScaleFactor;
    }

    if (horizontalAxis == nullptr) {
        qWarning("Invalid axis type");
        return;
    }

    if (horizontalAxis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(horizontalAxis);
        m_sliceHorizontalLabelRepeater->setModel(valueAxis->labels().size());
    } else if (horizontalAxis->type() & QAbstract3DAxis::AxisTypeCategory) {
        m_sliceHorizontalLabelRepeater->setModel(horizontalAxis->labels().size());
    }

    if (verticalAxis->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(verticalAxis);
        m_sliceVerticalLabelRepeater->setModel(valueAxis->labels().size());
    } else if (horizontalAxis->type() & QAbstract3DAxis::AxisTypeCategory) {
        m_sliceVerticalLabelRepeater->setModel(verticalAxis->labels().size());
    }

    float textPadding = 12.0f;
    scaleFactor *= .8f;

    QVector3D fontScaled = QVector3D(scaleFactor, scaleFactor, 0.0f);

    float labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(horizontalAxis->labels()))) + textPadding;
    QFontMetrics fm(m_controller->activeTheme()->font());
    float labelHeight = fm.height() + textPadding;

    float adjustment = labelsMaxWidth * scaleFactor;
    float yPos = backgroundScale.y() + adjustment;

    QVector3D labelTrans = QVector3D(0.0f, -yPos, 0.0f);
    QStringList labels = horizontalAxis->labels();
    Q3DTheme *theme = m_controller->activeTheme();
    QFont font = theme->font();
    bool borderEnabled = theme->isLabelBorderEnabled();
    QColor labelTextColor = theme->labelTextColor();
    bool backgroundEnabled = theme->isLabelBackgroundEnabled();
    QColor backgroundColor = theme->labelBackgroundColor();

    if (horizontalAxis->type() == QAbstract3DAxis::AxisTypeValue) {
        auto valueAxis = static_cast<QValue3DAxis *>(horizontalAxis);
        for (int i = 0; i < m_sliceHorizontalLabelRepeater->count(); i++) {
            auto obj = static_cast<QQuick3DNode *>(m_sliceHorizontalLabelRepeater->objectAt(i));
            labelTrans.setX(valueAxis->labelPositionAt(i) * scale * 2.0f - translate);
            obj->setScale(fontScaled);
            obj->setPosition(labelTrans);
            obj->setProperty("labelText", labels[i]);
            obj->setProperty("labelWidth", labelsMaxWidth);
            obj->setProperty("labelHeight", labelHeight);
            obj->setProperty("labelFont", font);
            obj->setProperty("borderEnabled", borderEnabled);
            obj->setProperty("labelTextColor", labelTextColor);
            obj->setProperty("backgroundEnabled", backgroundEnabled);
            obj->setProperty("backgroundColor", backgroundColor);
            obj->setEulerRotation(QVector3D(.0f, .0f, -45.0f));
        }
    } else if (horizontalAxis->type() == QAbstract3DAxis::AxisTypeCategory) {
        for (int i = 0; i < m_sliceHorizontalLabelRepeater->count(); i++) {
            labelTrans = calculateCategoryLabelPosition(horizontalAxis, labelTrans, i);
            labelTrans.setY(labelTrans.y() - adjustment);
            auto obj = static_cast<QQuick3DNode *>(m_sliceHorizontalLabelRepeater->objectAt(i));
            obj->setScale(fontScaled);
            obj->setPosition(labelTrans);
            obj->setEulerRotation(QVector3D(0.0f, 0.0f, -45.0f));
            obj->setProperty("labelText", labels[i]);
            obj->setProperty("labelWidth", labelsMaxWidth);
            obj->setProperty("labelHeight", labelHeight);
            obj->setProperty("labelFont", font);
            obj->setProperty("borderEnabled", borderEnabled);
            obj->setProperty("labelTextColor", labelTextColor);
            obj->setProperty("backgroundEnabled", backgroundEnabled);
            obj->setProperty("backgroundColor", backgroundColor);
        }
    }

    scale = backgroundScale.y() - m_backgroundScaleMargin.y();
    translate = backgroundScale.y() - m_backgroundScaleMargin.y();
    labels = verticalAxis->labels();
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(labels))) + textPadding;
    adjustment = labelsMaxWidth * scaleFactor;
    float xPos = backgroundScale.x() + adjustment;
    labelTrans = QVector3D(xPos, 0.0f, 0.0f);

    if (verticalAxis->type() == QAbstract3DAxis::AxisTypeValue) {
        auto valueAxis = static_cast<QValue3DAxis *>(verticalAxis);
        for (int i = 0; i < m_sliceVerticalLabelRepeater->count(); i++) {
            auto obj = static_cast<QQuick3DNode *>(m_sliceVerticalLabelRepeater->objectAt(i));
            labelTrans.setY(valueAxis->labelPositionAt(i) * scale * 2.0f - translate);
            obj->setScale(fontScaled);
            obj->setPosition(labelTrans);
            obj->setProperty("labelText", labels[i]);
            obj->setProperty("labelWidth", labelsMaxWidth);
            obj->setProperty("labelHeight", labelHeight);
            obj->setProperty("labelFont", font);
            obj->setProperty("borderEnabled", borderEnabled);
            obj->setProperty("labelTextColor", labelTextColor);
            obj->setProperty("backgroundEnabled", backgroundEnabled);
            obj->setProperty("backgroundColor", backgroundColor);
        }
    } else if (verticalAxis->type() == QAbstract3DAxis::AxisTypeCategory) {
        for (int i = 0; i < m_sliceVerticalLabelRepeater->count(); i++) {
            labelTrans = calculateCategoryLabelPosition(verticalAxis, labelTrans, i);
            auto obj = static_cast<QQuick3DNode *>(m_sliceVerticalLabelRepeater->objectAt(i));
            obj->setScale(fontScaled);
            obj->setPosition(labelTrans);
            obj->setProperty("labelText", labels[i]);
            obj->setProperty("labelWidth", labelsMaxWidth);
            obj->setProperty("labelHeight", labelHeight);
            obj->setProperty("labelFont", font);
            obj->setProperty("borderEnabled", borderEnabled);
            obj->setProperty("labelTextColor", labelTextColor);
            obj->setProperty("backgroundEnabled", backgroundEnabled);
            obj->setProperty("backgroundColor", backgroundColor);
        }
    }

    labelHeight = fm.height() + textPadding;
    float labelWidth = fm.horizontalAdvance(verticalAxis->title()) + textPadding;
    adjustment = labelHeight * scaleFactor;
    xPos = backgroundScale.x() + adjustment;
    labelTrans = QVector3D(-xPos, 0.0f, 0.0f);

    m_sliceVerticalTitleLabel->setScale(fontScaled);
    m_sliceVerticalTitleLabel->setPosition(labelTrans);
    m_sliceVerticalTitleLabel->setProperty("labelWidth", labelWidth);
    m_sliceVerticalTitleLabel->setProperty("labelHeight", labelHeight);
    m_sliceVerticalTitleLabel->setProperty("labelText", verticalAxis->title());
    m_sliceVerticalTitleLabel->setProperty("labelFont", font);
    m_sliceVerticalTitleLabel->setProperty("borderEnabled", borderEnabled);
    m_sliceVerticalTitleLabel->setProperty("labelTextColor", labelTextColor);
    m_sliceVerticalTitleLabel->setProperty("backgroundEnabled", backgroundEnabled);
    m_sliceVerticalTitleLabel->setProperty("backgroundColor", backgroundColor);
    m_sliceVerticalTitleLabel->setEulerRotation(QVector3D(.0f, .0f, 90.0f));

    labelHeight = fm.height() + textPadding;
    labelWidth = fm.horizontalAdvance(horizontalAxis->title()) + textPadding;
    adjustment = labelHeight * scaleFactor;
    yPos = backgroundScale.y() * 1.5f + adjustment;
    labelTrans = QVector3D(0.0f, -yPos, 0.0f);

    if (!horizontalAxis->title().isEmpty()) {
        m_sliceHorizontalTitleLabel->setScale(fontScaled);
        m_sliceHorizontalTitleLabel->setPosition(labelTrans);
        m_sliceHorizontalTitleLabel->setProperty("labelWidth", labelWidth);
        m_sliceHorizontalTitleLabel->setProperty("labelHeight", labelHeight);
        m_sliceHorizontalTitleLabel->setProperty("labelText", horizontalAxis->title());
        m_sliceHorizontalTitleLabel->setProperty("labelFont", font);
        m_sliceHorizontalTitleLabel->setProperty("borderEnabled", borderEnabled);
        m_sliceHorizontalTitleLabel->setProperty("labelTextColor", labelTextColor);
        m_sliceHorizontalTitleLabel->setProperty("backgroundEnabled", backgroundEnabled);
        m_sliceHorizontalTitleLabel->setProperty("backgroundColor", backgroundColor);
    } else {
        m_sliceHorizontalTitleLabel->setVisible(false);
    }
}

void QQuickDataVisItem::setUpCamera()
{
    auto useOrtho = m_controller->isOrthoProjection();
    QQuick3DCamera *camera;
    if (!useOrtho) {
        auto persCamera = new QQuick3DPerspectiveCamera(rootNode());
        persCamera->setClipNear(0.001f);
        persCamera->setFieldOfView(45.0f);
        camera = persCamera;
    } else {
        auto orthCamera = new QQuick3DOrthographicCamera(rootNode());
        camera = orthCamera;
    }

    QQuick3DObjectPrivate::get(camera)->refSceneManager(
                *QQuick3DObjectPrivate::get(rootNode())->sceneManager);
    auto cameraTarget = new QQuick3DNode(rootNode());

    setCameraTarget(cameraTarget);
    cameraTarget->setPosition(QVector3D(0, 0, 0));
    QQuick3DObjectPrivate::get(cameraTarget)->refSceneManager(
                *QQuick3DObjectPrivate::get(rootNode())->sceneManager);

    camera->setParent(cameraTarget);
    camera->setParentItem(cameraTarget);

    camera->setPosition(QVector3D(0, 0, 5));
    camera->lookAt(cameraTarget);
    setCamera(camera);
}

void QQuickDataVisItem::setUpLight()
{
    auto light = new QQuick3DDirectionalLight(rootNode());
    QQuick3DObjectPrivate::get(light)->refSceneManager(
                *QQuick3DObjectPrivate::get(rootNode())->sceneManager);
    light->setParent(camera());
    light->setParentItem(camera());
    m_light = light;
}

QT_END_NAMESPACE

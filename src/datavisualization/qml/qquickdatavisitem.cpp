// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickdatavisitem_p.h"
#include "declarativetheme_p.h"
#include "declarativescene_p.h"

#include "abstract3dcontroller_p.h"
#include "utils_p.h"

#include <QtGui/QGuiApplication>

#include <QtQuick3D/private/qquick3dperspectivecamera_p.h>
#include <QtQuick3D/private/qquick3dorthographiccamera_p.h>
#include <QtQuick3D/private/qquick3dcustommaterial_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>
#include <QtQuick3D/private/qquick3ddirectionallight_p.h>
#include <QtQuick3D/private/qquick3drepeater_p.h>

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
    environment()->setAntialiasingMode(QQuick3DSceneEnvironment::QQuick3DEnvironmentAAModeValues::NoAA);
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

QQuick3DModel *QQuickDataVisItem::backgroundBB() const
{
    return m_backgroundBB;
}

void QQuickDataVisItem::setBackgroundBB(QQuick3DModel *newBackgroundBB)
{
    m_backgroundBB = newBackgroundBB;
}

void QQuickDataVisItem::handleFpsChanged()
{
    int fps = renderStats()->fps();
    emit currentFpsChanged(fps);
}

void QQuickDataVisItem::componentComplete()
{
    qDebug()<<staticMetaObject.className()<<__func__;
    QQuick3DViewport::componentComplete();

    auto url = QUrl(QStringLiteral("defaultMeshes/backgroundMesh"));
    auto background = new QQuick3DModel();
    m_backgroundScale = new QQuick3DNode();
    m_backgroundRotation = new QQuick3DNode();

    m_backgroundScale->setParent(rootNode());
    m_backgroundScale->setParentItem(rootNode());

    m_backgroundRotation->setParent    (m_backgroundScale);
    m_backgroundRotation->setParentItem(m_backgroundScale);


    background->setObjectName("Background");
    background->setParent    (m_backgroundRotation);
    background->setParentItem(m_backgroundRotation);

    background->setSource(url);
//    background->setPickable(true);

    auto backgroundBB = new QQuick3DModel();
    backgroundBB->setObjectName("BackgroundBB");
    backgroundBB->setParent    (background);
    backgroundBB->setParentItem(background);
//    backgroundBB->setScale(QVector3D(0.1f,0.1f,0.1f));
    backgroundBB->setSource(QUrl(QStringLiteral("defaultMeshes/barMeshFull")));
    backgroundBB->setPickable(true);

    setBackground(background);
    setBackgroundBB(backgroundBB);
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

    if (m_controller->axisX()->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(m_controller->axisX());
        int segmentCount = valueAxis->segmentCount();
        int subSegmentCount = valueAxis->subSegmentCount();
        int gridLineCount = 2 * (segmentCount + 1);
        int subGridLineCount = 2 * (segmentCount * (subSegmentCount - 1));
        m_segmentLineRepeaterX->setModel(gridLineCount);
        m_subsegmentLineRepeaterX->setModel(subGridLineCount);
        m_repeaterX->setModel(valueAxis->labels().count());
        m_controller->handleAxisLabelsChangedBySender(m_controller->axisX());
    }
    if (m_controller->axisY()->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(m_controller->axisY());
        int segmentCount = valueAxis->segmentCount();
        int subSegmentCount = valueAxis->subSegmentCount();
        int gridLineCount = 2 * (segmentCount + 1);
        int subGridLineCount = 2 * (segmentCount * (subSegmentCount - 1));
        m_segmentLineRepeaterY->setModel(gridLineCount);
        m_subsegmentLineRepeaterY->setModel(subGridLineCount);
        m_repeaterY->setModel(2 * valueAxis->labels().count());
        m_controller->handleAxisLabelsChangedBySender(m_controller->axisY());
    }
    if (m_controller->axisZ()->type() & QAbstract3DAxis::AxisTypeValue) {
        QValue3DAxis *valueAxis = static_cast<QValue3DAxis *>(m_controller->axisZ());
        int segmentCount = valueAxis->segmentCount();
        int subSegmentCount = valueAxis->subSegmentCount();
        int gridLineCount = 2 * (segmentCount + 1);
        int subGridLineCount = 2 * (segmentCount * (subSegmentCount - 1));
        m_segmentLineRepeaterZ->setModel(gridLineCount);
        m_subsegmentLineRepeaterZ->setModel(subGridLineCount);
        m_repeaterZ->setModel(valueAxis->labels().count());
        m_controller->handleAxisLabelsChangedBySender(m_controller->axisZ());
    }
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
    m_controller->setSelectionMode(QAbstract3DGraph::SelectionFlags(intmode));
}

QQuickDataVisItem::SelectionFlags QQuickDataVisItem::selectionMode() const
{
    int intmode = int(m_controller->selectionMode());
    return SelectionFlags(intmode);
}

void QQuickDataVisItem::setShadowQuality(ShadowQuality quality)
{
    m_controller->setShadowQuality(QAbstract3DGraph::ShadowQuality(quality));
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
        m_samples = 4;
    setAntialiasing(m_samples > 0);

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
    if (m_controller->m_changeTracker.axisXFormatterChanged) {
        m_controller->m_changeTracker.axisXFormatterChanged = false;
        QAbstract3DAxis *axisX = m_controller->axisX();
        if (axisX->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisX = static_cast<QValue3DAxis *>(axisX);
            valueAxisX->recalculate();
        }
    }

    if (m_controller->m_changeTracker.axisYFormatterChanged) {
        m_controller->m_changeTracker.axisYFormatterChanged = false;
        QAbstract3DAxis *axisY = m_controller->axisY();
        if (axisY->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisY = static_cast<QValue3DAxis *>(axisY);
            valueAxisY->recalculate();
        }
    }

    if (m_controller->m_changeTracker.axisZFormatterChanged) {
        m_controller->m_changeTracker.axisZFormatterChanged = false;
        QAbstract3DAxis *axisZ = m_controller->axisZ();
        if (axisZ->type() & QAbstract3DAxis::AxisTypeValue) {
            QValue3DAxis *valueAxisZ = static_cast<QValue3DAxis *>(axisZ);
            valueAxisZ->recalculate();
        }
    }

    QVector3D forward = camera()->forward();
    auto targetRotation = cameraTarget()->rotation();
    m_flipped.setY(targetRotation.x() > 0);
    m_flipped.setX(forward.x() > 0);
    m_flipped.setZ(forward.z() >= 0);

    QMatrix4x4 modelMatrix;
    QVector3D bgScale;
    if (!m_flipScales)
        bgScale = QVector3D(m_scaleWithBackground.x(), m_scaleWithBackground.y(), m_scaleWithBackground.z());
    else
        bgScale = QVector3D(m_scaleWithBackground.z(), m_scaleWithBackground.y(), m_scaleWithBackground.x());
    m_backgroundScale->setScale(bgScale);

    QVector3D rotVec;
    if (!m_manualRotation) {
        if (!m_flipped.y()) {
            rotVec = QVector3D(0, 270, 0);
            if (m_flipped.x() && m_flipped.z())
                rotVec.setY(90);
            else if (!m_flipped.x() && m_flipped.z())
                rotVec.setY(0);
            else if (m_flipped.x() && !m_flipped.z())
                rotVec.setY(180);
        } else {
            rotVec = QVector3D(0, 180, 180);
            if (m_flipped.x() && m_flipped.z())
                rotVec.setY(0);
            else if (!m_flipped.x() && m_flipped.z())
                rotVec.setY(270);
            else if (m_flipped.x() && !m_flipped.z())
                rotVec.setY(90);
        }
    } else {
        rotVec = QVector3D(m_rot.x(), m_rot.y(), m_rot.z());
    }

    auto rotation = Utils::calculateRotation(rotVec);
    if (m_flipped.y()) {
        m_backgroundRotation->setRotation(/*xFlipRot **/ rotation);
    } else {
        modelMatrix.rotate(rotation);
        m_backgroundRotation->setRotation(rotation);
    }

    updateGrid();

    if (m_controller->graphPositionQueryPending())
        graphPositionAt(m_controller->scene()->graphPositionQuery());

    updateCamera();

    Q3DTheme *theme = m_controller->activeTheme();
    if (m_controller->m_changeTracker.themeChanged) {
        environment()->setClearColor(theme->windowColor());

        m_controller->m_changeTracker.themeChanged = false;
    }

    Q3DThemeDirtyBitField themeDirtyBits = theme->d_ptr->m_dirtyBits;

    if (themeDirtyBits.lightStrengthDirty) {
        light()->setBrightness(theme->lightStrength() * 0.5f + 0.000002f);
        themeDirtyBits.lightStrengthDirty = false;
    }

    if (themeDirtyBits.ambientLightStrengthDirty) {
        float ambientStrength = theme->ambientLightStrength();
        QColor ambientColor = QColor::fromRgbF(ambientStrength, ambientStrength, ambientStrength);
        light()->setAmbientColor(ambientColor);
        themeDirtyBits.ambientLightStrengthDirty = false;
    }

    if (themeDirtyBits.lightColorDirty) {
        light()->setColor(theme->lightColor());
        themeDirtyBits.lightColorDirty = false;
    }

    // label Adjustments
    if (themeDirtyBits.labelBackgroundColorDirty) {
        QColor labelBackgroundColor = theme->labelBackgroundColor();
        changeLabelBackgroundColor(m_repeaterX, labelBackgroundColor);
        changeLabelBackgroundColor(m_repeaterY, labelBackgroundColor);
        changeLabelBackgroundColor(m_repeaterZ, labelBackgroundColor);
        m_titleLabelX->setProperty("backgroundColor", labelBackgroundColor);
        m_titleLabelY->setProperty("backgroundColor", labelBackgroundColor);
        m_titleLabelZ->setProperty("backgroundColor", labelBackgroundColor);
        m_itemLabel->setProperty("backgroundColor", labelBackgroundColor);
        themeDirtyBits.labelBackgroundColorDirty = false;
    }

    if (themeDirtyBits.labelBackgroundEnabledDirty) {
        bool enabled = theme->isLabelBackgroundEnabled();
        changeLabelBackgroundEnabled(m_repeaterX, enabled);
        changeLabelBackgroundEnabled(m_repeaterY, enabled);
        changeLabelBackgroundEnabled(m_repeaterZ, enabled);
        m_titleLabelX->setProperty("backgroundEnabled", enabled);
        m_titleLabelY->setProperty("backgroundEnabled", enabled);
        m_titleLabelZ->setProperty("backgroundEnabled", enabled);
        m_itemLabel->setProperty("backgroundEnabled", enabled);
        themeDirtyBits.labelBackgroundEnabledDirty = false;
    }

    if (themeDirtyBits.labelBorderEnabledDirty) {
        bool enabled = theme->isLabelBorderEnabled();
        changeLabelBorderEnabled(m_repeaterX, enabled);
        changeLabelBorderEnabled(m_repeaterY, enabled);
        changeLabelBorderEnabled(m_repeaterZ, enabled);
        m_titleLabelX->setProperty("borderEnabled", enabled);
        m_titleLabelY->setProperty("borderEnabled", enabled);
        m_titleLabelZ->setProperty("borderEnabled", enabled);
        m_itemLabel->setProperty("borderEnabled", enabled);
        themeDirtyBits.labelBorderEnabledDirty = false;
    }

    if (themeDirtyBits.labelTextColorDirty) {
        QColor labelTextColor = theme->labelTextColor();
        changeLabelTextColor(m_repeaterX, labelTextColor);
        changeLabelTextColor(m_repeaterY, labelTextColor);
        changeLabelTextColor(m_repeaterZ, labelTextColor);
        m_titleLabelX->setProperty("labelTextColor", labelTextColor);
        m_titleLabelY->setProperty("labelTextColor", labelTextColor);
        m_titleLabelZ->setProperty("labelTextColor", labelTextColor);
        m_itemLabel->setProperty("labelTextColor", labelTextColor);
        themeDirtyBits.labelTextColorDirty = false;
    }

    if (themeDirtyBits.fontDirty) {
        auto font = theme->font();
        if (font.pixelSize() == -1)
            font.setPointSizeF(font.pointSizeF() * 0.3);
        changeLabelFont(m_repeaterX, font);
        changeLabelFont(m_repeaterY, font);
        changeLabelFont(m_repeaterZ, font);
        m_titleLabelX->setProperty("labelFont", font);
        m_titleLabelY->setProperty("labelFont", font);
        m_titleLabelZ->setProperty("labelFont", font);
        m_itemLabel->setProperty("labelFont", font);
        themeDirtyBits.fontDirty = false;
    }

    updateLabels();

    // Grid and background adjustments
    if (themeDirtyBits.backgroundColorDirty) {
        auto bg = background();
        QQmlListReference materialsRef(bg, "materials");
        QQuick3DPrincipledMaterial *bgMat;
        if (!materialsRef.count()) {
            bgMat = new QQuick3DPrincipledMaterial();
            bgMat->setSpecularAmount(.5f);
            bgMat->setRoughness(.5f);
            bgMat->setBaseColor(Qt::gray);
            bgMat->setBaseColor(Qt::green);
            materialsRef.append(bgMat);
        } else {
            bgMat = static_cast<QQuick3DPrincipledMaterial *>(materialsRef.at(0));
        }
        bgMat->setBaseColor(theme->backgroundColor());

        themeDirtyBits.backgroundColorDirty = false;
    }

    if (themeDirtyBits.backgroundEnabledDirty) {
        background()->setVisible(theme->isBackgroundEnabled());
        themeDirtyBits.backgroundEnabledDirty = false;
    }

    if (themeDirtyBits.gridEnabledDirty) {
        bool enabled = theme->isGridEnabled();
        m_segmentLineRepeaterX->setVisible(enabled);
        m_segmentLineRepeaterY->setVisible(enabled);
        m_segmentLineRepeaterZ->setVisible(enabled);

        m_subsegmentLineRepeaterX->setVisible(enabled);
        m_subsegmentLineRepeaterY->setVisible(enabled);
        m_subsegmentLineRepeaterZ->setVisible(enabled);
        themeDirtyBits.gridEnabledDirty = false;
    }

    if (themeDirtyBits.gridLineColorDirty) {
        QColor gridLineColor = theme->gridLineColor();
        changeGridLineColor(m_segmentLineRepeaterX, gridLineColor);
        changeGridLineColor(m_subsegmentLineRepeaterX, gridLineColor);
        changeGridLineColor(m_segmentLineRepeaterY, gridLineColor);
        changeGridLineColor(m_subsegmentLineRepeaterY, gridLineColor);
        changeGridLineColor(m_segmentLineRepeaterZ, gridLineColor);
        changeGridLineColor(m_subsegmentLineRepeaterZ, gridLineColor);
        themeDirtyBits.gridLineColorDirty = false;
    }

    // Other adjustments
    if (themeDirtyBits.windowColorDirty) {
        environment()->setClearColor(theme->windowColor());
        themeDirtyBits.windowColorDirty = false;
    }

    m_controller->updateChangedSeriesList();

    updateGraph();
}

void QQuickDataVisItem::updateGrid()
{
    int gridLineCountX = m_segmentLineRepeaterX->count() / 2;
    int subGridLineCountX = m_subsegmentLineRepeaterX->count() / 2;
    int gridLineCountY = m_segmentLineRepeaterY->count() / 2;
    int subGridLineCountY = m_subsegmentLineRepeaterY->count() / 2;
    int gridLineCountZ = m_segmentLineRepeaterZ->count() / 2;
    int subGridLineCountZ = m_subsegmentLineRepeaterZ->count() / 2;

    auto axisX = static_cast<QValue3DAxis *>(m_controller->axisX());
    auto axisY = static_cast<QValue3DAxis *>(m_controller->axisY());
    auto axisZ = static_cast<QValue3DAxis *>(m_controller->axisZ());

    QVector3D scaleX(m_scaleOffset.x() * m_lineLengthScaleFactor, m_lineWidthScaleFactor, m_lineWidthScaleFactor);
    QVector3D scaleY(m_lineWidthScaleFactor, m_scaleOffset.y() * m_lineLengthScaleFactor, m_lineWidthScaleFactor);
    QVector3D scaleZ(m_lineWidthScaleFactor, m_scaleOffset.z() * m_lineLengthScaleFactor, 1);

    QVector3D lineBackRotationX(0, 0, 0);
    QVector3D lineBackRotationY(0, 0, 0);

    QVector3D lineSideRotationY(0, 90, -90);
    QVector3D lineSideRotationZ(0, 90, 0);

    QVector3D lineFloorRotationX(-90,0,0);
    QVector3D lineFloorRotationZ(-90,0,0);
    float linePosX = 0;
    float linePosY = 0;
    float linePosZ = 0;
    // Wall lines

    // Back

    // X = Column
    linePosY = 0;
    if (!m_flipped.z()) {
        linePosZ = -m_scaleWithBackground.z() + m_gridOffset;
    } else {
        linePosZ = m_scaleWithBackground.z() - m_gridOffset;
        lineBackRotationX = QVector3D(0, 180, 0);
        lineBackRotationY = QVector3D(0, 180, 0);
    }

    for (int i  = 0; i < gridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(m_segmentLineRepeaterX->objectAt(i));
        linePosX = axisX->gridPositionAt(i) * m_scaleOffset.x() + m_translate.x();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationX);
    }

    for (int i = 0; i <subGridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(m_subsegmentLineRepeaterX->objectAt(i));
        linePosX = axisX->subGridPositionAt(i) * m_scaleOffset.x() + m_translate.x();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationX);
    }

    // Y = Row
    linePosX = 0;
    for (int i  = 0; i < gridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(m_segmentLineRepeaterY->objectAt(i));
        linePosY = axisY->gridPositionAt(i) * m_scaleOffset.y() + m_translate.y();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationY);
    }

    for (int i = 0; i <subGridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(m_subsegmentLineRepeaterY->objectAt(i));
        linePosY = axisY->subGridPositionAt(i) * m_scaleOffset.y() + m_translate.y();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineBackRotationY);
    }

    // Side
    // Y = Row
    linePosZ = 0;
    int k = 0;
    if (!m_flipped.x()) {
        linePosX = -m_scaleWithBackground.x() + m_gridOffset;
    } else {
        linePosX = m_scaleWithBackground.x() - m_gridOffset;
        lineSideRotationY = QVector3D(0, -90, 90);
        lineSideRotationZ = QVector3D(0, -90, 0);
    }

    for (int i = gridLineCountY; i < m_segmentLineRepeaterY->count(); i++)
    {
        auto lineNode = static_cast<QQuick3DNode *>(m_segmentLineRepeaterY->objectAt(i));
        linePosY = axisY->gridPositionAt(k) * m_scaleOffset.y() + m_translate.y();
        positionAndScaleLine(lineNode,scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationY);
        k++;
    }

    k = 0;
    for (int i = subGridLineCountY; i < m_subsegmentLineRepeaterY->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(m_subsegmentLineRepeaterY->objectAt(i));
        linePosY = axisY->subGridPositionAt(k) * m_scaleOffset.y() + m_translate.y();
        positionAndScaleLine(lineNode,scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationY);
        k++;
    }

    // Z = Column
    linePosY = 0;
    for (int i = 0; i < gridLineCountZ; i++) {
        auto lineNode = static_cast<QQuick3DNode *>(m_segmentLineRepeaterZ->objectAt(i));
        linePosZ = axisZ->gridPositionAt(i) * m_scaleOffset.z() + m_translate.z();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationZ);
    }

    for (int i = 0; i < subGridLineCountZ; i++) {
        auto lineNode = static_cast<QQuick3DNode *>(m_subsegmentLineRepeaterZ->objectAt(i));
        linePosZ = axisZ->subGridPositionAt(i) * m_scaleOffset.z() + m_translate.z();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineSideRotationZ);
    }

    // Floor lines
    // X = Column
    linePosZ = 0;
    k = 0;
    if (!m_flipped.y()) {
        linePosY = -m_scaleWithBackground.y() + m_gridOffset;
    } else {
        linePosY = m_scaleWithBackground.y() - m_gridOffset;
        lineFloorRotationX = QVector3D(90, 0, 0);
        lineFloorRotationZ = QVector3D(90, 0, 0);
    }
    for (int i  = gridLineCountX; i < m_segmentLineRepeaterX->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(m_segmentLineRepeaterX->objectAt(i));
        linePosZ = axisZ->gridPositionAt(k) * m_scaleOffset.z() + m_translate.z();
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationX);
        k++;
    }

    k = 0;
    for (int i = subGridLineCountX; i < m_subsegmentLineRepeaterX->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(m_subsegmentLineRepeaterX->objectAt(i));
        linePosZ = axisZ->subGridPositionAt(k) * m_scaleOffset.z() + m_translate.z();
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationX);
        k++;
    }

    // Z = Row
    linePosX = 0;
    k = 0;
    for (int i = gridLineCountZ; i < m_segmentLineRepeaterZ->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(m_segmentLineRepeaterZ->objectAt(i));
        linePosZ = axisZ->gridPositionAt(k) * m_scaleOffset.z() + m_translate.z();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationZ);
        k++;
    }

    k = 0;
    for (int i = subGridLineCountZ; i < m_subsegmentLineRepeaterZ->count(); i++) {
        auto lineNode = static_cast<QQuick3DNode *>(m_subsegmentLineRepeaterZ->objectAt(i));
        linePosZ = axisZ->subGridPositionAt(k) * m_scaleOffset.z() + m_translate.z();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setEulerRotation(lineFloorRotationZ);
        k++;
    }
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
            if (backgroundBB() == hit) {
                m_controller->queriedGraphPosition().setZ(backgroundBB()->bounds().minimum().z());
            }
            else {
                m_controller->setQueriedGraphPosition(hit->position());
                break;
            }
        }
    }

    if (!isHitted)
        m_controller->setQueriedGraphPosition(QVector3D(0,0,0));

    emit queriedGraphPositionChanged(m_controller->queriedGraphPosition());
    m_controller->setGraphPositionQueryPending(false);
}

int QQuickDataVisItem::findLabelsMaxWidth(const QStringList &labels)
{
    int labelWidth = 0;
    QFontMetrics labelFM(m_controller->activeTheme()->font());

    for (const auto &label : qAsConst(labels)) {
        auto width = labelFM.horizontalAdvance(label);
        if (labelWidth < width)
            labelWidth = width;
    }
    return labelWidth;
}

void QQuickDataVisItem::updateXTitle(const QVector3D &labelRotation, const QVector3D &labelTrans,
                                     const QQuaternion &totalRotation, float labelsMaxWidth)
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
    if (m_flipped.y()) {
        zRotation = 180.0f;
        if (m_flipped.z()) {
            titleOffset = -titleOffset;
            if (m_flipped.x()) {
                offsetRotation = -offsetRotation;
                extraRotation = -extraRotation;
            } else {
                xRotation = -90.0f - labelRotation.z();
            }
        } else {
            yRotation = 180.0f;
            if (m_flipped.x()) {
                offsetRotation = -offsetRotation;
                xRotation = -90.0f - labelRotation.z();
            } else {
                extraRotation = -extraRotation;
            }
        }
    } else {
        if (m_flipped.z()) {
            titleOffset = -titleOffset;
            if (m_flipped.x()) {
                yRotation = 180.0f;
                offsetRotation = -offsetRotation;
            } else {
                yRotation = 180.0f;
                xRotation = -90.0f - labelRotation.z();
                extraRotation = -extraRotation;
            }
            if (m_flipped.y()) {
                extraRotation = -extraRotation;
                if (m_flipped.x())
                    xRotation = 90.0f + labelRotation.z();
                else
                    xRotation = 90.0f - labelRotation.z();
            }
        } else {
            if (m_flipped.x()) {
                offsetRotation = -offsetRotation;
                xRotation = -90.0f - labelRotation.z();
                extraRotation = -extraRotation;
            }
            if (m_flipped.y()) {
                xRotation = 90.0f + labelRotation.z();
                extraRotation = -extraRotation;
                if (m_flipped.x())
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

    m_titleLabelX->setPosition(labelTrans + titleOffsetVector);
    m_titleLabelX->setRotation(titleRotation);
}

void QQuickDataVisItem::updateYTitle(const QVector3D &sideLabelRotation, const QVector3D &backLabelRotation,
                                     const QVector3D &sideLabelTrans, const QVector3D &backLabelTrans,
                                     const QQuaternion &totalSideRotation, const QQuaternion &totalBackRotation,
                                     float labelsMaxWidth)
{
    float scaledFontSize = (0.05 + m_controller->activeTheme()->font().pointSizeF()) / 500.0f;
    float scaleFactor = scaledFontSize / 115.0f;
    float titleOffset = 2.0f * (m_labelMargin + (labelsMaxWidth * scaleFactor));

    QQuaternion zRightAngleRotation = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, 90.0f);
    float yRotation;
    QVector3D titleTrans;
    QQuaternion totalRotation;
    if (m_flipped.x() == m_flipped.z()) {
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

    m_titleLabelY->setPosition(titleTrans + titleOffsetVector);
    m_titleLabelY->setRotation(titleRotation);
}

void QQuickDataVisItem::updateZTitle(const QVector3D &labelRotation, const QVector3D &labelTrans,
                                     const QQuaternion &totalRotation, float labelsMaxWidth)
{
    float scaledFontSize = (0.05 + m_controller->activeTheme()->font().pointSizeF()) / 500.0f;
    float scaleFactor = scaledFontSize / 115.0f;
    float titleOffset = 2.0f * (m_labelMargin + (labelsMaxWidth * scaleFactor));
    float zRotation = labelRotation.z();
    float yRotation = -90.0f;
    float xRotation = -90.0f;
    float extraRotation = 90.0f;

    if (m_flipped.y()) {

        xRotation = -xRotation;
        if (m_flipped.z()) {
            if (m_flipped.x()) {
                titleOffset = -titleOffset;
                zRotation = -zRotation;
                extraRotation = -extraRotation;
            } else {
                zRotation = -zRotation;
                yRotation = -yRotation;
            }
        } else {
            if (m_flipped.x()) {
                titleOffset = -titleOffset;
            } else {
                extraRotation = -extraRotation;
                yRotation = -yRotation;
            }
        }
    } else {
        if (m_flipped.z()) {
            zRotation = -zRotation;
            if (m_flipped.x()) {
                titleOffset = -titleOffset;
            } else {
                extraRotation = -extraRotation;
                yRotation = -yRotation;
            }
        } else {
            if (m_flipped.x()) {
                titleOffset = -titleOffset;
                extraRotation = -extraRotation;
            } else {
                yRotation = -yRotation;
            }
        }
        if (m_flipped.y()) {
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

    m_titleLabelZ->setPosition(labelTrans + titleOffsetVector);
    m_titleLabelZ->setRotation(titleRotation);
}

void QQuickDataVisItem::updateCamera()
{
    float zoom = 500.0f / m_controller->scene()->activeCamera()->zoomLevel();
    camera()->setZ(zoom);
    cameraTarget()->setPosition(m_controller->scene()->activeCamera()->target());
    cameraTarget()->setEulerRotation(QVector3D(
                                         -m_controller->scene()->activeCamera()->yRotation(),
                                         -m_controller->scene()->activeCamera()->xRotation(),
                                         0));
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
            emit msaaSamplesChanged(samples);
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
            this, &QQuickDataVisItem::synchData,
            Qt::DirectConnection);

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

void QQuickDataVisItem::handleSelectionModeChange(QAbstract3DGraph::SelectionFlags mode)
{
    int intmode = int(mode);
    emit selectionModeChanged(SelectionFlags(intmode));
}

void QQuickDataVisItem::handleShadowQualityChange(QAbstract3DGraph::ShadowQuality quality)
{
    emit shadowQualityChanged(ShadowQuality(quality));
}

void QQuickDataVisItem::handleSelectedElementChange(QAbstract3DGraph::ElementType type)
{
    emit selectedElementChanged(ElementType(type));
}

void QQuickDataVisItem::handleOptimizationHintChange(QAbstract3DGraph::OptimizationHints hints)
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
    m_controller->setOptimizationHints(QAbstract3DGraph::OptimizationHints(intmode));
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

QQuick3DModel *QQuickDataVisItem::background() const
{
    return m_background;
}

void QQuickDataVisItem::setBackground(QQuick3DModel *newBackground)
{
    m_background = newBackground;
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

void QQuickDataVisItem::setUpCamera()
{
    auto useOrtho = m_controller->isOrthoProjection();
    QQuick3DCamera *camera;
    if (!useOrtho) {
        auto persCamera = new QQuick3DPerspectiveCamera(rootNode());
        persCamera->setClipNear(0.001f);
        camera = persCamera;
    }
    else {
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

    camera->setPosition(QVector3D(0,0,5));
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
    light->setEulerRotation(QVector3D(0,0,0));
    m_light = light;
}

QT_END_NAMESPACE

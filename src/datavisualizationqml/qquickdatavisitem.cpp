// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickdatavisitem_p.h"
#include "declarativetheme_p.h"

#include <private/abstract3dcontroller_p.h>

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
    QQuick3DRepeater *repeaterX = createRepeater();
    QQuick3DRepeater *repeaterY = createRepeater();
    QQuick3DRepeater *repeaterZ = createRepeater();

    auto delegateModelX = createRepeaterDelegateComponent(QStringLiteral(":/axis/AxisLabel"));
    auto delegateModelY = createRepeaterDelegateComponent(QStringLiteral(":/axis/AxisLabel"));
    auto delegateModelZ = createRepeaterDelegateComponent(QStringLiteral(":/axis/AxisLabel"));

    repeaterX->setDelegate(delegateModelX);
    repeaterY->setDelegate(delegateModelY);
    repeaterZ->setDelegate(delegateModelZ);

//    m_controller->setRepeaterAxisX(repeaterX);
//    m_controller->setRepeaterAxisY(repeaterY);
//    m_controller->setRepeaterAxisZ(repeaterZ);


    // title labels for axes
    auto titleLabelAxisX = createTitleLabel();
    auto titleLabelAxisY = createTitleLabel();
    auto titleLabelAxisZ = createTitleLabel();

    Q_UNUSED(titleLabelAxisX);
    Q_UNUSED(titleLabelAxisY);
    Q_UNUSED(titleLabelAxisZ);

//    m_controller->setTitleLabelAxisX(titleLabelAxisX);
//    m_controller->setTitleLabelAxisY(titleLabelAxisY);
//    m_controller->setTitleLabelAxisZ(titleLabelAxisZ);

    // Testing gridline

    // X lines
    auto segmentGridLineRepeaterAxisX = createRepeater();

    auto segmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    segmentGridLineRepeaterAxisX->setDelegate(segmentLineDelegate);
//    m_controller->m_segmentGridLineRepeaterAxisX = segmentGridLineRepeaterAxisX;

    auto subsegmentLineRepeaterAxisX = createRepeater();
    auto subsegmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    subsegmentLineRepeaterAxisX->setDelegate(subsegmentLineDelegate);
//    m_controller->m_subSegmentGridLineRepeaterAxisX = subsegmentLineRepeaterAxisX;

    // Y lines
    auto segmentLineRepeaterAxisY = createRepeater();

    segmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    segmentLineRepeaterAxisY->setDelegate(segmentLineDelegate);
//    m_controller->m_segmentGridLineRepeaterAxisY = segmentLineRepeaterAxisY;

    auto subsegmentLineRepeaterAxisY = createRepeater();
    subsegmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    subsegmentLineRepeaterAxisY->setDelegate(subsegmentLineDelegate);
//    m_controller->m_subSegmentGridLineRepeaterAxisY = subsegmentLineRepeaterAxisY;

    // Z lines
    auto segmentLineRepeaterAxisZ = createRepeater();

    segmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    segmentLineRepeaterAxisZ->setDelegate(segmentLineDelegate);
//    m_controller->m_segmentGridLineRepeaterAxisZ = segmentLineRepeaterAxisZ;

    auto subsegmentLineRepeaterAxisZ = createRepeater();
    subsegmentLineDelegate = createRepeaterDelegateComponent(QStringLiteral(":/axis/GridLine"));
    subsegmentLineRepeaterAxisZ->setDelegate(subsegmentLineDelegate);
//    m_controller->m_subSegmentGridLineRepeaterAxisZ = subsegmentLineRepeaterAxisZ;

//    m_controller->qmlReady();
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

void QQuickDataVisItem::synchDataToRenderer()
{
    m_controller->synchDataToRenderer();
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
            this, &QQuickDataVisItem::synchDataToRenderer,
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

void QQuickDataVisItem::itemChange(ItemChange change, const ItemChangeData & value)
{
    QQuick3DViewport::itemChange(change, value);
    updateWindowParameters();
}

void QQuickDataVisItem::updateWindowParameters()
{
    /*
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
    */
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

QAbstract3DInputHandler* QQuickDataVisItem::inputHandler() const
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

//    m_background->setPickable(true);
    m_controller->wheelEvent(event);

//    m_background->setPickable(false);
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
                            &QQuickDataVisItem::synchDataToRenderer);
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
    /*
    if (event->type() == QEvent::MouseButtonPress) {
        auto mouseEvent = static_cast<QMouseEvent *>(event);
        m_controller->mousePressEvent(mouseEvent, mouseEvent->pos());
    }
    static QEventPoint::State lastState = QEventPoint::Unknown;
    qDebug()<<staticMetaObject.className()<<__func__<<event;
    if (event->isPointerEvent()) {
        auto pev = static_cast<QPointerEvent *>(event);
        for (auto &p : pev->points()) {
            qDebug()<<staticMetaObject.className()<<__func__<<"state"<<p.state();
        }
        if (pev->pointingDevice()->pointerType() == QPointingDevice::PointerType::Generic) {
            auto spev = static_cast<QSinglePointEvent *>(event);
            switch (spev->point(0).state()) {
            case QEventPoint::Pressed:
                qDebug()<<staticMetaObject.className()<<__func__<<spev->point(0).state();
                m_controller->mousePressEvent(static_cast<QMouseEvent *>(spev), spev->point(0).position().toPoint());
                break;
            case QEventPoint::Released:
                qDebug()<<staticMetaObject.className()<<__func__<<spev->point(0).state();
                m_controller->mouseReleaseEvent(static_cast<QMouseEvent *>(spev), spev->point(0).position().toPoint());
                break;
            case QEventPoint::Updated:
                qDebug()<<staticMetaObject.className()<<__func__<<spev->point(0).state();
                m_controller->mouseMoveEvent(static_cast<QMouseEvent *>(spev), spev->point(0).position().toPoint());
                break;
            default:
                qDebug()<<staticMetaObject.className()<<__func__<<spev->point(0).state();
            }
        }
    }

    return QQuick3DViewport::event(event);
    */
    return QQuickItem::event(event);
}

void QQuickDataVisItem::setUpCamera()
{
    bool useOrtho = false;
//    auto useOrtho = m_controller->m_useOrthoProjection;
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
//    light->setPosition(QVector3D(0, 0, 0));
    light->setEulerRotation(QVector3D(0,0,0));
    m_light = light;
}

QT_END_NAMESPACE

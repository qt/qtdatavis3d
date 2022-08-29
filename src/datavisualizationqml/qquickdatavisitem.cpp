// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickdatavisitem_p.h"
#include "declarativetheme_p.h"
#include "declarativerendernode_p.h"
#include <QtGui/QGuiApplication>
#if defined(Q_OS_IOS)
#include <QtCore/QTimer>
#endif
#if defined(Q_OS_MACOS)
#include <qpa/qplatformnativeinterface.h>
#endif

#if !defined(Q_OS_MACOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
#define USE_SHARED_CONTEXT
#else
#include "glstatestore_p.h"
#endif

QT_BEGIN_NAMESPACE

static QHash<QQuickDataVisItem *, QQuickWindow *> graphWindowList;

QQuickDataVisItem::QQuickDataVisItem(QQuickItem *parent) :
    QQuickItem(parent),
    AbstractDeclarativeInterface(),
    m_controller(0),
    m_contextWindow(0),
    m_renderMode(RenderIndirect),
    m_samples(0),
    m_windowSamples(0),
    m_initialisedSize(0, 0),
    m_contextOrStateStore(0),
    m_qtContext(0),
    m_mainThread(QThread::currentThread()),
    m_contextThread(0)
{
    m_nodeMutex = QSharedPointer<QMutex>::create();

    connect(this, &QQuickItem::windowChanged, this, &QQuickDataVisItem::handleWindowChanged);

    // Set contents to false in case we are in qml designer to make component look nice
    m_runningInDesigner = QGuiApplication::applicationDisplayName() == QLatin1String("Qml2Puppet");
    setFlag(ItemHasContents, !m_runningInDesigner);

    // Accept touchevents
    setAcceptTouchEvents(true);
}

QQuickDataVisItem::~QQuickDataVisItem()
{
    destroyContext();

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

    QQuickWindow *win = window();

    switch (mode) {
    case RenderDirectToBackground:
        // Intentional flowthrough
    case RenderDirectToBackground_NoClear:
        m_initialisedSize = QSize(0, 0);
        if (previousMode == RenderIndirect) {
            update();
            setFlag(ItemHasContents, false);
            if (win) {
                QObject::connect(win, &QQuickWindow::beforeRenderPassRecording, this,
                                 &QQuickDataVisItem::render, Qt::DirectConnection);
                checkWindowList(win);
                setAntialiasing(m_windowSamples > 0);
                if (m_windowSamples != m_samples)
                    emit msaaSamplesChanged(m_windowSamples);
            }
        }
        break;
    case RenderIndirect:
        m_initialisedSize = QSize(0, 0);
        setFlag(ItemHasContents, !m_runningInDesigner);
        update();
        if (win) {
            QObject::disconnect(win, &QQuickWindow::beforeRenderPassRecording, this,
                                &QQuickDataVisItem::render);
            checkWindowList(win);
        }
        setAntialiasing(m_samples > 0);
        if (m_windowSamples != m_samples)
            emit msaaSamplesChanged(m_samples);
        break;
    }

    updateWindowParameters();

    emit renderingModeChanged(mode);
}

QQuickDataVisItem::RenderingMode QQuickDataVisItem::renderingMode() const
{
    return m_renderMode;
}

QSGNode *QQuickDataVisItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    Q_UNUSED(oldNode);
//    QSize boundingSize = boundingRect().size().toSize() * m_controller->scene()->devicePixelRatio();
//    if (m_runningInDesigner || boundingSize.width() <= 0 || boundingSize.height() <= 0
//            || m_controller.isNull() || !window()) {
//        delete oldNode;
//        return 0;
//    }
//    DeclarativeRenderNode *node = static_cast<DeclarativeRenderNode *>(oldNode);

//    if (!node) {
//        node = new DeclarativeRenderNode(this, m_nodeMutex);
//        node->setController(m_controller.data());
//        node->setQuickWindow(window());
//    }

//    node->setSize(boundingSize);
//    node->setSamples(m_samples);
//    node->update();
//    node->markDirty(QSGNode::DirtyMaterial);

    return nullptr;
}

Declarative3DScene* QQuickDataVisItem::scene() const
{
    return static_cast<Declarative3DScene *>(m_controller->scene());
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
    return reinterpret_cast<QQuickDataVisItem *>(list->data)->m_controller->m_customItems.size();
}

QCustom3DItem *QQuickDataVisItem::atCustomItemFunc(QQmlListProperty<QCustom3DItem> *list,
                                                     qsizetype index)
{
    return reinterpret_cast<QQuickDataVisItem *>(list->data)->m_controller->m_customItems.at(index);
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
    m_controller->m_qml = this;

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
    QObject::connect(m_controller.data(), &Abstract3DController::currentFpsChanged, this,
                     &QQuickDataVisItem::currentFpsChanged);

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

void QQuickDataVisItem::activateOpenGLContext(QQuickWindow *window)
{
    // We can assume we are not in middle of QQuickDataVisItem destructor when we are here,
    // since m_context creation is always done when this function is called from
    // synchDataToRenderer(), which blocks main thread -> no need to mutex.
    if (!m_contextOrStateStore || !m_qtContext || m_contextWindow != window) {
        QOpenGLContext *currentContext = QOpenGLContext::currentContext();

        // Note: Changing graph to different window when using multithreaded renderer will break!

        delete m_contextOrStateStore;

        m_contextThread = QThread::currentThread();
        m_contextWindow = window;
        m_qtContext = currentContext;

#ifdef USE_SHARED_CONTEXT
        m_context = new QOpenGLContext();
        m_context->setFormat(m_qtContext->format());
        m_context->setShareContext(m_qtContext);
        m_context->create();
        m_context->makeCurrent(window);
#else
        // Shared contexts don't work properly in some platforms, so just store the
        // context state on those
        m_stateStore = new GLStateStore(QOpenGLContext::currentContext());
        m_stateStore->storeGLState();
#endif
        m_controller->initializeOpenGL();

        // Make sure context / state store gets deleted.
        QObject::connect(m_contextThread, &QThread::finished, this,
                         &QQuickDataVisItem::destroyContext, Qt::DirectConnection);
    } else {
#ifdef USE_SHARED_CONTEXT
        m_context->makeCurrent(window);
#else
        m_stateStore->storeGLState();
#endif
    }
}

void QQuickDataVisItem::doneOpenGLContext(QQuickWindow *window)
{
#ifdef USE_SHARED_CONTEXT
    m_qtContext->makeCurrent(window);
#else
    Q_UNUSED(window);
    m_stateStore->restoreGLState();
#endif
}

void QQuickDataVisItem::synchDataToRenderer()
{
    QQuickWindow *win = window();
    activateOpenGLContext(win);
    m_controller->synchDataToRenderer();
    doneOpenGLContext(win);
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

void QQuickDataVisItem::handleWindowChanged(QQuickWindow *window)
{
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
        connect(window, &QQuickWindow::beforeRenderPassRecording, this, &QQuickDataVisItem::render,
                Qt::DirectConnection);
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
    QQuickItem::itemChange(change, value);
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

void QQuickDataVisItem::render()
{
    updateWindowParameters();

    // If we're not rendering directly to the background, return
    if (m_renderMode != RenderDirectToBackground && m_renderMode != RenderDirectToBackground_NoClear)
        return;

    // Clear the background once per window as that is not done by default
    QQuickWindow *win = window();
    win->beginExternalCommands();
    activateOpenGLContext(win);

    QOpenGLFunctions *funcs = QOpenGLContext::currentContext()->functions();

    if (isVisible()) {
        funcs->glDepthMask(GL_TRUE);
        funcs->glEnable(GL_DEPTH_TEST);
        funcs->glDepthFunc(GL_LESS);
        funcs->glEnable(GL_CULL_FACE);
        funcs->glCullFace(GL_BACK);
        funcs->glDisable(GL_BLEND);

        m_controller->render();

        funcs->glEnable(GL_BLEND);
    }

    doneOpenGLContext(win);
    win->endExternalCommands();
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
                            &QQuickDataVisItem::synchDataToRenderer);
        QObject::disconnect(oldWindow, &QQuickWindow::beforeRenderPassRecording, this,
                            &QQuickDataVisItem::render);
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
}

bool QQuickDataVisItem::measureFps() const
{
    return m_controller->measureFps();
}

qreal QQuickDataVisItem::currentFps() const
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

void QQuickDataVisItem::windowDestroyed(QObject *obj)
{
    // Remove destroyed window from window lists
    QQuickWindow *win = static_cast<QQuickWindow *>(obj);
    QQuickWindow *oldWindow = graphWindowList.value(this);

    if (win == oldWindow)
        graphWindowList.remove(this);
}

void QQuickDataVisItem::destroyContext()
{
    if (m_contextThread && m_contextThread != m_mainThread) {
        if (m_contextOrStateStore)
            m_contextOrStateStore->deleteLater();
    } else {
        delete m_contextOrStateStore;
    }
    m_contextOrStateStore = 0;

    if (m_contextThread) {
        QObject::disconnect(m_contextThread, &QThread::finished, this,
                            &QQuickDataVisItem::destroyContext);
        m_contextThread = 0;
    }
}

QT_END_NAMESPACE

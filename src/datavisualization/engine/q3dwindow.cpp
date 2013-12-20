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

#include "q3dwindow.h"
#include "q3dwindow_p.h"
#include "abstract3dcontroller_p.h"
#include "qabstract3dinputhandler_p.h"
#include "q3dscene_p.h"

#include <QGuiApplication>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>


QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DWindow
 * \inmodule QtDataVisualization
 * \brief The Q3DWindow class provides a window and render loop.
 * \since Qt Data Visualization 1.0
 *
 * This class creates a QWindow and provides render loop for visualization types inheriting it.
 *
 * You should not need to use this class directly, but one of its subclasses instead.
 *
 * \note Q3DWindow sets window flag \c{Qt::FramelessWindowHint} on by default. If you want to display
 * graph windows as standalone windows with regular window frame, clear this flag after constructing
 * the graph. For example:
 *
 * \code
 *  Q3DBars *graphWindow = new Q3DBars;
 *  graphWindow->setFlags(graphWindow->flags() ^ Qt::FramelessWindowHint);
 * \endcode
 *
 * \sa Q3DBars, Q3DScatter, Q3DSurface, {Qt Data Visualization C++ Classes}
 */

/*!
 * \internal
 */
Q3DWindow::Q3DWindow(Q3DWindowPrivate *d, QWindow *parent)
    : QWindow(parent),
      d_ptr(d)
{
    d_ptr->m_context = new QOpenGLContext(this);

    setFlags(flags() | Qt::FramelessWindowHint);
    setSurfaceType(QWindow::OpenGLSurface);
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(24);
#if !defined(QT_OPENGL_ES_2)
    surfaceFormat.setSamples(8);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
#else
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    setFormat(surfaceFormat);

    create();

    d_ptr->m_context->setFormat(requestedFormat());
    d_ptr->m_context->create();
    d_ptr->m_context->makeCurrent(this);

    initializeOpenGLFunctions();

    const GLubyte *version = glGetString(GL_VERSION);
    qDebug() << "OpenGL version:" << (const char *)version;
    version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    qDebug() << "GLSL version:" << (const char *)version;
#if !defined(QT_OPENGL_ES_2)
    // If we have real OpenGL, GLSL version must be 1.2 or over. Quit if not.
    QStringList splitversionstr = QString::fromLatin1((const char *)version).split(QChar::fromLatin1(' '));
    if (splitversionstr[0].toFloat() < 1.2)
        qFatal("GLSL version must be 1.20 or higher. Try installing latest display drivers.");
#endif

    QObject::connect(d_ptr->m_visualController, &Abstract3DController::activeThemeChanged, this,
                     &Q3DWindow::activeThemeChanged);

    d_ptr->renderLater();
}

/*!
 * Destroys Q3DWindow.
 */
Q3DWindow::~Q3DWindow()
{
}

/*!
 * Adds the given \a inputHandler to the graph. The input handlers added via addInputHandler
 * are not taken in to use directly. Only the ownership of the a\ inputHandler is given to the graph.
 * The \a inputHandler must not be null or already added to another graph.
 *
 * \sa releaseInputHandler(), setActiveInputHandler()
 */
void Q3DWindow::addInputHandler(QAbstract3DInputHandler *inputHandler)
{
    d_ptr->m_visualController->addInputHandler(inputHandler);
}

/*!
 * Releases the ownership of the \a inputHandler back to the caller, if it was added to this graph.
 * If the released \a inputHandler is in use there will be no input handler active after this call.
 *
 * If the default input handler is released and added back later, it behaves as any other input handler would.
 *
 * \sa addInputHandler(), setActiveInputHandler()
 */
void Q3DWindow::releaseInputHandler(QAbstract3DInputHandler *inputHandler)
{
    d_ptr->m_visualController->releaseInputHandler(inputHandler);
}

/*!
 * Sets the active \a inputHandler. Implicitly calls addInputHandler() to transfer ownership of
 * the \a inputHandler to this graph.
 *
 * If the \a inputHandler is null, no input handler will be active after this call.
 *
 * \sa addInputHandler(), releaseInputHandler()
 */
void Q3DWindow::setActiveInputHandler(QAbstract3DInputHandler *inputHandler)
{
    d_ptr->m_visualController->setActiveInputHandler(inputHandler);
}

/*!
 * \return currently active input handler.
 */
QAbstract3DInputHandler *Q3DWindow::activeInputHandler()
{
    return d_ptr->m_visualController->activeInputHandler();
}

/*!
 * Adds the given \a theme to the graph. The themes added via addTheme are not taken in to use
 * directly. Only the ownership of the a\ theme is given to the graph.
 * The \a theme must not be null or already added to another graph.
 *
 * \sa releaseTheme(), setActiveTheme()
 */
void Q3DWindow::addTheme(Q3DTheme *theme)
{
    d_ptr->m_visualController->addTheme(theme);
}

/*!
 * Releases the ownership of the \a theme back to the caller, if it was added to this graph.
 * If the released \a theme is in use, a new default theme will be created and set active.
 *
 * If the default theme is released and added back later, it behaves as any other theme would.
 *
 * \sa addTheme(), setActiveTheme()
 */
void Q3DWindow::releaseTheme(Q3DTheme *theme)
{
    d_ptr->m_visualController->releaseTheme(theme);
}

/*!
 * \property Q3DWindow::activeTheme
 *
 * The active \a theme to be used for the graph. Implicitly calls addTheme() to transfer ownership
 * of the \a theme to this graph.
 * If the \a theme is null, a temporary default theme is created. This temporary theme is destroyed
 * if any theme is explicitly set later.
 * Properties of the \a theme can be modified even after setting it, and the modifications take
 * effect immediately.
 */
void Q3DWindow::setActiveTheme(Q3DTheme *theme)
{
    d_ptr->m_visualController->setActiveTheme(theme);
}


Q3DTheme *Q3DWindow::activeTheme() const
{
    return d_ptr->m_visualController->activeTheme();
}

/*!
 * \return list of all added themes.
 *
 * \sa addTheme()
 */
QList<Q3DTheme *> Q3DWindow::themes() const
{
    return d_ptr->m_visualController->themes();
}

/*!
 * \internal
 */
bool Q3DWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        d_ptr->renderNow();
        return true;
    case QEvent::TouchBegin:
    case QEvent::TouchCancel:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        d_ptr->m_visualController->touchEvent(static_cast<QTouchEvent *>(event));
        return true;
    default:
        return QWindow::event(event);
    }
}

/*!
 * \internal
 */
void Q3DWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    Q3DScene *scene = d_ptr->m_visualController->scene();
    scene->d_ptr->setWindowSize(QSize(width(), height()));
    scene->d_ptr->setViewport(QRect(0, 0, width(), height()));
}

/*!
 * \internal
 */
void Q3DWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        d_ptr->renderNow();
}

Q3DWindowPrivate::Q3DWindowPrivate(Q3DWindow *q)
    : QObject(0),
      q_ptr(q),
      m_updatePending(false),
      m_visualController(0),
      m_devicePixelRatio(1.f)
{
}

Q3DWindowPrivate::~Q3DWindowPrivate()
{
}

void Q3DWindowPrivate::render()
{
    handleDevicePixelRatioChange();
    m_visualController->synchDataToRenderer();
    m_visualController->render();
}

void Q3DWindowPrivate::renderLater()
{
    if (!m_updatePending) {
        m_updatePending = true;
        QCoreApplication::postEvent(q_ptr, new QEvent(QEvent::UpdateRequest));
    }
}

void Q3DWindowPrivate::renderNow()
{
    if (!q_ptr->isExposed())
        return;

    m_updatePending = false;

    m_context->makeCurrent(q_ptr);

    render();

    m_context->swapBuffers(q_ptr);
}

void Q3DWindowPrivate::setVisualController(Abstract3DController *controller)
{
    m_visualController = controller;
}

void Q3DWindowPrivate::handleDevicePixelRatioChange()
{
    if (q_ptr->devicePixelRatio() == m_devicePixelRatio || !m_visualController)
        return;

    m_devicePixelRatio = q_ptr->devicePixelRatio();
    m_visualController->scene()->setDevicePixelRatio(m_devicePixelRatio);
}

QT_DATAVISUALIZATION_END_NAMESPACE

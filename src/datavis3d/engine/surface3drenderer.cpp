/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "surface3dcontroller_p.h"
#include "surface3drenderer_p.h"
#include "camerahelper_p.h"
#include "objecthelper_p.h"
#include "theme_p.h"
#include "utils_p.h"
#include "drawer_p.h"

#include <QMatrix4x4>
#include <QMouseEvent>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

Surface3dRenderer::Surface3dRenderer(QRect rect, Surface3dController *controller)
    : QObject(controller),
      m_controller(controller),
      m_mousePressed(MouseNone),
      m_mousePos(QPoint(0, 0)),
      m_boundingRect(rect.x(), rect.y(), rect.width(), rect.height()),
      m_theme(new Theme()),
      m_labelTransparency(TransparencyFromTheme),
      m_font(QFont(QStringLiteral("Arial"))),
      m_hasNegativeValues(false),
      m_camera(new CameraHelper()),
      m_mainViewPort(rect.x(), rect.y(), rect.width(), rect.height()),
      m_sliceViewPort(rect.x(), rect.y(), rect.width(), rect.height()),
      m_isInitialized(false),
      m_backgroundObj(0),
      m_drawer(new Drawer(*m_theme, m_font, m_labelTransparency))
{
}

Surface3dRenderer::~Surface3dRenderer()
{
    if (m_backgroundObj)
        delete m_backgroundObj;
}

void Surface3dRenderer::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (m_isInitialized)
        return;

    initializeOpenGLFunctions();

    m_drawer->initializeOpenGL();

    // Init selection shader
    //initSelectionShader();

    // Load grid line mesh
    //loadGridLineMesh();

    // Load label mesh
    //loadLabelMesh();

    // Set OpenGL features
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

#if !defined(QT_OPENGL_ES_2)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
#endif

    // Set initial camera position
    // X must be 0 for rotation to work - we can use "setCameraRotation" for setting it later
    m_camera->setDefaultCameraOrientation(QVector3D(0.0f, 0.0f, 6.0f + zComp),
                                          QVector3D(0.0f, 0.0f, zComp),
                                          QVector3D(0.0f, 1.0f, 0.0f));

    // Set view port
    glViewport(m_sliceViewPort.x(), m_sliceViewPort.y(),
               m_sliceViewPort.width(), m_sliceViewPort.height());

    // Set initialized -flag
    m_isInitialized = true;

    // Resize in case we've missed resize events
    // Resize calls initSelectionBuffer and initDepthBuffer, so they don't need to be called here
    resizeNotify();

    // Load background mesh (we need to be initialized first)
    loadBackgroundMesh();
}

void Surface3dRenderer::render(const GLuint defaultFboHandle)
{
    if (!m_isInitialized)
        return;

    if (defaultFboHandle) {
        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    QVector3D clearColor = Utils::vectorFromColor(m_theme->m_windowColor);
    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawScene(defaultFboHandle);
}

void Surface3dRenderer::drawScene(const GLuint defaultFboHandle)
{
    Q_UNUSED(defaultFboHandle)

    // Specify viewport
    glViewport(m_mainViewPort.x(), m_mainViewPort.y(),
               m_mainViewPort.width(), m_mainViewPort.height());

    // Set up projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, (GLfloat)m_mainViewPort.width()
                                 / (GLfloat)m_mainViewPort.height(), 0.1f, 100.0f);

    // Calculate view matrix
    QMatrix4x4 viewMatrix = m_camera->calculateViewMatrix(m_mousePos,
                                                          1.0, //Hard coded
                                                          m_mainViewPort.width(),
                                                          m_mainViewPort.height(),
                                                          m_hasNegativeValues);
}

void Surface3dRenderer::loadBackgroundMesh()
{
    if (!m_isInitialized)
        return;

    if (m_backgroundObj)
        delete m_backgroundObj;
    if (m_hasNegativeValues)
        m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/negativeBackground"));
    else
        m_backgroundObj = new ObjectHelper(QStringLiteral(":/defaultMeshes/background"));
    m_backgroundObj->load();
}


const QSize Surface3dRenderer::size()
{
    return m_boundingRect.size();
}

const QRect Surface3dRenderer::boundingRect()
{
    return m_boundingRect;
}

void Surface3dRenderer::setBoundingRect(const QRect boundingRect)
{
    m_boundingRect = boundingRect;
    resizeNotify();
}

void Surface3dRenderer::setWidth(const int width)
{
    m_boundingRect.setWidth(width);
    resizeNotify();
}

int Surface3dRenderer::width()
{
    return m_boundingRect.width();
}

void Surface3dRenderer::setHeight(const int height)
{
    m_boundingRect.setHeight(height);
    resizeNotify();
}

int Surface3dRenderer::height()
{
    return m_boundingRect.height();
}

void Surface3dRenderer::setX(const int x)
{
    m_boundingRect.setX(x);
}

int Surface3dRenderer::x()
{
    return m_boundingRect.x();
}

void Surface3dRenderer::setY(const int y)
{
    m_boundingRect.setY(y);
}

int Surface3dRenderer::y()
{
    return m_boundingRect.y();
}

#if defined(Q_OS_ANDROID)
void Surface3dRenderer::mouseDoubleClickEvent(QMouseEvent *event)
{

}

void Surface3dRenderer::touchEvent(QTouchEvent *event)
{

}
#endif
void Surface3dRenderer::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    Q_UNUSED(mousePos)
}

void Surface3dRenderer::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    Q_UNUSED(mousePos)
}

void Surface3dRenderer::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    Q_UNUSED(event)
    Q_UNUSED(mousePos)
}

void Surface3dRenderer::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
}

void Surface3dRenderer::resizeNotify()
{
    if (!m_isInitialized)
        return;

    m_sliceViewPort = QRect(0, 0, width(), height());
}

QT_DATAVIS3D_END_NAMESPACE

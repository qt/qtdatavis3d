/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#include "declarativerendernode_p.h"
#include <QtQuick/QQuickWindow>
#include <QtGui/QOpenGLFramebufferObject>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

DeclarativeRenderNode::DeclarativeRenderNode(QQuickWindow *window,
                                             Abstract3DController *controller,
                                             AbstractDeclarative::RenderingMode mode,
                                             QObject *parent)
    : QObject(parent),
      m_fbo(0),
      m_texture(0),
      m_window(window),
      m_controller(controller),
      m_mode(mode)
{
    connect(window, &QQuickWindow::beforeRendering,
            this, &DeclarativeRenderNode::renderFBO,
            Qt::DirectConnection);
}

DeclarativeRenderNode::~DeclarativeRenderNode()
{
    delete m_texture;
    delete m_fbo;
}

void DeclarativeRenderNode::renderFBO()
{
    QSize size = rect().size().toSize();

    // Create FBO
    if (!m_fbo) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Depth);
        m_fbo = new QOpenGLFramebufferObject(size, format);
        m_texture = m_window->createTextureFromId(m_fbo->texture(), size);

        setTexture(m_texture);

        // Flip texture
        QSize ts = m_texture->textureSize();
        QRectF sourceRect(0, 0, ts.width(), ts.height());
        float tmp = sourceRect.top();
        sourceRect.setTop(sourceRect.bottom());
        sourceRect.setBottom(tmp);
        QSGGeometry *geometry = this->geometry();
        QSGGeometry::updateTexturedRectGeometry(geometry, rect(),
                                                m_texture->convertToNormalizedSourceRect(sourceRect));
        markDirty(DirtyMaterial);
    }

    // Call the shared rendering function
    m_fbo->bind();
    glDisable(GL_BLEND);

    m_controller->render(m_fbo->handle());

    glEnable(GL_BLEND);

    m_fbo->release();

    // New view is in the FBO, request repaint of scene graph
}

QT_END_NAMESPACE_DATAVISUALIZATION

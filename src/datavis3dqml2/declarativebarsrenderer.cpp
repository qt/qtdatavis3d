/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include "declarativebarsrenderer_p.h"

#include <QtQuick/QQuickWindow>
#include <QtGui/QOpenGLFramebufferObject>

QT_DATAVIS3D_BEGIN_NAMESPACE

DeclarativeBarsRenderer::DeclarativeBarsRenderer(QQuickWindow *window, Bars3DController *renderer)
    : m_fbo(0),
      m_texture(0),
      m_window(window),
      m_barsRenderer(renderer)
{
    connect(m_window, &QQuickWindow::beforeSynchronizing, this,
            &DeclarativeBarsRenderer::synchDataToRenderer, Qt::DirectConnection);
    connect(m_window, &QQuickWindow::beforeRendering, this,
            &DeclarativeBarsRenderer::renderFBO, Qt::DirectConnection);
    connect(m_barsRenderer, &Abstract3DController::needRender, m_window,
            &QQuickWindow::update);
}

DeclarativeBarsRenderer::~DeclarativeBarsRenderer()
{
    delete m_texture;
    delete m_fbo;
}

void DeclarativeBarsRenderer::synchDataToRenderer()
{
    m_barsRenderer->initializeOpenGL();
    m_barsRenderer->synchDataToRenderer();
}

void DeclarativeBarsRenderer::renderFBO()
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
        // TODO: Can be gotten rid of once support for texture flipping becomes available (in Qt5.2)
        QSize ts = m_texture->textureSize();
        QRectF sourceRect(0, 0, ts.width(), ts.height());
        float tmp = sourceRect.top();
        sourceRect.setTop(sourceRect.bottom());
        sourceRect.setBottom(tmp);
        QSGGeometry *geometry = this->geometry();
        QSGGeometry::updateTexturedRectGeometry(geometry, rect(),
                                                m_texture->convertToNormalizedSourceRect(sourceRect));
        markDirty(DirtyMaterial);
        //qDebug() << "create node" << m_fbo->handle() << m_texture->textureId() << m_fbo->size();
    }

    // Call the shared rendering function
    m_fbo->bind();

    m_barsRenderer->render(m_fbo->handle());

    m_fbo->release();
}

QT_DATAVIS3D_END_NAMESPACE

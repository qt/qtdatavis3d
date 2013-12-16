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

#include "declarativerenderer_p.h"

#include <QtQuick/QQuickWindow>
#include <QtGui/QOpenGLFramebufferObject>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeRenderer::DeclarativeRenderer(QQuickWindow *window, Abstract3DController *controller) :
    m_fbo(0),
    m_texture(0),
    m_window(window),
    m_controller(controller)
{
    connect(m_window, &QQuickWindow::beforeSynchronizing, this,
            &DeclarativeRenderer::synchDataToRenderer, Qt::DirectConnection);
    connect(m_window, &QQuickWindow::beforeRendering, this,
            &DeclarativeRenderer::renderFBO, Qt::DirectConnection);
    connect(m_controller.data(), &Abstract3DController::needRender, m_window,
            &QQuickWindow::update);
}

DeclarativeRenderer::~DeclarativeRenderer()
{
    delete m_texture;
    delete m_fbo;
}

void DeclarativeRenderer::synchDataToRenderer()
{
    if (m_controller) {
        m_controller->initializeOpenGL();
        m_controller->synchDataToRenderer();
    }
}

void DeclarativeRenderer::setDevicePixelRatio(float devicePixelRatio )
{
    m_devicePixelRatio = devicePixelRatio;
}

void DeclarativeRenderer::renderFBO()
{
    if (m_controller) {
        QSize size = rect().size().toSize();
        size.setWidth(size.width() * m_devicePixelRatio);
        size.setHeight(size.height() * m_devicePixelRatio);

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
        }

        // Call the graph rendering function
        m_fbo->bind();

        m_controller->render(m_fbo->handle());

        m_fbo->release();
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE

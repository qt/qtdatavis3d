/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include "declarativerendernode_p.h"
#include "abstractdeclarative_p.h"
#include <QtGui/QOpenGLFramebufferObject>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

DeclarativeRenderNode::DeclarativeRenderNode(AbstractDeclarative *declarative)
    : QSGGeometryNode(),
      m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4),
      m_texture(0),
      m_declarative(declarative),
      m_controller(0),
      m_fbo(0),
      m_multisampledFBO(0),
      m_window(0),
      m_samples(0),
      m_dirtyFBO(false)
{
    setMaterial(&m_material);
    setOpaqueMaterial(&m_materialO);
    setGeometry(&m_geometry);
    setFlag(UsePreprocess);
}

DeclarativeRenderNode::~DeclarativeRenderNode()
{
    delete m_fbo;
    delete m_multisampledFBO;
    delete m_texture;
}

void DeclarativeRenderNode::setSize(const QSize &size)
{
    if (size == m_size)
        return;

    m_size = size;
    m_dirtyFBO = true;
    markDirty(DirtyGeometry);
}

void DeclarativeRenderNode::update()
{
    if (m_dirtyFBO) {
        updateFBO();
        m_dirtyFBO = false;
    }
}

void DeclarativeRenderNode::updateFBO()
{
    m_declarative->activateOpenGLContext(m_window);

    if (m_fbo)
        delete m_fbo;

    m_fbo = new QOpenGLFramebufferObject(m_size);
    m_fbo->setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // Multisampled
    if (m_multisampledFBO) {
        delete m_multisampledFBO;
        m_multisampledFBO = 0;
    }
    if (m_samples > 0) {
        QOpenGLFramebufferObjectFormat multisampledFrambufferFormat;
        multisampledFrambufferFormat.setSamples(m_samples);
        multisampledFrambufferFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

        m_multisampledFBO = new QOpenGLFramebufferObject(m_size, multisampledFrambufferFormat);
    }

    QSGGeometry::updateTexturedRectGeometry(&m_geometry,
                                            QRectF(0, 0,
                                                   m_size.width()
                                                   / m_controller->scene()->devicePixelRatio(),
                                                   m_size.height()
                                                   / m_controller->scene()->devicePixelRatio()),
                                            QRectF(0, 1, 1, -1));

    delete m_texture;
    m_texture = m_window->createTextureFromId(m_fbo->texture(), m_size);
    m_material.setTexture(m_texture);
    m_materialO.setTexture(m_texture);

    m_declarative->doneOpenGLContext(m_window);
}

void DeclarativeRenderNode::setQuickWindow(QQuickWindow *window)
{
    Q_ASSERT(window);

    m_window = window;
}

void DeclarativeRenderNode::setController(Abstract3DController *controller)
{
    m_controller = controller;
}

void DeclarativeRenderNode::setSamples(int samples)
{
    if (m_samples == samples)
        return;

    m_samples = samples;
    m_dirtyFBO = true;
}

void DeclarativeRenderNode::preprocess()
{
    QOpenGLFramebufferObject *targetFBO;
    if (m_samples > 0)
        targetFBO = m_multisampledFBO;
    else
        targetFBO = m_fbo;

    m_declarative->activateOpenGLContext(m_window);

    targetFBO->bind();
    // Render scene here
    m_controller->render(targetFBO->handle());

    targetFBO->release();

    if (m_samples > 0)
        QOpenGLFramebufferObject::blitFramebuffer(m_fbo, m_multisampledFBO);

    m_declarative->doneOpenGLContext(m_window);
}

QT_END_NAMESPACE_DATAVISUALIZATION

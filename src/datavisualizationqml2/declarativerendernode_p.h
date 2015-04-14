/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DECLARATIVERENDERNODE_P_H
#define DECLARATIVERENDERNODE_P_H

#include "datavisualizationglobal_p.h"

#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGTextureMaterial>
#include <QtQuick/QQuickWindow>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Abstract3DController;
class AbstractDeclarative;

class DeclarativeRenderNode : public QSGGeometryNode
{
public:
    DeclarativeRenderNode(AbstractDeclarative *declarative);
    ~DeclarativeRenderNode();

    void setSize(const QSize &size);
    QSize size() const { return m_size; }

    void update();
    void updateFBO();

    void setController(Abstract3DController *controller);
    void setQuickWindow(QQuickWindow *window);
    void setSamples(int samples);

    void preprocess();

private:
    QSGTextureMaterial m_material;
    QSGOpaqueTextureMaterial m_materialO;
    QSGGeometry m_geometry;
    QSGTexture *m_texture;
    QSize m_size;

    AbstractDeclarative *m_declarative;
    Abstract3DController *m_controller;
    QOpenGLFramebufferObject *m_fbo;
    QOpenGLFramebufferObject *m_multisampledFBO;
    QQuickWindow *m_window;
    int m_samples;

    bool m_dirtyFBO;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DECLARATIVERENDERNODE_P_H
#define DECLARATIVERENDERNODE_P_H

#include <QObject>
#include "datavisualizationglobal_p.h"
#include "abstract3dcontroller_p.h"
#include "abstractdeclarative_p.h"
#include <qsgsimpletexturenode.h>

class QOpenGLFramebufferObject;
class QSGTexture;
class QQuickWindow;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class DeclarativeRenderNode : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT
public:
    explicit DeclarativeRenderNode(QQuickWindow *window, Abstract3DController *controller,
                                   AbstractDeclarative::RenderingMode mode, QObject *parent = 0);
    virtual ~DeclarativeRenderNode();

    // Renders view to FBO before render cycle starts.
    void renderFBO();

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture *m_texture;
    QQuickWindow *m_window;
    QPointer<Abstract3DController> m_controller;
    AbstractDeclarative::RenderingMode m_mode;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

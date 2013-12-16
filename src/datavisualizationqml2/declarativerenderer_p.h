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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef ABSTRACTDECLARATIVERENDERER_P_H
#define ABSTRACTDECLARATIVERENDERER_P_H

#include "datavisualizationglobal_p.h"
#include <abstract3dcontroller_p.h>
#include <QSGSimpleTextureNode>
#include <QPointer>

class QOpenGLFramebufferObject;
class QSGTexture;
class QQuickWindow;

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class DeclarativeRenderer : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT
public:
    explicit DeclarativeRenderer(QQuickWindow *window, Abstract3DController *shared);
    virtual ~DeclarativeRenderer();

public slots:
    // Used to synch up data model from controller to renderer while main thread is locked
    void synchDataToRenderer();
    // Renders view to FBO before render cycle starts.
    void renderFBO();
    void setDevicePixelRatio(float devicePixelRatio);

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture *m_texture;
    QQuickWindow *m_window;
    QPointer<Abstract3DController> m_controller;
    float m_devicePixelRatio;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // ABSTRACTDECLARATIVERENDERER_P_H

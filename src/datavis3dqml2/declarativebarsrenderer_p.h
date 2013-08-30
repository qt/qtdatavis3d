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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DECLARATIVEBARSRENDERER_H
#define DECLARATIVEBARSRENDERER_H

#include "datavis3dglobal_p.h"
#include "bars3dcontroller_p.h"
#include <qsgsimpletexturenode.h>

class QOpenGLFramebufferObject;
class QSGTexture;
class QQuickWindow;

QT_DATAVIS3D_BEGIN_NAMESPACE

class DeclarativeBarsRenderer : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    DeclarativeBarsRenderer(QQuickWindow *window, Bars3DController *shared);
    ~DeclarativeBarsRenderer();

public slots:
    // Used to synch up data model from controller to renderer while main thread is locked
    void synchDataToRenderer();
    // Renders view to FBO before render cycle starts.
    void renderFBO();

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture *m_texture;
    QQuickWindow *m_window;
    Bars3DController *m_barsRenderer;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

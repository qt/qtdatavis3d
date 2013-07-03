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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SURFACE3DCONTROLLER_P_H
#define SURFACE3DCONTROLLER_P_H

#include "abstract3dcontroller_p.h"
#include "datavis3dglobal_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class Surface3dRenderer;

class QT_DATAVIS3D_EXPORT Surface3dController : public Abstract3DController
{
    Q_OBJECT

    Surface3dRenderer *m_renderer;
    bool m_isInitialized;

private:
    // Interaction
    MouseState m_mouseState;
    QPoint m_mousePos;
    SelectionMode m_selectionMode;

public:
    explicit Surface3dController(QRect rect);
    ~Surface3dController();

    void initializeOpenGL();
    void render(const GLuint defaultFboHandle = 0);

    QMatrix4x4 calculateViewMatrix(int zoom, int viewPortWidth, int viewPortHeight, bool showUnder = false);

    void setWidth(const int width);
    void setHeight(const int height);

    //TODO: Temp solution
    void setData(QList<qreal> series, int width, int depth);

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    void wheelEvent(QWheelEvent *event);

    // TODO: abstract renderer should have accessor for Drawer instead
    virtual Drawer *drawer();

private:
    QList<qreal> m_series; // TODO: TEMP
    int m_dataWidth;
    int m_dataDepth;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // SURFACE3DCONTROLLER_P_H

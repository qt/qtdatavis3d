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

#ifndef SURFACEOBJECT_P_H
#define SURFACEOBJECT_P_H

#include "datavis3dglobal_p.h"
#include "abstractobjecthelper_p.h"
#include <QOpenGLFunctions>

QT_DATAVIS3D_BEGIN_NAMESPACE

class SurfaceObject : public AbstractObjectHelper
{
public:
    SurfaceObject();
    ~SurfaceObject();

    void setUpData(QList<qreal> series, int columns, int rows, GLfloat yRange, bool changeGeometry);
    void setUpSmoothData(QList<qreal> series, int columns, int rows, GLfloat yRange, bool changeGeometry);
    GLuint gridElementBuf();
    GLuint gridIndexCount();

private:
    QVector3D normal(const QVector3D &a, const QVector3D &b, const QVector3D &c);
    void createBuffers(const QVector<QVector3D> &vertices, const QVector<QVector2D> &uvs,
                       const QVector<QVector3D> &normals, const GLushort *indices,
                       const GLushort *gridIndices, bool changeGeometry);

private:
    QList<qreal> m_series;
    int m_dataWidth;
    int m_dataDepth;
    GLfloat m_yRange;
    GLuint m_gridElementbuffer;
    GLuint m_gridIndexCount;
};

QT_DATAVIS3D_END_NAMESPACE
#endif // SURFACEOBJECT_P_H

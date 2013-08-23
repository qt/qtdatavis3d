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

#ifndef Q3DCAMERA_P_H
#define Q3DCAMERA_P_H

#include "datavis3dglobal_p.h"
#include "q3dbars.h"
#include <QObject>

QT_DATAVIS3D_BEGIN_NAMESPACE

class Q3DCamera;

class Q3DCameraPrivate
{
public:
    Q3DCameraPrivate(Q3DCamera *q);
    ~Q3DCameraPrivate();

public:
    Q3DCamera *q_ptr;
    QVector3D m_target;
    QVector3D m_up;

    QMatrix4x4 m_viewMatrix;

    GLfloat m_xRotation;
    GLfloat m_yRotation;
    int m_zoomLevel;
    QDataVis::CameraPreset m_activePreset;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // Q3DCAMERA_P_H

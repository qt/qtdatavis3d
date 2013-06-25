/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of QtDataVis3D module.
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

#ifndef DATAVIS3DGLOBAL_P_H
#define DATAVIS3DGLOBAL_P_H

#include "qdatavis3dglobal.h"
#include "qdatavis3dnamespace.h"
#include <QOpenGLFunctions>
#include <QVector3D>
#include <QDebug>

//#define ROTATE_ZOOM_SELECTION

// Uncomment this if you want to try hardcoded QML2 -integration h4x0r version of the engine
//#define USE_QML2_VERSION

QT_DATAVIS3D_BEGIN_NAMESPACE

// Constants used in several files
// Compensation for z position; move all objects to positive z, as shader can't handle negative values correctly
const GLfloat zComp = 10.0f;
// Default light position. To have shadows working correctly, light should be as far as camera, or a bit further
// y position is added to the minimum height (or can be thought to be that much above or below the camera)
const QVector3D defaultLightPos = QVector3D(0.0f, 0.5f, zComp);
const GLfloat defaultRatio = 1.0f / 1.6f; // default aspect ratio 16:10

QT_DATAVIS3D_END_NAMESPACE

#endif // DATAVIS3DGLOBAL_P_H

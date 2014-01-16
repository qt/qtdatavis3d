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

#ifndef DATAVISUALIZATIONGLOBAL_P_H
#define DATAVISUALIZATIONGLOBAL_P_H

#include "qdatavisualizationglobal.h"
#include <QOpenGLFunctions>
#include <QVector3D>
#include <QDebug>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

// Constants used in several files
// Distance from camera to origin
static const GLfloat cameraDistance = 6.0f;
// Size of font to be used in label texture rendering. Doesn't affect the actual font size.
static const int textureFontSize = 50;
static const GLfloat defaultRatio = 1.0f / 1.6f; // default aspect ratio 16:10
static const float gridLineOffset = 0.0001f; // Offset for lifting grid lines off background
// Default light position. To have shadows working correctly, light should be as far as camera, or a bit further
// y position is added to the minimum height (or can be thought to be that much above or below the camera)
static const QVector3D defaultLightPos = QVector3D(0.0f, 0.5f, 0.0f);
static const QVector3D zeroVector = QVector3D(0.0f, 0.0f, 0.0f);
static const QVector3D upVector = QVector3D(0.0f, 1.0f, 0.0f);
static const QVector3D cameraDistanceVector = QVector3D(0.0f, 0.0f, cameraDistance);

// Skip color == selection texture's background color
static const QVector3D selectionSkipColor = QVector3D(255.0f, 255.0f, 255.0f);
static const QVector3D invalidColorVector = QVector3D(-1.0f, -1.0f, -1.0f);
static const GLfloat gradientTextureHeight = 1024.0f;
static const GLfloat gradientTextureWidth = 2.0f;

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

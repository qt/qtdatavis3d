// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtGraphs API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef GRAPHSGLOBAL_P_H
#define GRAPHSGLOBAL_P_H

#include "qgraphsglobal.h"
#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLFramebufferObject>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

// Constants used in several files
// Distance from camera to origin
static const float cameraDistance = 6.0f;
// Size of font to be used in label texture rendering. Doesn't affect the actual font size.
static const int textureFontSize = 50;
static const float defaultRatio = 1.0f / 1.6f; // default aspect ratio 16:10
#if !(defined QT_OPENGL_ES)
static const float gridLineOffset = 0.0001f; // Offset for lifting grid lines off background
#else
static const float gridLineOffset = 0.0035f; // Offset for lifting grid lines off background
#endif
// Default light position. To have shadows working correctly, light should be as far as camera, or a bit further
// y position is added to the minimum height (or can be thought to be that much above or below the camera)
static const QVector3D defaultLightPos = QVector3D(0.0f, 0.5f, 0.0f);
static const QVector3D zeroVector = QVector3D(0.0f, 0.0f, 0.0f);
static const QVector3D oneVector = QVector3D(1.0f, 1.0f, 1.0f);
static const QVector3D upVector = QVector3D(0.0f, 1.0f, 0.0f);
static const QVector3D cameraDistanceVector = QVector3D(0.0f, 0.0f, cameraDistance);

// Skip color == selection texture's background color
static const QVector4D selectionSkipColor = QVector4D(255.0f, 255.0f, 255.0f, 255.0f);
static const QVector4D invalidColorVector = QVector4D(-1.0f, -1.0f, -1.0f, -1.0f);
static const float itemAlpha = 0.0f;
static const float customItemAlpha = 252.0f;
static const float labelValueAlpha = 253.0f;
static const float labelRowAlpha = 254.0f;
static const float labelColumnAlpha = 255.0f;
static const float gradientTextureHeight = 1024.0f;
static const float gradientTextureWidth = 2.0f;
static const float uniformTextureHeight = 64.0f;
static const float uniformTextureWidth = 2.0f;
static const float labelMargin = 0.05f;
static const float gridLineWidth = 0.005f;

QT_END_NAMESPACE

#endif

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

#ifndef QVIS3DNAMESPACE_H
#define QVIS3DNAMESPACE_H

#include "qdatavis3dglobal.h"
#include <QOpenGLFunctions>
#include <QVector3D>

QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE

// Constants used in several files
const GLfloat m_pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f;
// Compensation for z position; move all objects to positive z, as shader can't handle negative values correctly
const GLfloat zComp = 10.0f;
// Default light position. To have shadows working correctly, light should be as far as camera, or a bit further
// y position is added to the minimum height (or can be thought to be that much above or below the camera)
const QVector3D defaultLightPos = QVector3D(0.0f, 0.5f, zComp);
const GLfloat defaultRatio = 1.0f / 1.6f; // default aspect ratio 16:10

// Enums used in several files
enum BarStyle {
    Bars = 0,
    Pyramids,
    Cones,
    Cylinders,
    BevelBars,
    Spheres
};

enum CameraPreset {
    PresetFrontLow = 0,
    PresetFront,
    PresetFrontHigh,
    PresetLeftLow,
    PresetLeft,
    PresetLeftHigh,
    PresetRightLow,
    PresetRight,
    PresetRightHigh,
    PresetBehindLow,
    PresetBehind,
    PresetBehindHigh,
    PresetIsometricLeft,
    PresetIsometricLeftHigh,
    PresetIsometricRight,
    PresetIsometricRightHigh,
    PresetDirectlyAbove,
    PresetDirectlyAboveCW45,
    PresetDirectlyAboveCCW45,
    PresetFrontBelow,           // These work only for graphs including negative values.
    PresetLeftBelow,            // They act as Preset...Low for positive-only values.
    PresetRightBelow,
    PresetBehindBelow,
    PresetDirectlyBelow
};

enum ColorTheme {
    ThemeSystem = 0,
    ThemeBlueCerulean,
    ThemeBlueIcy,
    ThemeBlueNcs,
    ThemeBrownSand,
    ThemeDark,
    ThemeHighContrast,
    ThemeLight
};

enum LabelTransparency {
    TransparencyNone = 0,       // Full solid, using colors from theme
    TransparencyFromTheme,      // Use colors and transparencies from theme
    TransparencyNoBackground    // Draw just text on transparent background
};

// TODO: Will this be used in other vis types than q3dbars?
enum SelectionMode {
    ModeNone = 0,
    ModeBar,
    ModeBarAndRow,
    ModeBarAndColumn,
    ModeBarRowAndColumn,
    ModeZoomRow,
    ModeZoomColumn
};

// TODO: Should this be moved to Q3DBarsPrivate? Not for use via API directly?
enum LabelPosition {
    LabelBelow = 0,
    LabelLow,
    LabelMid,
    LabelHigh,
    LabelOver,
    LabelBottom,    // Absolute positions from here onward, used for axes (QDataItem is ignored)
    LabelTop,
    LabelLeft,
    LabelRight
};

enum ShadowQuality {
    ShadowNone = 0,
    ShadowLow = 1,
    ShadowMedium = 3,
    ShadowHigh = 5
};

QTENTERPRISE_DATAVIS3D_END_NAMESPACE

#endif

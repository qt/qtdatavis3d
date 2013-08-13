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

#ifndef QVIS3DNAMESPACE_H
#define QVIS3DNAMESPACE_H

#include <QtDataVis3D/qdatavis3dglobal.h>

namespace QtDataVis3D {

// Enums used in several files
enum BarStyle {
    Bars = 0,
    Pyramids,
    Cones,
    Cylinders,
    BevelBars,
    Spheres,
    Dots
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

// TODO: Will these be used from other vis types than Q3DBars?
enum SelectionMode {
    ModeNone = 0,
    ModeBar,
    ModeBarAndRow,
    ModeBarAndColumn,
    ModeBarRowAndColumn,
    ModeZoomRow,
    ModeZoomColumn
};

enum ShadowQuality {
    ShadowNone = 0,
    ShadowLow = 1,
    ShadowMedium = 3,
    ShadowHigh = 5
};

enum LabelTransparency {
    TransparencyNone = 0,       // Full solid, using colors from theme
    TransparencyFromTheme,      // Use colors and transparencies from theme
    TransparencyNoBackground    // Draw just text on transparent background
};

}

#endif

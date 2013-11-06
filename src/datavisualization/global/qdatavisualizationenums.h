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

#ifndef QDATAVISUALIZATIONENUMS_H
#define QDATAVISUALIZATIONENUMS_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QObject>

// namespace must be declared without using macros for qdoc
namespace QtDataVisualization {

class QT_DATAVISUALIZATION_EXPORT QDataVis : public QObject
{
    Q_OBJECT
    Q_ENUMS(MeshStyle)
    Q_ENUMS(CameraPreset)
    Q_ENUMS(Theme)
    Q_ENUMS(ShadowQuality)
    Q_ENUMS(LabelStyle)
    Q_FLAGS(SelectionFlag SelectionFlags)

public:
    enum InputState {
        InputStateNone = 0,
        InputStateOnScene,
        InputStateOnOverview,
        InputStateOnSlice,
        InputStateRotating,
        InputStateOnPinch
    };

    enum MeshStyle {
        MeshStyleBars = 0,
        MeshStylePyramids,
        MeshStyleCones,
        MeshStyleCylinders,
        MeshStyleBevelBars,
        MeshStyleSpheres,
        MeshStyleDots,
        MeshStylePoints
    };

    enum CameraPreset {
        CameraPresetNone = -1,
        CameraPresetFrontLow = 0,
        CameraPresetFront,
        CameraPresetFrontHigh,
        CameraPresetLeftLow,
        CameraPresetLeft,
        CameraPresetLeftHigh,
        CameraPresetRightLow,
        CameraPresetRight,
        CameraPresetRightHigh,
        CameraPresetBehindLow,
        CameraPresetBehind,
        CameraPresetBehindHigh,
        CameraPresetIsometricLeft,
        CameraPresetIsometricLeftHigh,
        CameraPresetIsometricRight,
        CameraPresetIsometricRightHigh,
        CameraPresetDirectlyAbove,
        CameraPresetDirectlyAboveCW45,
        CameraPresetDirectlyAboveCCW45,
        CameraPresetFrontBelow,
        CameraPresetLeftBelow,
        CameraPresetRightBelow,
        CameraPresetBehindBelow,
        CameraPresetDirectlyBelow
    };

    enum Theme {
        ThemeDefault = -1,
        ThemeQt,
        ThemePrimaryColors,
        ThemeDigia,
        ThemeStoneMoss,
        ThemeArmyBlue,
        ThemeRetro,
        ThemeEbony,
        ThemeIsabelle
    };

    enum SelectionFlag {
        SelectionNone              = 0,
        SelectionItem              = 1,
        SelectionRow               = 2,
        SelectionItemAndRow        = SelectionItem | SelectionRow,
        SelectionColumn            = 4,
        SelectionItemAndColumn     = SelectionItem | SelectionColumn,
        SelectionRowAndColumn      = SelectionRow | SelectionColumn,
        SelectionItemRowAndColumn  = SelectionItem | SelectionRow | SelectionColumn,
        SelectionSlice             = 8
    };
    Q_DECLARE_FLAGS(SelectionFlags, SelectionFlag)

    enum ShadowQuality {
        ShadowQualityNone = 0,
        ShadowQualityLow,
        ShadowQualityMedium,
        ShadowQualityHigh,
        ShadowQualitySoftLow,
        ShadowQualitySoftMedium,
        ShadowQualitySoftHigh
    };

    enum LabelStyle {
        LabelStyleOpaque = 0,
        LabelStyleFromTheme,
        LabelStyleTransparent
    };
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDataVis::SelectionFlags)
}

#endif

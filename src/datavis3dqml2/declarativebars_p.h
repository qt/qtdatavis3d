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

#ifndef DECLARATIVEBARS_P_H
#define DECLARATIVEBARS_P_H

#include "datavis3dglobal_p.h"
#include "bars3dcontroller_p.h"
#include "declarativebars_p.h"
#include "qitemmodelbardatamapping.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>

QT_DATAVIS3D_BEGIN_NAMESPACE

class DeclarativeBars : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *data READ data WRITE setData)
    Q_PROPERTY(QItemModelBarDataMapping *mapping READ mapping WRITE setMapping)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(MeshStyle barType READ barType WRITE setBarType)
    Q_PROPERTY(CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset)
    Q_PROPERTY(ColorTheme theme READ theme WRITE setTheme)
    Q_PROPERTY(QSizeF barThickness READ barThickness WRITE setBarThickness)
    Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing)
    Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative)
    Q_PROPERTY(bool barSmooth READ barSmooth WRITE setBarSmooth)
    Q_PROPERTY(QString meshFileName READ meshFileName WRITE setMeshFileName)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(float fontSize READ fontSize WRITE setFontSize)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_PROPERTY(int rows READ rows WRITE setRows)
    Q_PROPERTY(int columns READ columns WRITE setColumns)
    Q_ENUMS(SelectionMode)
    Q_ENUMS(ShadowQuality)
    Q_ENUMS(LabelTransparency)
    Q_ENUMS(MeshStyle)
    Q_ENUMS(CameraPreset)
    Q_ENUMS(ColorTheme)

public:
    // Duplicated here to be able to use the same enums
    enum SelectionMode {
        ModeNone = 0,
        ModeItem,
        ModeItemAndRow,
        ModeItemAndColumn,
        ModeItemRowAndColumn,
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

    enum MeshStyle {
        Bars = 0,
        Pyramids,
        Cones,
        Cylinders,
        BevelBars,
        Spheres,
        Dots
    };

    enum CameraPreset {
        NoPreset = -1,
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
        ThemeDefault = -1,
        ThemeSystem = 0,
        ThemeBlueCerulean,
        ThemeBlueIcy,
        ThemeBlueNcs,
        ThemeBrownSand,
        ThemeDark,
        ThemeHighContrast,
        ThemeLight
    };

public:
    explicit DeclarativeBars(QQuickItem *parent = 0);
    ~DeclarativeBars();

    void classBegin();
    void componentComplete();

     // how many samples per row and column
    Q_INVOKABLE void setupSampleSpace(int rowCount, int columnCount);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height)
    // color to change from bottom to top
    Q_INVOKABLE void setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                                 bool uniform = true);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) (or (-90...90) if there are negative values) angles and distance in
    // percentage (10...500))
    Q_INVOKABLE void setCameraPosition(qreal horizontal, qreal vertical, int distance);

    // Set segment count and step. Note; segmentCount * step should be the maximum possible value of data
    // set. Minimum is the absolute minimum possible value a bar can have. This is especially
    // important to set if values can be negative.
    Q_INVOKABLE void setSegmentCount(int segmentCount, qreal step, qreal minimum);

    // Add whole data set.
    void setData(QAbstractItemModel *data);
    QAbstractItemModel *data();

    QItemModelBarDataMapping *mapping() const;
    void setMapping(QItemModelBarDataMapping *mapping);

    // Set bar thickness. Y -component sets the thickness of z -direction.
    void setBarThickness(QSizeF thickness);
    QSizeF barThickness();

    // Set spacing between bars. Y -component sets the spacing of z -direction.
    // If spacing is relative, 0.0f means side-to-side and 1.0f = one thickness in between.
    void setBarSpacing(QSizeF spacing);
    QSizeF barSpacing();

    // Set bar spacing relative to thickness or absolute
    void setBarSpacingRelative(bool relative);
    bool isBarSpacingRelative();

    // Bar type
    void setBarType(MeshStyle style);
    MeshStyle barType();

    // Bar smoothing
    void setBarSmooth(bool smooth);
    bool barSmooth();

    // override object type with own mesh
    void setMeshFileName(const QString &objFileName);
    QString meshFileName();

    // Select preset camera placement
    void setCameraPreset(CameraPreset preset);
    CameraPreset cameraPreset();

    // Set theme (object colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    void setTheme(ColorTheme theme);
    ColorTheme theme();

    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode();

    // Font size adjustment
    void setFontSize(float fontsize);
    float fontSize();

    // Set font
    void setFont(const QFont &font);
    QFont font();

    // Label transparency adjustment
    void setLabelTransparency(LabelTransparency transparency);
    LabelTransparency labelTransparency();

    // Enable or disable background grid
    void setGridVisible(bool visible);
    bool isGridVisible();

    // Enable or disable background mesh
    void setBackgroundVisible(bool visible);
    bool isBackgroundVisible();

    // Adjust shadow quality
    void setShadowQuality(ShadowQuality quality);
    ShadowQuality shadowQuality();

    int rows() const;
    void setRows(int rows);

    int columns() const;
    void setColumns(int columns);

public slots:
    // Used to detect when shadow quality changes autonomously due to e.g. resizing.
    void handleShadowQualityUpdate(QtDataVis3D::ShadowQuality quality);

signals:
    // Signals shadow quality changes.
    void shadowQualityChanged(DeclarativeBars::ShadowQuality quality);

protected:
    Bars3dController *m_shared;

    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QSize m_initialisedSize;
    CameraPreset m_cameraPreset;
    ColorTheme m_theme;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

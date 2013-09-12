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

#ifndef DECLARATIVESURFACE_P_H
#define DECLARATIVESURFACE_P_H

#include "datavisualizationglobal_p.h"
#include "surface3dcontroller_p.h"
#include "declarativesurface_p.h"
#include "q3dvalueaxis.h"
#include "qsurfacedataproxy.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>
#include <QQuickWindow>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class DeclarativeSurface : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QSurfaceDataProxy *dataProxy READ dataProxy WRITE setDataProxy)
    Q_PROPERTY(Q3DValueAxis *axisX READ axisX WRITE setAxisX)
    Q_PROPERTY(Q3DValueAxis *axisY READ axisY WRITE setAxisY)
    Q_PROPERTY(Q3DValueAxis *axisZ READ axisZ WRITE setAxisZ)
    Q_PROPERTY(QtDataVisualization::QDataVis::SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(QtDataVisualization::QDataVis::LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(QtDataVisualization::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(QtDataVisualization::QDataVis::CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset)
    Q_PROPERTY(QtDataVisualization::QDataVis::ColorTheme theme READ theme WRITE setTheme)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_PROPERTY(bool smoothSurfaceEnabled READ isSmoothSurfaceEnabled WRITE setSmoothSurfaceEnabled)
    Q_PROPERTY(bool surfaceGridEnabled READ isSurfaceGridEnabled WRITE setSurfaceGridEnabled)
    Q_PROPERTY(QString itemLabelFormat READ itemLabelFormat WRITE setItemLabelFormat)
    Q_ENUMS(QtDataVisualization::QDataVis::SelectionMode)
    Q_ENUMS(QtDataVisualization::QDataVis::ShadowQuality)
    Q_ENUMS(QtDataVisualization::QDataVis::LabelTransparency)
    Q_ENUMS(QtDataVisualization::QDataVis::CameraPreset)
    Q_ENUMS(QtDataVisualization::QDataVis::ColorTheme)

public:
    explicit DeclarativeSurface(QQuickItem *parent = 0);
    ~DeclarativeSurface();

    Q_INVOKABLE void setGradientColorAt(qreal pos, const QColor &color);

    QSurfaceDataProxy *dataProxy() const;
    void setDataProxy(QSurfaceDataProxy *dataProxy);

    Q3DValueAxis *axisX() const;
    void setAxisX(Q3DValueAxis *axis);
    Q3DValueAxis *axisY() const;
    void setAxisY(Q3DValueAxis *axis);
    Q3DValueAxis *axisZ() const;
    void setAxisZ(Q3DValueAxis *axis);

    // Select preset camera placement
    void setCameraPreset(QDataVis::CameraPreset preset);
    QDataVis::CameraPreset cameraPreset();

    // Set theme (object colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    void setTheme(QDataVis::ColorTheme theme);
    QDataVis::ColorTheme theme();

    // Change selection mode
    void setSelectionMode(QDataVis::SelectionMode mode);
    QDataVis::SelectionMode selectionMode();

    // Set font
    void setFont(const QFont &font);
    QFont font();

    // Label transparency adjustment
    void setLabelTransparency(QDataVis::LabelTransparency transparency);
    QDataVis::LabelTransparency labelTransparency();

    // Enable or disable background grid
    void setGridVisible(bool visible);
    bool isGridVisible();

    // Enable or disable background mesh
    void setBackgroundVisible(bool visible);
    bool isBackgroundVisible();

    // Enable or disable the smoothes of the surface
    void setSmoothSurfaceEnabled(bool enabled);
    bool isSmoothSurfaceEnabled() const;

    // Enable or disable the grid on the surface
    void setSurfaceGridEnabled(bool enabled);
    bool isSurfaceGridEnabled() const;

    // Adjust shadow quality
    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality();

    void setItemLabelFormat(const QString &format);
    QString itemLabelFormat();

signals:
    // Signals shadow quality changes.
    void shadowQualityChanged(QDataVis::ShadowQuality quality);

protected:
    Surface3DController *m_shared;

    // Used to detect when shadow quality changes autonomously due to e.g. resizing.
    void handleShadowQualityUpdate(QDataVis::ShadowQuality quality);

    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QSize m_initialisedSize;
    QDataVis::CameraPreset m_cameraPreset;
    QDataVis::ColorTheme m_theme;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

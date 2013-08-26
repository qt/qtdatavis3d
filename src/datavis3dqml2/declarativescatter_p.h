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

#ifndef DECLARATIVESCATTER_P_H
#define DECLARATIVESCATTER_P_H

#include "datavis3dglobal_p.h"
#include "scatter3dcontroller_p.h"
#include "declarativescatter_p.h"
#include "qitemmodelscatterdatamapping.h"
#include "qvalueaxis.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>

QT_DATAVIS3D_BEGIN_NAMESPACE

class DeclarativeScatter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *data READ data WRITE setData)
    Q_PROPERTY(QItemModelScatterDataMapping *mapping READ mapping WRITE setMapping)
    Q_PROPERTY(QValueAxis *axisX READ axisX WRITE setAxisX)
    Q_PROPERTY(QValueAxis *axisY READ axisY WRITE setAxisY)
    Q_PROPERTY(QValueAxis *axisZ READ axisZ WRITE setAxisZ)
    Q_PROPERTY(QtDataVis3D::QDataVis::SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(QtDataVis3D::QDataVis::LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(QtDataVis3D::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(QtDataVis3D::QDataVis::MeshStyle objectType READ objectType WRITE setObjectType)
    Q_PROPERTY(QtDataVis3D::QDataVis::CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset)
    Q_PROPERTY(QtDataVis3D::QDataVis::ColorTheme theme READ theme WRITE setTheme)
    Q_PROPERTY(bool objectSmooth READ objectSmooth WRITE setObjectSmooth)
    Q_PROPERTY(QString meshFileName READ meshFileName WRITE setMeshFileName)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_PROPERTY(QString itemLabelFormat READ itemLabelFormat WRITE setItemLabelFormat)
    Q_ENUMS(QtDataVis3D::QDataVis::SelectionMode)
    Q_ENUMS(QtDataVis3D::QDataVis::ShadowQuality)
    Q_ENUMS(QtDataVis3D::QDataVis::LabelTransparency)
    Q_ENUMS(QtDataVis3D::QDataVis::MeshStyle)
    Q_ENUMS(QtDataVis3D::QDataVis::CameraPreset)
    Q_ENUMS(QtDataVis3D::QDataVis::ColorTheme)

public:
    explicit DeclarativeScatter(QQuickItem *parent = 0);
    ~DeclarativeScatter();

    void classBegin();
    void componentComplete();

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (-90...90) angles and distance in percentage (10...500))
    Q_INVOKABLE void setCameraPosition(qreal horizontal, qreal vertical, int distance);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height)
    // color to change from bottom to top
    Q_INVOKABLE void setObjectColor(const QColor &baseColor, const QColor &heightColor,
                                    const QColor &depthColor, bool uniform = true);

    // Add whole data set.
    void setData(QAbstractItemModel *data);
    QAbstractItemModel *data();

    QItemModelScatterDataMapping *mapping() const;
    void setMapping(QItemModelScatterDataMapping *mapping);

    QValueAxis *axisX() const;
    void setAxisX(QValueAxis *axis);
    QValueAxis *axisY() const;
    void setAxisY(QValueAxis *axis);
    QValueAxis *axisZ() const;
    void setAxisZ(QValueAxis *axis);

    // Object type
    void setObjectType(QDataVis::MeshStyle style);
    QDataVis::MeshStyle objectType();

    // Object smoothing
    void setObjectSmooth(bool smooth);
    bool objectSmooth();

    // override object type with own mesh
    void setMeshFileName(const QString &objFileName);
    QString meshFileName();

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

    void setItemLabelFormat(const QString &format);
    QString itemLabelFormat();

    // Adjust shadow quality
    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality();

public slots:
    // Used to detect when shadow quality changes autonomously due to e.g. resizing.
    void handleShadowQualityUpdate(QDataVis::ShadowQuality quality);

signals:
    // Signals shadow quality changes.
    void shadowQualityChanged(QDataVis::ShadowQuality quality);

protected:
    Scatter3DController *m_shared;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QSize m_initialisedSize;
    QDataVis::CameraPreset m_cameraPreset;
    QDataVis::ColorTheme m_theme;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

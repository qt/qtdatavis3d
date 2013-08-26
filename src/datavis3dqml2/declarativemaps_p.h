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
//
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DECLARATIVEMAPS_P_H
#define DECLARATIVEMAPS_P_H

#include "datavis3dglobal_p.h"
#include "maps3dcontroller_p.h"
#include "declarativemaps_p.h"
#include "qitemmodelmapdatamapping.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>

QT_DATAVIS3D_BEGIN_NAMESPACE

class DeclarativeMaps : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *data READ data WRITE setData)
    Q_PROPERTY(QtDataVis3D::QDataVis::SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(QtDataVis3D::QDataVis::LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(QtDataVis3D::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(float fontSize READ fontSize WRITE setFontSize)
    Q_PROPERTY(QItemModelMapDataMapping *mapping READ mapping WRITE setMapping)
    Q_ENUMS(QtDataVis3D::QDataVis::SelectionMode)
    Q_ENUMS(QtDataVis3D::QDataVis::ShadowQuality)
    Q_ENUMS(QtDataVis3D::QDataVis::LabelTransparency)

public:
    explicit DeclarativeMaps(QQuickItem *parent = 0);
    ~DeclarativeMaps();

    void classBegin();
    void componentComplete();

    void setData(QAbstractItemModel *data);
    QAbstractItemModel *data();

    // bar specifications; base thickness in x, y and z, enum to indicate which direction is increased with value
    // TODO: Start using thickness also in adjustment direction; use it as a relative value.
    // For example, in AdjustAll mode setting thickness to (0.1f, 1.0f, 0.5f) would apply value to
    // x at 10%, y at 100% and z at 50%. If a dimension is not included, given thickness states its absolute value.
    Q_INVOKABLE void setBarSpecs(const QVector3D &thickness = QVector3D(1.0f, 1.0f, 1.0f),
                                 Q3DMaps::AdjustmentDirection direction = Q3DMaps::AdjustHeight);

    // bar type; bars (=cubes), pyramids, cones, cylinders, balls, etc.
    Q_INVOKABLE void setBarType(QDataVis::MeshStyle style, bool smooth = false);

    // override bar type with own mesh
    Q_INVOKABLE void setMeshFileName(const QString &objFileName);

    // Select preset camera placement
    Q_INVOKABLE void setCameraPreset(QDataVis::CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (180...180) and
    // vertical (0...90) angles and distance in percentage (10...500))
    Q_INVOKABLE void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text colors are affected)
    Q_INVOKABLE void setTheme(QDataVis::ColorTheme theme);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height) color to change from bottom to top
    Q_INVOKABLE void setBarColor(const QColor &baseColor, const QColor &heightColor,
                                 bool uniform = true);

    // Set area specs
    Q_INVOKABLE void setAreaSpecs(const QRect &areaRect, const QImage &image);

    // Set area image
    Q_INVOKABLE void setImage(const QImage &image);
    Q_INVOKABLE void setImage(const QString &imageUrl);

    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(QDataVis::SelectionMode mode);
    QDataVis::SelectionMode selectionMode();

    // Font size adjustment
    void setFontSize(float fontsize);
    float fontSize();

    // Set font
    void setFont(const QFont &font);
    QFont font();

    // Label transparency adjustment
    void setLabelTransparency(QDataVis::LabelTransparency transparency);
    QDataVis::LabelTransparency labelTransparency();

    // Adjust shadow quality
    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality();

    QItemModelMapDataMapping *mapping() const;
    void setMapping(QItemModelMapDataMapping *mapping);

protected:
    Maps3DController *m_shared;

    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QSize m_initializedSize;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

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

#ifndef Q3DBARS_H
#define Q3DBARS_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QtDataVisualization/q3dwindow.h>
#include <QFont>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DBarsPrivate;
class Q3DAbstractAxis;
class Q3DCategoryAxis;
class Q3DValueAxis;
class QBarDataProxy;

class QT_DATAVISUALIZATION_EXPORT Q3DBars : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::QDataVis::SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(QtDataVisualization::QDataVis::LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(QtDataVisualization::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset)
    Q_PROPERTY(qreal barThickness READ barThickness WRITE setBarThickness)
    Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing)
    Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative)
    Q_PROPERTY(QString meshFileName READ meshFileName WRITE setMeshFileName)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_PROPERTY(QPoint selectedBarPos READ selectedBarPos WRITE setSelectedBarPos NOTIFY selectedBarPosChanged)
    Q_ENUMS(QtDataVisualization::QDataVis::SelectionMode)
    Q_ENUMS(QtDataVisualization::QDataVis::ShadowQuality)
    Q_ENUMS(QtDataVisualization::QDataVis::LabelTransparency)
    Q_ENUMS(QtDataVisualization::QDataVis::CameraPreset)

public:
    explicit Q3DBars();
    ~Q3DBars();

    void setBarType(QDataVis::MeshStyle style, bool smooth = false);

    void setCameraPreset(QDataVis::CameraPreset preset);
    QDataVis::CameraPreset cameraPreset() const;

    void setCameraPosition(qreal horizontal, qreal vertical, int distance = 100);

    void setTheme(QDataVis::ColorTheme theme);

    void setBarThickness(qreal thicknessRatio);
    qreal barThickness();

    void setBarSpacing(QSizeF spacing);
    QSizeF barSpacing();

    void setBarSpacingRelative(bool relative);
    bool isBarSpacingRelative();

    void setBarColor(const QColor &baseColor, bool uniform = true);
    QColor barColor() const;

    void setMeshFileName(const QString &objFileName);
    QString meshFileName() const;

    void setSelectionMode(QDataVis::SelectionMode mode);
    QDataVis::SelectionMode selectionMode() const;

    void setFont(const QFont &font);
    QFont font() const;

    void setLabelTransparency(QDataVis::LabelTransparency transparency);
    QDataVis::LabelTransparency labelTransparency() const;

    void setGridVisible(bool visible);
    bool isGridVisible() const;

    void setWidth(const int width);
    void setHeight(const int height);

    void setBackgroundVisible(bool visible);
    bool isBackgroundVisible() const;

    void setSelectedBarPos(const QPoint &position);
    QPoint selectedBarPos() const;

    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality() const;

    void setRowAxis(Q3DCategoryAxis *axis);
    Q3DCategoryAxis *rowAxis() const;
    void setColumnAxis(Q3DCategoryAxis *axis);
    Q3DCategoryAxis *columnAxis() const;
    void setValueAxis(Q3DValueAxis *axis);
    Q3DValueAxis *valueAxis() const;
    void addAxis(Q3DAbstractAxis *axis);
    void releaseAxis(Q3DAbstractAxis *axis);
    QList<Q3DAbstractAxis *> axes() const;

    void setActiveDataProxy(QBarDataProxy *proxy);
    QBarDataProxy *activeDataProxy() const;
    void addDataProxy(QBarDataProxy *proxy);
    void releaseDataProxy(QBarDataProxy *proxy);
    QList<QBarDataProxy *> dataProxies() const;

signals:
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void selectedBarPosChanged(QPoint position);

protected:

    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QScopedPointer<Q3DBarsPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DBars)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

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

#ifndef Q3DSURFACE_H
#define Q3DSURFACE_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QtDataVisualization/q3dwindow.h>
#include <QtDataVisualization/q3dscene.h>
#include <QFont>
#include <QLinearGradient>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DSurfacePrivate;
class Q3DValueAxis;
class QSurfaceDataProxy;

class QT_DATAVISUALIZATION_EXPORT Q3DSurface : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::QDataVis::SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::LabelStyle labelStyle READ labelStyle WRITE setLabelStyle NOTIFY labelStyleChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::Theme theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(bool surfaceVisible READ isSurfaceVisible WRITE setSurfaceVisible NOTIFY surfaceVisibleChanged)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible NOTIFY backgroundVisibleChanged)
    Q_PROPERTY(bool smoothSurfaceEnabled READ isSmoothSurfaceEnabled WRITE setSmoothSurfaceEnabled NOTIFY smoothSurfaceEnabledChanged)
    Q_PROPERTY(bool surfaceGridEnabled READ isSurfaceGridEnabled WRITE setSurfaceGridEnabled NOTIFY surfaceGridEnabledChanged)
    Q_PROPERTY(QLinearGradient gradient READ gradient WRITE setGradient)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(Q3DScene* scene READ scene)
    Q_PROPERTY(QPoint selectedPoint READ selectedPoint WRITE setSelectedPoint NOTIFY selectedPointChanged)


public:
    explicit Q3DSurface();
    ~Q3DSurface();

    void setSurfaceVisible(bool visible);
    bool isSurfaceVisible() const;

    void setGridVisible(bool visible);
    bool isGridVisible() const;

    void setBackgroundVisible(bool visible);
    bool isBackgroundVisible() const;

    void setTheme(QDataVis::Theme theme);
    QDataVis::Theme theme() const;

    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality() const;

    void setSmoothSurfaceEnabled(bool enabled);
    bool isSmoothSurfaceEnabled() const;

    void setSelectionMode(QDataVis::SelectionFlags mode);
    QDataVis::SelectionFlags selectionMode() const;
    void setSurfaceGridEnabled(bool enabled);
    bool isSurfaceGridEnabled() const;

    void setGradient(const QLinearGradient &gradient);
    QLinearGradient gradient() const;

    void setGradientColorAt(qreal pos, const QColor &color);

    // Axes
    void setAxisX(Q3DValueAxis *axis);
    Q3DValueAxis *axisX() const;
    void setAxisY(Q3DValueAxis *axis);
    Q3DValueAxis *axisY() const;
    void setAxisZ(Q3DValueAxis *axis);
    Q3DValueAxis *axisZ() const;
    void addAxis(Q3DValueAxis *axis);
    void releaseAxis(Q3DValueAxis *axis);
    QList<Q3DValueAxis *> axes() const;

    void setActiveDataProxy(QSurfaceDataProxy *proxy);
    QSurfaceDataProxy *activeDataProxy() const;
    void addDataProxy(QSurfaceDataProxy *proxy);
    void releaseDataProxy(QSurfaceDataProxy *proxy);
    QList<QSurfaceDataProxy *> dataProxies() const;

    void setFont(const QFont &font);
    QFont font() const;

    Q3DScene *scene() const;

    void setSelectedPoint(const QPoint &position);
    QPoint selectedPoint() const;

    void setLabelStyle(QDataVis::LabelStyle style);
    QDataVis::LabelStyle labelStyle() const;

signals:
    void selectionModeChanged(QDataVis::SelectionFlags mode);
    void labelStyleChanged(QDataVis::LabelStyle style);
    void themeChanged(QDataVis::Theme theme);
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void surfaceVisibleChanged(bool visible);
    void gridVisibleChanged(bool visible);
    void backgroundVisibleChanged(bool visible);
    void smoothSurfaceEnabledChanged(bool enabled);
    void surfaceGridEnabledChanged(bool visible);
    void fontChanged(QFont font);
    void selectedPointChanged(QPoint position);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QScopedPointer<Q3DSurfacePrivate> d_ptr;
    Q_DISABLE_COPY(Q3DSurface)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // Q3DSURFACE_H

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

#ifndef Q3DSCATTER_H
#define Q3DSCATTER_H

#include <QtDataVis3D/qdatavis3denums.h>
#include <QtDataVis3D/q3dwindow.h>
#include <QFont>

QT_DATAVIS3D_BEGIN_NAMESPACE

class Q3DScatterPrivate;
class LabelItem;
class Q3DValueAxis;
class Q3DCategoryAxis;
class QScatterDataProxy;

class QT_DATAVIS3D_EXPORT Q3DScatter : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QtDataVis3D::QDataVis::SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(QtDataVis3D::QDataVis::LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(QtDataVis3D::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_PROPERTY(int selectedItemIndex READ selectedItemIndex WRITE setSelectedItemIndex NOTIFY selectedItemIndexChanged)
    Q_ENUMS(QtDataVis3D::QDataVis::SelectionMode)
    Q_ENUMS(QtDataVis3D::QDataVis::ShadowQuality)
    Q_ENUMS(QtDataVis3D::QDataVis::LabelTransparency)

public:
    explicit Q3DScatter();
    ~Q3DScatter();

    void setObjectType(QDataVis::MeshStyle style, bool smooth = false);

    void setCameraPreset(QDataVis::CameraPreset preset);

    void setCameraPosition(qreal horizontal, qreal vertical, int distance = 100);

    void setTheme(QDataVis::ColorTheme theme);

    void setObjectColor(const QColor &baseColor, const QColor &heightColor,
                        const QColor &depthColor, bool uniform = true);

    void setMeshFileName(const QString &objFileName);

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

    void setSelectedItemIndex(int index);
    int selectedItemIndex() const;

    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality() const;

    void setAxisX(Q3DValueAxis *axis);
    Q3DValueAxis *axisX() const;
    void setAxisY(Q3DValueAxis *axis);
    Q3DValueAxis *axisY() const;
    void setAxisZ(Q3DValueAxis *axis);
    Q3DValueAxis *axisZ() const;
    void addAxis(Q3DValueAxis *axis);
    void releaseAxis(Q3DValueAxis *axis);
    QList<Q3DValueAxis *> axes() const;

    void setActiveDataProxy(QScatterDataProxy *proxy);
    QScatterDataProxy *activeDataProxy() const;
    void addDataProxy(QScatterDataProxy *proxy);
    void releaseDataProxy(QScatterDataProxy *proxy);
    QList<QScatterDataProxy *> dataProxies() const;

signals:
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void selectedItemIndexChanged(int index);

protected:
    void render();

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QScopedPointer<Q3DScatterPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DScatter)
};

QT_DATAVIS3D_END_NAMESPACE

#endif

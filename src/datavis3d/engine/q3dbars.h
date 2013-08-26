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

#ifndef Q3DBARS_H
#define Q3DBARS_H

#include <QtDataVis3D/qdatavis3denums.h>
#include <QtDataVis3D/q3dwindow.h>
#include <QFont>

QT_DATAVIS3D_BEGIN_NAMESPACE

class Q3DBarsPrivate;
class QCategoryAxis;
class QValueAxis;
class QBarDataProxy;

class QT_DATAVIS3D_EXPORT Q3DBars : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QtDataVis3D::QDataVis::SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(QtDataVis3D::QDataVis::LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(QtDataVis3D::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_PROPERTY(QPoint selectedBarPos READ selectedBarPos WRITE setSelectedBarPos NOTIFY selectedBarPosChanged)
    Q_ENUMS(QtDataVis3D::QDataVis::SelectionMode)
    Q_ENUMS(QtDataVis3D::QDataVis::ShadowQuality)
    Q_ENUMS(QtDataVis3D::QDataVis::LabelTransparency)

public:
    explicit Q3DBars();
    ~Q3DBars();

    void setBarSpecs(qreal thicknessRatio = 1.0,
                     const QSizeF &spacing = QSizeF(1.0, 1.0),
                     bool relative = true);

    void setBarType(QDataVis::MeshStyle style, bool smooth = false);

    // TODO: This defines the data window, needs additional parameters startRow, startColumn
    void setDataWindow(int samplesRow, int samplesColumn);
    QSize dataWindow() const; // TODO: Return QRect once data window properly implemented?

    void setCameraPreset(QDataVis::CameraPreset preset);

    void setCameraPosition(qreal horizontal, qreal vertical, int distance = 100);

    void setTheme(QDataVis::ColorTheme theme);

    void setBarColor(const QColor &baseColor, const QColor &heightColor, const QColor &depthColor,
                     bool uniform = true);

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

    void setSelectedBarPos(const QPoint &position);
    QPoint selectedBarPos() const;

    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality() const;

    QCategoryAxis *rowAxis() const;
    QCategoryAxis *columnAxis() const;
    void setValueAxis(QValueAxis *axis);
    QValueAxis *valueAxis() const;

    void setDataProxy(QBarDataProxy *proxy);
    QBarDataProxy *dataProxy();

signals:
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void selectedBarPosChanged(QPoint position);

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
    QScopedPointer<Q3DBarsPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DBars)
};

QT_DATAVIS3D_END_NAMESPACE

#endif

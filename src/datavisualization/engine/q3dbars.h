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
#include <QtDataVisualization/q3dtheme.h>
#include <QFont>
#include <QLinearGradient>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DBarsPrivate;
class Q3DAbstractAxis;
class Q3DCategoryAxis;
class Q3DValueAxis;
class Q3DScene;
class QBar3DSeries;

class QT_DATAVISUALIZATION_EXPORT Q3DBars : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::QDataVis::SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(float barThickness READ barThickness WRITE setBarThickness NOTIFY barThicknessChanged)
    Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing NOTIFY barSpacingChanged)
    Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative NOTIFY barSpacingRelativeChanged)
    Q_PROPERTY(Q3DTheme* theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(Q3DScene* scene READ scene)

public:
    explicit Q3DBars();
    ~Q3DBars();

    void addSeries(QBar3DSeries *series);
    void removeSeries(QBar3DSeries *series);
    QList<QBar3DSeries *> seriesList();

    void setTheme(Q3DTheme *theme);
    Q3DTheme *theme() const;

    void setBarThickness(float thicknessRatio);
    float barThickness();

    void setBarSpacing(QSizeF spacing);
    QSizeF barSpacing();

    void setBarSpacingRelative(bool relative);
    bool isBarSpacingRelative();

    void setSelectionMode(QDataVis::SelectionFlags mode);
    QDataVis::SelectionFlags selectionMode() const;

    Q3DScene *scene() const;

    void setWidth(const int width);
    void setHeight(const int height);

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

signals:
    void selectionModeChanged(QDataVis::SelectionFlags mode);
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void barThicknessChanged(float thicknessRatio);
    void barSpacingChanged(QSizeF spacing);
    void barSpacingRelativeChanged(bool relative);
    void themeChanged(Q3DTheme *theme);

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

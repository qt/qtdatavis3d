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
#include <QLinearGradient>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DBarsPrivate;
class Q3DAbstractAxis;
class Q3DCategoryAxis;
class Q3DValueAxis;
class QBarDataProxy;
class Q3DScene;

class QT_DATAVISUALIZATION_EXPORT Q3DBars : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::QDataVis::SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::LabelStyle labelStyle READ labelStyle WRITE setLabelStyle NOTIFY labelStyleChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(qreal barThickness READ barThickness WRITE setBarThickness NOTIFY barThicknessChanged)
    Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing NOTIFY barSpacingChanged)
    Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative NOTIFY barSpacingRelativeChanged)
    Q_PROPERTY(QString meshFileName READ meshFileName WRITE setMeshFileName NOTIFY meshFileNameChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::Theme theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible NOTIFY backgroundVisibleChanged)
    Q_PROPERTY(QPoint selectedBar READ selectedBar WRITE setSelectedBar NOTIFY selectedBarChanged)
    Q_PROPERTY(Q3DScene* scene READ scene)
    Q_PROPERTY(QtDataVisualization::QDataVis::ColorStyle colorStyle READ colorStyle WRITE setColorStyle NOTIFY colorStyleChanged)
    Q_PROPERTY(QColor barColor READ barColor WRITE setBarColor NOTIFY barColorChanged)
    Q_PROPERTY(QLinearGradient barGradient READ barGradient WRITE setBarGradient NOTIFY barGradientChanged)
    Q_PROPERTY(QColor singleHighlightColor READ singleHighlightColor WRITE setSingleHighlightColor NOTIFY singleHighlightColorChanged)
    Q_PROPERTY(QLinearGradient singleHighlightGradient READ singleHighlightGradient WRITE setSingleHighlightGradient NOTIFY singleHighlightGradientChanged)
    Q_PROPERTY(QColor multiHighlightColor READ multiHighlightColor WRITE setMultiHighlightColor NOTIFY multiHighlightColorChanged)
    Q_PROPERTY(QLinearGradient multiHighlightGradient READ multiHighlightGradient WRITE setMultiHighlightGradient NOTIFY multiHighlightGradientChanged)

public:
    explicit Q3DBars();
    ~Q3DBars();

    // TODO: Move to dataset object once that is done QTRD-2121
    void setBarType(QDataVis::MeshStyle style, bool smooth = false);

    void setTheme(QDataVis::Theme theme);
    QDataVis::Theme theme() const;

    void setBarThickness(qreal thicknessRatio);
    qreal barThickness();

    void setBarSpacing(QSizeF spacing);
    QSizeF barSpacing();

    void setBarSpacingRelative(bool relative);
    bool isBarSpacingRelative();

    // TODO: Move to dataset object once that is done QTRD-2121
    void setMeshFileName(const QString &objFileName);
    QString meshFileName() const;

    void setSelectionMode(QDataVis::SelectionFlags mode);
    QDataVis::SelectionFlags selectionMode() const;

    void setFont(const QFont &font);
    QFont font() const;

    Q3DScene *scene() const;

    void setLabelStyle(QDataVis::LabelStyle style);
    QDataVis::LabelStyle labelStyle() const;

    void setGridVisible(bool visible);
    bool isGridVisible() const;

    void setWidth(const int width);
    void setHeight(const int height);

    void setBackgroundVisible(bool visible);
    bool isBackgroundVisible() const;

    void setSelectedBar(const QPoint &position);
    QPoint selectedBar() const;

    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality() const;

    // TODO: Move to dataset object once that is done QTRD-2121
    void setColorStyle(QDataVis::ColorStyle style);
    QDataVis::ColorStyle colorStyle() const;
    void setBarColor(const QColor &color);
    QColor barColor() const;
    void setBarGradient(const QLinearGradient &gradient);
    QLinearGradient barGradient() const;
    void setSingleHighlightColor(const QColor &color);
    QColor singleHighlightColor() const;
    void setSingleHighlightGradient(const QLinearGradient &gradient);
    QLinearGradient singleHighlightGradient() const;
    void setMultiHighlightColor(const QColor &color);
    QColor multiHighlightColor() const;
    void setMultiHighlightGradient(const QLinearGradient &gradient);
    QLinearGradient multiHighlightGradient() const;

    void setRowAxis(Q3DCategoryAxis *axis);
    Q3DCategoryAxis *rowAxis() const;
    void setColumnAxis(Q3DCategoryAxis *axis);
    Q3DCategoryAxis *columnAxis() const;
    void setValueAxis(Q3DValueAxis *axis);
    Q3DValueAxis *valueAxis() const;
    void addAxis(Q3DAbstractAxis *axis);
    void releaseAxis(Q3DAbstractAxis *axis);
    QList<Q3DAbstractAxis *> axes() const;

    // TODO: Move to dataset object once that is done QTRD-2121
    void setActiveDataProxy(QBarDataProxy *proxy);
    QBarDataProxy *activeDataProxy() const;
    void addDataProxy(QBarDataProxy *proxy);
    void releaseDataProxy(QBarDataProxy *proxy);
    QList<QBarDataProxy *> dataProxies() const;

signals:
    void selectionModeChanged(QDataVis::SelectionFlags mode);
    void labelStyleChanged(QDataVis::LabelStyle style);
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void barThicknessChanged(qreal thicknessRatio);
    void barSpacingChanged(QSizeF spacing);
    void barSpacingRelativeChanged(bool relative);
    void meshFileNameChanged(QString filename);
    void fontChanged(QFont font);
    void themeChanged(QDataVis::Theme theme);
    void gridVisibleChanged(bool visible);
    void backgroundVisibleChanged(bool visible);
    void selectedBarChanged(QPoint position);
    void colorStyleChanged(QDataVis::ColorStyle style);
    void barColorChanged(QColor color);
    void barGradientChanged(QLinearGradient gradient);
    void singleHighlightColorChanged(QColor color);
    void singleHighlightGradientChanged(QLinearGradient gradient);
    void multiHighlightColorChanged(QColor color);
    void multiHighlightGradientChanged(QLinearGradient gradient);

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

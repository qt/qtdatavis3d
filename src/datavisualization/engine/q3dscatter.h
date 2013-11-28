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

#ifndef Q3DSCATTER_H
#define Q3DSCATTER_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QtDataVisualization/q3dwindow.h>
#include <QtDataVisualization/q3dscene.h>
#include <QFont>
#include <QLinearGradient>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DScatterPrivate;
class LabelItem;
class Q3DValueAxis;
class Q3DCategoryAxis;
class QScatter3DSeries;
class Q3DTheme;

class QT_DATAVISUALIZATION_EXPORT Q3DScatter : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::QDataVis::SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(QString meshFileName READ meshFileName WRITE setMeshFileName NOTIFY meshFileNameChanged)
    Q_PROPERTY(Q3DTheme* theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(Q3DScene* scene READ scene)
    Q_PROPERTY(QtDataVisualization::QDataVis::ColorStyle colorStyle READ colorStyle WRITE setColorStyle NOTIFY colorStyleChanged)
    Q_PROPERTY(QColor itemColor READ itemColor WRITE setItemColor NOTIFY itemColorChanged)
    Q_PROPERTY(QLinearGradient itemGradient READ itemGradient WRITE setItemGradient NOTIFY itemGradientChanged)
    Q_PROPERTY(QColor singleHighlightColor READ singleHighlightColor WRITE setSingleHighlightColor NOTIFY singleHighlightColorChanged)
    Q_PROPERTY(QLinearGradient singleHighlightGradient READ singleHighlightGradient WRITE setSingleHighlightGradient NOTIFY singleHighlightGradientChanged)
    Q_PROPERTY(QColor multiHighlightColor READ multiHighlightColor WRITE setMultiHighlightColor NOTIFY multiHighlightColorChanged)
    Q_PROPERTY(QLinearGradient multiHighlightGradient READ multiHighlightGradient WRITE setMultiHighlightGradient NOTIFY multiHighlightGradientChanged)

public:
    explicit Q3DScatter();
    ~Q3DScatter();

    void addSeries(QScatter3DSeries *series);
    void removeSeries(QScatter3DSeries *series);
    QList<QScatter3DSeries *> seriesList();

    void setObjectType(QDataVis::MeshStyle style, bool smooth = false);

    void setTheme(Q3DTheme *theme);
    Q3DTheme *theme() const;

    void setMeshFileName(const QString &objFileName);
    QString meshFileName() const;

    void setSelectionMode(QDataVis::SelectionFlags mode);
    QDataVis::SelectionFlags selectionMode() const;

    Q3DScene *scene() const;

    void setWidth(const int width);
    void setHeight(const int height);

    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality() const;

    // TODO: Move to dataset object once that is done QTRD-2121
    void setColorStyle(QDataVis::ColorStyle style);
    QDataVis::ColorStyle colorStyle() const;
    void setItemColor(const QColor &color);
    QColor itemColor() const;
    void setItemGradient(const QLinearGradient &gradient);
    QLinearGradient itemGradient() const;
    void setSingleHighlightColor(const QColor &color);
    QColor singleHighlightColor() const;
    void setSingleHighlightGradient(const QLinearGradient &gradient);
    QLinearGradient singleHighlightGradient() const;
    void setMultiHighlightColor(const QColor &color);
    QColor multiHighlightColor() const;
    void setMultiHighlightGradient(const QLinearGradient &gradient);
    QLinearGradient multiHighlightGradient() const;

    void setAxisX(Q3DValueAxis *axis);
    Q3DValueAxis *axisX() const;
    void setAxisY(Q3DValueAxis *axis);
    Q3DValueAxis *axisY() const;
    void setAxisZ(Q3DValueAxis *axis);
    Q3DValueAxis *axisZ() const;
    void addAxis(Q3DValueAxis *axis);
    void releaseAxis(Q3DValueAxis *axis);
    QList<Q3DValueAxis *> axes() const;

signals:
    void selectionModeChanged(QDataVis::SelectionFlags mode);
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void meshFileNameChanged(QString filename);
    void themeChanged(Q3DTheme* theme);
    void colorStyleChanged(QDataVis::ColorStyle style);
    void itemColorChanged(QColor color);
    void itemGradientChanged(QLinearGradient gradient);
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
    QScopedPointer<Q3DScatterPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DScatter)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

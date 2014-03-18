/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#ifndef SCATTERDATAMODIFIER_H
#define SCATTERDATAMODIFIER_H

#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qscatter3dseries.h>

#include <QFont>
#include <QDebug>
#include <QTimer>

using namespace QtDataVisualization;

class ScatterDataModifier : public QObject
{
    Q_OBJECT
public:
    explicit ScatterDataModifier(Q3DScatter *scatter);
    ~ScatterDataModifier();

    void addData();
    void changeStyle();
    void changePresetCamera();
    void changeTheme();
    void changeLabelStyle();
    void changeFont(const QFont &font);
    void changeFontSize(int fontSize);
    void changePointSize(int pointSize);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void setMinX(int min);
    void setMinY(int min);
    void setMinZ(int min);
    void setMaxX(int max);
    void setMaxY(int max);
    void setMaxZ(int max);
    void start();

public slots:
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);
    void clear();
    void resetAxes();
    void addOne();
    void addBunch();
    void insertOne();
    void insertBunch();
    void changeOne();
    void changeBunch();
    void removeOne();
    void removeBunch();
    void timeout();
    void startStopTimer();
    void selectItem();
    void handleSelectionChange(int index);
    void setGradient();
    void addSeries();
    void removeSeries();
    void toggleSeriesVisibility();
    void changeSeriesName();

    void handleAxisXChanged(QValue3DAxis *axis);
    void handleAxisYChanged(QValue3DAxis *axis);
    void handleAxisZChanged(QValue3DAxis *axis);

signals:
    void shadowQualityChanged(int quality);

private:
    QVector3D randVector();
    QScatter3DSeries *createAndAddSeries();

    Q3DScatter *m_chart;
    int m_fontSize;
    QTimer m_timer;
    int m_loopCounter;
    int m_selectedItem;
    QScatter3DSeries *m_targetSeries;
};

#endif

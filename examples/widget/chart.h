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

#ifndef CHARTMODIFIER_H
#define CHARTMODIFIER_H

#include <QtDataVisualization/q3dbars.h>

#include <QFont>
#include <QDebug>
#include <QStringList>
#include <QPointer>

using namespace QtDataVisualization;

class ChartModifier : public QObject
{
    Q_OBJECT
public:
    explicit ChartModifier(Q3DBars *barchart);
    ~ChartModifier();

    void resetTemperatureData();
    void changePresetCamera();
    void changeTransparency();
    void changeFont(const QFont &font);
    void changeFontSize(int fontsize);
    void rotateX(int rotation);
    void rotateY(int rotation);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void setSmoothBars(int smooth);
    void start();

public slots:
    void changeStyle(int style);
    void changeSelectionMode(int selectionMode);
    void changeTheme(int theme);
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QDataVis::ShadowQuality shadowQuality);

signals:
    void shadowQualityChanged(int quality);

private:
    Q3DBars *m_chart;
    float m_xRotation;
    float m_yRotation;
    int m_fontSize;
    int m_segments;
    int m_subSegments;
    qreal m_minval;
    qreal m_maxval;
    QStringList m_months;
    QStringList m_years;
    Q3DValueAxis *m_temperatureAxis;
    Q3DCategoryAxis *m_yearAxis;
    Q3DCategoryAxis *m_monthAxis;
    QBarDataProxy *m_temperatureData;
    QDataVis::MeshStyle m_style;
    bool m_smooth;
};

#endif

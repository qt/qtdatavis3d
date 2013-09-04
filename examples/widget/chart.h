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

#ifndef CHARTMODIFIER_H
#define CHARTMODIFIER_H

#include <QtDataVis3D/q3dbars.h>

#include <QFont>
#include <QDebug>
#include <QStringList>
#include <QPointer>

using namespace QtDataVis3D;

class ChartModifier : public QObject
{
    Q_OBJECT
public:
    explicit ChartModifier(Q3DBars *barchart);
    ~ChartModifier();

    void resetTemperatureData();
    void changeStyle();
    void changePresetCamera();
    void changeTheme();
    void changeTransparency();
    void changeSelectionMode();
    void changeFont(const QFont &font);
    void changeFontSize(int fontsize);
    void rotateX(int rotation);
    void rotateY(int rotation);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void start();

public slots:
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
};

#endif

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

#ifndef RAINFALLCHART_H
#define RAINFALLCHART_H

#include "variantbardataproxy.h"
#include <QtDataVis3D/q3dbars.h>
#include <QTimer>

using namespace QtDataVis3D;

class RainfallChart : public QObject
{
    Q_OBJECT
public:
    explicit RainfallChart(Q3DBars *rainfall);
    ~RainfallChart();

    void addDataSet();
    void start();

public slots:
    void timeout();
private:

    void updateYearsList(int start, int end);
    Q3DBars *m_chart;
    int m_columnCount;
    int m_rowCount;
    QStringList m_years;
    QStringList m_numericMonths;
    QTimer m_timer;
    VariantBarDataProxy *m_proxy;
    int m_city;
    VariantBarDataMapping *m_mapping;
    VariantDataSet *m_dataSet;
};


#endif // RAINFALLCHART_H

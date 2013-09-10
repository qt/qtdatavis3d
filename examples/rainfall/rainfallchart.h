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

#ifndef RAINFALLCHART_H
#define RAINFALLCHART_H

#include "variantbardataproxy.h"
#include <QtDataVisualization/q3dbars.h>

using namespace QtDataVisualization;

class RainfallChart : public QObject
{
    Q_OBJECT
public:
    explicit RainfallChart(Q3DBars *rainfall);
    ~RainfallChart();

    void addDataSet();
    void start();

private:

    void updateYearsList(int start, int end);
    Q3DBars *m_chart;
    int m_columnCount;
    int m_rowCount;
    QStringList m_years;
    QStringList m_numericMonths;
    VariantBarDataProxy *m_proxy;
    VariantBarDataMapping *m_mapping;
    VariantDataSet *m_dataSet;
};


#endif // RAINFALLCHART_H

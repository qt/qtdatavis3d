/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef RAINFALLGRAPH_H
#define RAINFALLGRAPH_H

#include "variantbardataproxy.h"
#include <QtDataVisualization/q3dbars.h>

using namespace QtDataVisualization;

class RainfallGraph : public QObject
{
    Q_OBJECT
public:
    explicit RainfallGraph(Q3DBars *rainfall);
    ~RainfallGraph();

    void addDataSet();
    void start();

private:

    void updateYearsList(int start, int end);
    Q3DBars *m_graph;
    int m_columnCount;
    int m_rowCount;
    QStringList m_years;
    QStringList m_numericMonths;
    VariantBarDataProxy *m_proxy;
    VariantBarDataMapping *m_mapping;
    VariantDataSet *m_dataSet;
};


#endif

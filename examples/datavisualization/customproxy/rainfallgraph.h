/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

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

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

#ifndef VARIANTBARDATAPROXY_H
#define VARIANTBARDATAPROXY_H

#include "qbardataproxy.h"
#include "variantdataset.h"
#include "variantbardatamapping.h"
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QPointer>

using namespace QtDataVisualization;

//! [0]
class VariantBarDataProxy : public QBarDataProxy
//! [0]
{
    Q_OBJECT

public:
    explicit VariantBarDataProxy();
    explicit VariantBarDataProxy(VariantDataSet *newSet, VariantBarDataMapping *mapping);
    virtual ~VariantBarDataProxy();

    //! [1]
    // Doesn't gain ownership of the dataset, but does connect to it to listen for data changes.
    void setDataSet(VariantDataSet *newSet);
    VariantDataSet *dataSet();

    // Map key (row, column, value) to value index in data item (VariantItem).
    // Doesn't gain ownership of mapping, but does connect to it to listen for mapping changes.
    // Modifying mapping that is set to proxy will trigger dataset re-resolving.
    void setMapping(VariantBarDataMapping *mapping);
    VariantBarDataMapping *mapping();
    //! [1]

public slots:
    void handleItemsAdded(int index, int count);
    void handleDataCleared();
    void handleMappingChanged();

private:
    void resolveDataSet();

    QPointer<VariantDataSet> m_dataSet;
    QPointer<VariantBarDataMapping> m_mapping;

    Q_DISABLE_COPY(VariantBarDataProxy)
};

#endif

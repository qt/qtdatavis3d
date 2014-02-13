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

#ifndef VARIANTBARDATAPROXY_H
#define VARIANTBARDATAPROXY_H

#include "qbardataproxy.h"
#include "variantdataset.h"
#include "variantbardatamapping.h"
#include <QStringList>
#include <QMap>
#include <QPointer>

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

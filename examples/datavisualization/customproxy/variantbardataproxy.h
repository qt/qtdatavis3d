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

public Q_SLOTS:
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

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

#ifndef VARIANTDATASET_H
#define VARIANTDATASET_H

#include "qdatavis3dnamespace.h"
#include <QScopedPointer>
#include <QVariantList>

using namespace QtDataVis3D;

typedef QVariantList VariantDataItem;
typedef QList<VariantDataItem *> VariantDataItemList;

class VariantDataSet : public QObject
{
    Q_OBJECT

public:
    explicit VariantDataSet();
    ~VariantDataSet();

    void clear();

    int addItem(VariantDataItem *item);
    int addItems(VariantDataItemList *itemList);

    const VariantDataItemList &itemList() const;

signals:
    void itemsAdded(int index, int count);
    void dataCleared();

private:
    VariantDataItemList m_variantData;

    Q_DISABLE_COPY(VariantDataSet)
};

#endif

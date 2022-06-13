// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef VARIANTDATASET_H
#define VARIANTDATASET_H

#include <QtCore/QScopedPointer>
#include <QtCore/QVariantList>

//! [1]
typedef QVariantList VariantDataItem;
typedef QList<VariantDataItem *> VariantDataItemList;
//! [1]

class VariantDataSet : public QObject
{
    Q_OBJECT

public:
    explicit VariantDataSet();
    ~VariantDataSet();

    //! [0]
    void clear();

    int addItem(VariantDataItem *item);
    int addItems(VariantDataItemList *itemList);

    const VariantDataItemList &itemList() const;

Q_SIGNALS:
    void itemsAdded(int index, int count);
    void dataCleared();
    //! [0]

private:
    VariantDataItemList m_variantData;

    Q_DISABLE_COPY(VariantDataSet)
};

#endif

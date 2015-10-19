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

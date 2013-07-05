/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVARIANTBARDATAPROXY_H
#define QVARIANTBARDATAPROXY_H

#include "qbardataproxy.h"
#include "qvariantdataset.h"
#include <QStringList>
#include <QMap>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QVariantBarDataProxyPrivate;
class QVariantDataSet;
class QVariantBarDataProxy;

class QVariantBarMappingItem {
public:
    enum Mapping {
        MapRow = 0,
        MapColumn,
        MapValue
    };

    QVariantBarMappingItem()
        : itemIndex(0) {}

    QVariantBarMappingItem(int index)
        : itemIndex(index) {}

    QVariantBarMappingItem(int index, QStringList categories)
        : itemIndex(index),
          itemCategories(categories) {}

    QVariantBarMappingItem(const QVariantBarMappingItem &item)
        : itemIndex(item.itemIndex),
          itemCategories(item.itemCategories) {}

    ~QVariantBarMappingItem() {}

    // Index of the mapped item in the QVariantDataItem
    int itemIndex;

    // For row/column items, sort items into these categories. Other categories are ignored.
    // Not used for value mapping.
    QStringList itemCategories;
};

typedef QMap<QVariantBarMappingItem::Mapping, QVariantBarMappingItem> QVariantBarMapping;

class QT_DATAVIS3D_EXPORT QVariantBarDataProxy : public QBarDataProxy
{
    Q_OBJECT

public:
    explicit QVariantBarDataProxy();
    virtual ~QVariantBarDataProxy();

    void setDataSet(QVariantDataSet *newSet); // Gains ownership

    // Map key (row, column, value) to value index in data item (QVariantItem).
    void setMappings(const QVariantBarMapping &mappings);

protected:
    QVariantBarDataProxyPrivate *dptr();

private:
    Q_DISABLE_COPY(QVariantBarDataProxy)
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QBARDATAPROXY_H

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

#include "qitemmodelbardataproxy_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QItemModelBarDataProxy::QItemModelBarDataProxy() :
    QBarDataProxy(new QItemModelBarDataProxyPrivate(this))
{
}

QItemModelBarDataProxy::QItemModelBarDataProxy(QAbstractItemModel *itemModel,
                                               QItemModelBarDataMapping *mapping) :
    QBarDataProxy(new QItemModelBarDataProxyPrivate(this))
{
    dptr()->setItemModel(itemModel);
    dptr()->setMapping(mapping);
}

QItemModelBarDataProxy::~QItemModelBarDataProxy()
{
}

void QItemModelBarDataProxy::setItemModel(QAbstractItemModel *itemModel)
{
    dptr()->setItemModel(itemModel);
}

QAbstractItemModel *QItemModelBarDataProxy::itemModel()
{
    return dptr()->m_itemModel.data();
}

void QItemModelBarDataProxy::setMapping(QItemModelBarDataMapping *mapping)
{
    dptr()->setMapping(mapping);
}

QItemModelBarDataMapping *QItemModelBarDataProxy::mapping()
{
    return dptr()->m_mapping.data();
}

QItemModelBarDataProxyPrivate *QItemModelBarDataProxy::dptr()
{
    return static_cast<QItemModelBarDataProxyPrivate *>(d_ptr.data());
}

// QItemModelBarDataProxyPrivate

QItemModelBarDataProxyPrivate::QItemModelBarDataProxyPrivate(QItemModelBarDataProxy *q)
    : QBarDataProxyPrivate(q)
{
}

QItemModelBarDataProxyPrivate::~QItemModelBarDataProxyPrivate()
{
}

void QItemModelBarDataProxyPrivate::setItemModel(QAbstractItemModel *itemModel)
{
    if (!m_itemModel.isNull()) {
        // TODO disconnect
    }

    m_itemModel = itemModel;

    if (!m_itemModel.isNull()) {
        // TODO
        //QObject::connect(m_itemModel, &QAbstractItemModel::xxx, this, &QItemModelBarDataProxyPrivate::handleItemsAdded);
        //QObject::connect(m_itemModel, &QAbstractItemModel::yyy, this, &QItemModelBarDataProxyPrivate::handleDataCleared);
    }
    resolveModel();
}

void QItemModelBarDataProxyPrivate::setMapping(QItemModelBarDataMapping *mapping)
{
    if (!m_mapping.isNull())
        QObject::disconnect(m_mapping.data(), &QItemModelBarDataMapping::mappingChanged, this, &QItemModelBarDataProxyPrivate::handleMappingChanged);

    m_mapping = mapping;

    if (!m_mapping.isNull())
        QObject::connect(m_mapping.data(), &QItemModelBarDataMapping::mappingChanged, this, &QItemModelBarDataProxyPrivate::handleMappingChanged);

    resolveModel();
}

void QItemModelBarDataProxyPrivate::handleMappingChanged()
{
    resolveModel();
}

// Resolve entire item model into QBarDataArray.
void QItemModelBarDataProxyPrivate::resolveModel()
{
    if (m_itemModel.isNull() || m_mapping.isNull() || m_mapping->valueRole().isEmpty()) {
        qptr()->resetArray(0);
        return;
    }

    bool useModelRows(false);
    if (!m_mapping->rowCategories().size() || !m_mapping->columnCategories().size()) {
        useModelRows = true;
    } else if (m_mapping->rowRole().isEmpty() || m_mapping->columnRole().isEmpty()) {
        qptr()->resetArray(0);
        return;
    }

    QBarDataArray *newProxyArray = new QBarDataArray;
    QHash<int, QByteArray> roleHash = m_itemModel->roleNames();
    int valueRole = roleHash.key(m_mapping->valueRole().toLatin1());
    int rowCount = m_itemModel->rowCount();
    int columnCount = m_itemModel->columnCount();

    if (useModelRows) {
        for (int i = 0; i < rowCount; i++) {
            QBarDataRow *newProxyRow = new QBarDataRow(columnCount);
            for (int j = 0; j < columnCount; j++)
                (*newProxyRow)[i].setValue(m_itemModel->index(i, j).data(valueRole).toReal());
            newProxyArray->append(newProxyRow);
        }
    } else {
        int rowRole = roleHash.key(m_mapping->rowRole().toLatin1());
        int columnRole = roleHash.key(m_mapping->columnRole().toLatin1());
        const QStringList &rowList = m_mapping->rowCategories();
        const QStringList &columnList = m_mapping->columnCategories();

        // Sort values into rows and columns
        typedef QHash<QString, qreal> ColumnValueMap;
        QHash <QString, ColumnValueMap> itemValueMap;
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < columnCount; j++) {
                QModelIndex index = m_itemModel->index(i, j);
                itemValueMap[index.data(rowRole).toString()][index.data(columnRole).toString()]
                        = index.data(valueRole).toReal();
            }
        }

        // Create new data array from itemValueMap
        foreach (QString rowKey, rowList) {
            QBarDataRow *newProxyRow = new QBarDataRow(columnList.size());
            for (int i = 0; i < columnList.size(); i++)
                (*newProxyRow)[i].setValue(itemValueMap[rowKey][columnList.at(i)]);
            newProxyArray->append(newProxyRow);
        }
    }
    qDebug() << __FUNCTION__ << "RowCount:" << newProxyArray->size() << "Column count:" << newProxyArray->at(0)->size();

    qptr()->resetArray(newProxyArray);
}

QItemModelBarDataProxy *QItemModelBarDataProxyPrivate::qptr()
{
    return static_cast<QItemModelBarDataProxy *>(q_ptr);
}

QT_DATAVIS3D_END_NAMESPACE

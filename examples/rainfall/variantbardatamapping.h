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

#ifndef VARIANTBARDATAMAPPING_H
#define VARIANTBARDATAMAPPING_H

#include "qdatavis3dnamespace.h"
#include <QStringList>

using namespace QtDataVis3D;

class VariantBarDataMapping : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rowIndex READ rowIndex WRITE setRowIndex)
    Q_PROPERTY(int columnIndex READ columnIndex WRITE setColumnIndex)
    Q_PROPERTY(int valueIndex READ valueIndex WRITE setValueIndex)
    Q_PROPERTY(QStringList rowCategories READ rowCategories WRITE setRowCategories)
    Q_PROPERTY(QStringList columnCategories READ columnCategories WRITE setColumnCategories)
public:
    explicit VariantBarDataMapping();
    VariantBarDataMapping(const VariantBarDataMapping &other);
    VariantBarDataMapping(int rowIndex, int columnIndex, int valueIndex,
                           const QStringList &rowCategories,
                           const QStringList &columnCategories);
    virtual ~VariantBarDataMapping();

    VariantBarDataMapping &operator=(const VariantBarDataMapping &other);

    void setRowIndex(int index);
    int rowIndex() const;
    void setColumnIndex(int index);
    int columnIndex() const;
    void setValueIndex(int index);
    int valueIndex() const;

    void setRowCategories(const QStringList &categories);
    const QStringList &rowCategories() const;
    void setColumnCategories(const QStringList &categories);
    const QStringList &columnCategories() const;

    void remap(int rowIndex, int columnIndex, int valueIndex,
               const QStringList &rowCategories,
               const QStringList &columnCategories);
signals:
    void mappingChanged();

private:
    // Indexes of the mapped items in the VariantDataItem
    int m_rowIndex;
    int m_columnIndex;
    int m_valueIndex;

    // For row/column items, sort items into these categories. Other categories are ignored.
    QStringList m_rowCategories;
    QStringList m_columnCategories;
};

#endif

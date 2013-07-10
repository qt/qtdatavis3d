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

#include "qvariantbardatamapping_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

QVariantBarDataMapping::QVariantBarDataMapping()
    : QObject(0),
      d_ptr(new QVariantBarDataMappingPrivate(this))
{
}

QVariantBarDataMapping::QVariantBarDataMapping(int rowIndex, int columnIndex, int valueIndex,
                                               const QStringList &rowCategories,
                                               const QStringList &columnCategories)
    : QObject(0),
      d_ptr(new QVariantBarDataMappingPrivate(this))
{
    d_ptr->m_rowIndex = rowIndex;
    d_ptr->m_columnIndex = columnIndex;
    d_ptr->m_valueIndex = valueIndex;
    d_ptr->m_rowCategories = rowCategories;
    d_ptr->m_columnCategories = columnCategories;
}

QVariantBarDataMapping::~QVariantBarDataMapping()
{
}

void QVariantBarDataMapping::setRowIndex(int index)
{
    d_ptr->m_rowIndex = index;
    emit mappingChanged();
}

int QVariantBarDataMapping::rowIndex() const
{
    return d_ptr->m_rowIndex;
}

void QVariantBarDataMapping::setColumnIndex(int index)
{
    d_ptr->m_columnIndex = index;
    emit mappingChanged();
}

int QVariantBarDataMapping::columnIndex() const
{
    return d_ptr->m_columnIndex;
}

void QVariantBarDataMapping::setValueIndex(int index)
{
    d_ptr->m_valueIndex = index;
    emit mappingChanged();
}

int QVariantBarDataMapping::valueIndex() const
{
    return d_ptr->m_valueIndex;
}

void QVariantBarDataMapping::setRowCategories(const QStringList &categories)
{
    d_ptr->m_rowCategories = categories;
    emit mappingChanged();
}

const QStringList &QVariantBarDataMapping::rowCategories() const
{
    return d_ptr->m_rowCategories;
}

void QVariantBarDataMapping::setColumnCategories(const QStringList &categories)
{
    d_ptr->m_columnCategories = categories;
    emit mappingChanged();
}

const QStringList &QVariantBarDataMapping::columnCategories() const
{
    return d_ptr->m_columnCategories;
}

void QVariantBarDataMapping::remap(int rowIndex, int columnIndex, int valueIndex,
                                   const QStringList &rowCategories,
                                   const QStringList &columnCategories)
{
    d_ptr->m_rowIndex = rowIndex;
    d_ptr->m_columnIndex = columnIndex;
    d_ptr->m_valueIndex = valueIndex;
    d_ptr->m_rowCategories = rowCategories;
    d_ptr->m_columnCategories = columnCategories;
    emit mappingChanged();
}

// QVariantBarDataMappingPrivate

QVariantBarDataMappingPrivate::QVariantBarDataMappingPrivate(QVariantBarDataMapping *q)
    : QObject(0),
      q_ptr(q),
      m_rowIndex(0),
      m_columnIndex(1),
      m_valueIndex(2)
{
}

QVariantBarDataMappingPrivate::~QVariantBarDataMappingPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE

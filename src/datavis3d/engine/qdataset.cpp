/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "qdataset.h"
#include "qdataset_p.h"
#include "qdatarow.h"
#include "qdatarow_p.h"

#include <QPoint>
#include <QString>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

QDataSet::QDataSet()
    : d_ptr(new QDataSetPrivate(this))
{
    qDebug("QDataSet");
}

QDataSet::~QDataSet()
{
    qDebug("~QDataSet");
}

void QDataSet::setLabels(const QString &xAxis
                         , const QString &zAxis
                         , const QString &yAxis
                         , const QVector<QString> &labelsRow
                         , const QVector<QString> &labelsColumn)
{
    d_ptr->m_xAxis = xAxis;
    d_ptr->m_zAxis = zAxis;
    d_ptr->m_yAxis = yAxis;
    d_ptr->m_labelsRow = labelsRow;
    d_ptr->m_labelsColumn = labelsColumn;
}

void QDataSet::addRow(QDataRow *row)
{
    d_ptr->m_set.prepend(row);
}

QDataSetPrivate::QDataSetPrivate(QDataSet *q)
    : q_ptr(q)
    , m_set(QVector<QDataRow*>())
    , m_xAxis(QString())
    , m_zAxis(QString())
    , m_yAxis(QString())
    , m_labelsRow(QVector<QString>())
    , m_labelsColumn(QVector<QString>())
{
}

QDataSetPrivate::~QDataSetPrivate()
{
    for (int itemCount = 0; itemCount < m_set.size(); itemCount++) {
        delete m_set.at(itemCount);
    }
    m_set.clear();
}

QVector<QDataRow*> QDataSetPrivate::set()
{
    return m_set;
}

QVector<QString> QDataSetPrivate::rowLabels()
{
    return m_labelsRow;
}

QVector<QString> QDataSetPrivate::columnLabels()
{
    return m_labelsColumn;
}

void QDataSetPrivate::axisLabels(QString *xAxis, QString *zAxis, QString *yAxis)
{
    *xAxis = m_xAxis;
    *zAxis = m_zAxis;
    *yAxis = m_yAxis;
}

void QDataSetPrivate::verifySize(int colSize, int rowSize)
{
    qDebug("verifySize (QDataSet)");
    // First verify columns
    // QVector's resize doesn't delete data contained in it
    // Delete contents of rows to be removed
    if (colSize > m_set.size()) {
        int nbrToBeRemoved = m_set.size() - colSize;
        for (int rowCount = 0; rowCount < nbrToBeRemoved; rowCount++) {
            int rowToBeRemoved = m_set.size() - rowCount - 1; // -1 to compensate index 0
            delete m_set.at(rowToBeRemoved);
        }
        // Resize vector
        m_set.resize(colSize);
    }
    // Then verify each row left
    if (rowSize) {
        for (int i = 0; i < m_set.size(); i++)
            m_set.at(i)->d_ptr->verifySize(rowSize);
    }
}

float QDataSetPrivate::highestValue()
{
    float max = 0;
    float rowMax = 0;
    for (int i = 0; i < m_set.size(); i++) {
        rowMax = m_set.at(i)->d_ptr->highestValue();
        if (max < rowMax)
            max = rowMax;
    }
    return max;
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

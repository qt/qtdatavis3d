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

#include "qdataset.h"
#include "qdataset_p.h"
#include "qdatarow.h"
#include "qdatarow_p.h"

#include "qvalueaxis.h"
#include "qcategoryaxis.h"
#include "qabstractaxis_p.h"

#include <QPoint>
#include <QString>

//#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class QDataSet
 * \inmodule QtDataVis3D
 * \brief The QDataSet class provides a container for data rows to be added to graphs.
 * \since 1.0.0
 *
 * A QDataSet is a container for data to be added into a Q3DBars instance. It holds instances of
 * QDataRow and has no knowledge of sample space size set for Q3DBars. The ownership of QDataRow
 * added is transferred to QDataSet.
 *
 * \sa QDataItem, QDataRow, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * Constructs QDataSet.
 */
QDataSet::QDataSet()
    : d_ptr(new QDataSetPrivate(this))
{
}

/*!
 * Destroys QDataSet, including all QDataRow instances it may hold.
 */
QDataSet::~QDataSet()
{
}

/*!
 * \a xAxis A QString label for x axis.
 *
 * \a zAxis A QString label for z axis.
 *
 * \a yAxis A QString label for y axis.
 *
 * \a labelsRow A QVector of QStrings, one for each row.
 *
 * \a labelsColumn A QVector of QStrings, one for each column.
 *
 * Sets labels for the QDataSet.
 */
void QDataSet::setLabels(const QString &xAxis,
                         const QString &zAxis,
                         const QString &yAxis,
                         const QVector<QString> &labelsRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_axisX->setTitle(xAxis);
    d_ptr->m_axisZ->setTitle(zAxis);
    d_ptr->m_axisY->setTitle(yAxis);

    static_cast<QCategoryAxis *>(d_ptr->m_axisX)->setLabels(labelsRow);
    static_cast<QCategoryAxis *>(d_ptr->m_axisZ)->setLabels(labelsColumn);
}

/*!
 * \a row A QDataRow instance.
 *
 * Adds a QDataRow instance to QDataSet. Ownership of the QDataRow instance is transferred to
 * QDataSet.
 */
void QDataSet::addRow(QDataRow *row)
{
    d_ptr->m_set.prepend(row);
}

QDataSetPrivate::QDataSetPrivate(QDataSet *q)
    : q_ptr(q),
      m_set(QVector<QDataRow*>()),
      m_axisX(new QCategoryAxis()),
      m_axisY(new QValueAxis()),
      m_axisZ(new QCategoryAxis())
{
}

QDataSetPrivate::~QDataSetPrivate()
{
    for (int itemCount = 0; itemCount < m_set.size(); itemCount++)
        delete m_set.at(itemCount);
    m_set.clear();
}

void QDataSetPrivate::setDrawer(Drawer *drawer)
{
    m_axisX->d_ptr->setDrawer(drawer);
    m_axisY->d_ptr->setDrawer(drawer);
    m_axisZ->d_ptr->setDrawer(drawer);
}

QVector<QDataRow*> QDataSetPrivate::set()
{
    return m_set;
}

QDataRow *QDataSetPrivate::getRow(int rowIndex)
{
    QDataRow *row = NULL;
    if (m_set.size() > rowIndex)
        row = m_set.at(rowIndex);
    return row;
}

QVector<QString> QDataSetPrivate::rowLabels()
{
    // TODO get rid of this function
    return m_axisX->labels();
}

QVector<QString> QDataSetPrivate::columnLabels()
{
    // TODO get rid of this function
    return m_axisZ->labels();
}

QVector<LabelItem> QDataSetPrivate::rowLabelItems()
{
    // TODO get rid of this function
    return m_axisX->d_ptr->labelItems();
}

QVector<LabelItem> QDataSetPrivate::columnLabelItems()
{
    // TODO get rid of this function
    return m_axisZ->d_ptr->labelItems();
}

void QDataSetPrivate::axisLabels(QString *xAxis, QString *zAxis, QString *yAxis)
{
    // TODO get rid of this function
    *xAxis = m_axisX->title();
    *zAxis = m_axisZ->title();
    *yAxis = m_axisY->title();
}

void QDataSetPrivate::axisLabelItems(LabelItem *xAxisItem, LabelItem *zAxisItem,
                                     LabelItem *yAxisItem)
{
    // TODO get rid of this function
    *xAxisItem = m_axisX->d_ptr->titleItem();
    *zAxisItem = m_axisZ->d_ptr->titleItem();
    *yAxisItem = m_axisY->d_ptr->titleItem();
}

void QDataSetPrivate::verifySize(int colSize, int rowSize)
{
    // First verify columns
    // QVector's resize doesn't delete data contained in it
    // Delete contents of rows to be removed
    if (colSize < m_set.size()) {
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

QPair<GLfloat, GLfloat> QDataSetPrivate::limitValues()
{
    QPair<GLfloat, GLfloat> limits = qMakePair(100.0f, -100.0f);
    QPair<GLfloat, GLfloat> rowLimits;
    for (int i = 0; i < m_set.size(); i++) {
        rowLimits = m_set.at(i)->d_ptr->limitValues();
        if (limits.second < rowLimits.second)
            limits.second = rowLimits.second;
        if (limits.first > rowLimits.first)
            limits.first = rowLimits.first;
    }
    return limits;
}

QT_DATAVIS3D_END_NAMESPACE

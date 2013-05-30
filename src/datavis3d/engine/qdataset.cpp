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

#include <QPoint>
#include <QString>

//#include <QDebug>

QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE

const QString empty;

QDataSet::QDataSet()
    : d_ptr(new QDataSetPrivate(this))
{
    //qDebug("QDataSet");
}

QDataSet::~QDataSet()
{
    //qDebug("~QDataSet");
}

void QDataSet::setLabels(const QString &xAxis,
                         const QString &zAxis,
                         const QString &yAxis,
                         const QVector<QString> &labelsRow,
                         const QVector<QString> &labelsColumn)
{
    // skip empty labels, keep the previous ones
    if (xAxis != empty && d_ptr->m_xAxis != xAxis) {
        d_ptr->m_xAxis = xAxis;
        // Generate axis label texture
        if (d_ptr->m_drawer)
            d_ptr->m_drawer->generateLabelItem(&d_ptr->m_xAxisItem, xAxis);
    }
    if (zAxis != empty && d_ptr->m_zAxis != zAxis) {
        d_ptr->m_zAxis = zAxis;
        // Generate axis label texture
        if (d_ptr->m_drawer)
            d_ptr->m_drawer->generateLabelItem(&d_ptr->m_zAxisItem, zAxis);
    }
    if (yAxis != empty && d_ptr->m_yAxis != yAxis) {
        d_ptr->m_yAxis = yAxis;
        // Generate axis label texture
        if (d_ptr->m_drawer)
            d_ptr->m_drawer->generateLabelItem(&d_ptr->m_yAxisItem, yAxis);
    }
    d_ptr->m_labelsRow = labelsRow;
    d_ptr->m_labelsColumn = labelsColumn;
    // Generate row and column label textures
    if (d_ptr->m_drawer) {
        for (int itemCount = 0; itemCount < labelsColumn.size(); itemCount++) {
            d_ptr->m_labelItemsColumn.append(LabelItem());
            d_ptr->m_drawer->generateLabelItem(&d_ptr->m_labelItemsColumn[itemCount],
                                               labelsColumn.at(itemCount));
        }
        for (int itemCount = 0; itemCount < labelsRow.size(); itemCount++) {
            d_ptr->m_labelItemsRow.append(LabelItem());
            d_ptr->m_drawer->generateLabelItem(&d_ptr->m_labelItemsRow[itemCount],
                                               labelsRow.at(itemCount));
        }
    }
}

void QDataSet::addRow(QDataRow *row)
{
    d_ptr->m_set.prepend(row);
}

QDataSetPrivate::QDataSetPrivate(QDataSet *q)
    : q_ptr(q),
      m_set(QVector<QDataRow*>()),
      m_xAxis(QString()),
      m_zAxis(QString()),
      m_yAxis(QString()),
      m_labelsRow(QVector<QString>()),
      m_labelsColumn(QVector<QString>()),
      m_xAxisItem(LabelItem()),
      m_zAxisItem(LabelItem()),
      m_yAxisItem(LabelItem()),
      m_labelItemsRow(QVector<LabelItem>()),
      m_labelItemsColumn(QVector<LabelItem>()),
      m_drawer(0)
{
}

QDataSetPrivate::~QDataSetPrivate()
{
    for (int itemCount = 0; itemCount < m_set.size(); itemCount++)
        delete m_set.at(itemCount);
    m_set.clear();
    // Delete axis textures
    GLuint textureid = m_xAxisItem.textureId();
    if (textureid)
        glDeleteTextures(1, &textureid);
    textureid = m_zAxisItem.textureId();
    if (textureid)
        glDeleteTextures(1, &textureid);
    textureid = m_yAxisItem.textureId();
    if (textureid)
        glDeleteTextures(1, &textureid);
    // Delete row and column textures
    for (int itemCount = 0; itemCount < m_labelItemsColumn.size(); itemCount++) {
        LabelItem item = m_labelItemsColumn.at(itemCount);
        textureid = item.textureId();
        if (textureid)
            glDeleteTextures(1, &textureid);
    }
    for (int itemCount = 0; itemCount < m_labelItemsRow.size(); itemCount++) {
        LabelItem item = m_labelItemsRow.at(itemCount);
        textureid = item.textureId();
        if (textureid)
            glDeleteTextures(1, &textureid);
    }
}

void QDataSetPrivate::setDrawer(Drawer *drawer)
{
    m_drawer = drawer;
    connect(m_drawer, SIGNAL(drawerChanged()), this, SLOT(updateTextures()));
    updateTextures();
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
    return m_labelsRow;
}

QVector<QString> QDataSetPrivate::columnLabels()
{
    return m_labelsColumn;
}

QVector<LabelItem> QDataSetPrivate::rowLabelItems()
{
    return m_labelItemsRow;
}

QVector<LabelItem> QDataSetPrivate::columnLabelItems()
{
    return m_labelItemsColumn;
}

void QDataSetPrivate::axisLabels(QString *xAxis, QString *zAxis, QString *yAxis)
{
    *xAxis = m_xAxis;
    *zAxis = m_zAxis;
    *yAxis = m_yAxis;
}

void QDataSetPrivate::axisLabelItems(LabelItem *xAxisItem, LabelItem *zAxisItem,
                                     LabelItem *yAxisItem)
{
    *xAxisItem = m_xAxisItem;
    *zAxisItem = m_zAxisItem;
    *yAxisItem = m_yAxisItem;
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

void QDataSetPrivate::updateTextures()
{
    if (m_xAxis != empty)
        m_drawer->generateLabelItem(&m_xAxisItem, m_xAxis);
    if (m_zAxis != empty)
        m_drawer->generateLabelItem(&m_zAxisItem, m_zAxis);
    if (m_yAxis != empty)
        m_drawer->generateLabelItem(&m_yAxisItem, m_yAxis);
    for (int itemCount = 0; itemCount < m_labelsColumn.size(); itemCount++) {
        if (m_labelItemsColumn.size() < itemCount + 1)
            m_labelItemsColumn.append(LabelItem());
        m_drawer->generateLabelItem(&m_labelItemsColumn[itemCount], m_labelsColumn.at(itemCount));
    }
    for (int itemCount = 0; itemCount < m_labelsRow.size(); itemCount++) {
        if (m_labelItemsRow.size() < itemCount + 1)
            m_labelItemsRow.append(LabelItem());
        m_drawer->generateLabelItem(&m_labelItemsRow[itemCount], m_labelsRow.at(itemCount));
    }
}

QTENTERPRISE_DATAVIS3D_END_NAMESPACE

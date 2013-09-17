/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QBARDATAPROXY_P_H
#define QBARDATAPROXY_P_H

#include "qbardataproxy.h"
#include "qabstractdataproxy_p.h"
#include "qbardataitem.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QBarDataProxyPrivate : public QAbstractDataProxyPrivate
{
    Q_OBJECT
public:
    QBarDataProxyPrivate(QBarDataProxy *q);
    virtual ~QBarDataProxyPrivate();

    bool resetArray(QBarDataArray *newArray, const QStringList *rowLabels,
                    const QStringList *columnLabels);
    void setRow(int rowIndex, QBarDataRow *row, const QString *label);
    void setRows(int rowIndex, const QBarDataArray &rows, const QStringList *labels);
    void setItem(int rowIndex, int columnIndex, const QBarDataItem &item);
    int addRow(QBarDataRow *row, const QString *label);
    int addRows(const QBarDataArray &rows, const QStringList *labels);
    void insertRow(int rowIndex, QBarDataRow *row, const QString *label);
    void insertRows(int rowIndex, const QBarDataArray &rows, const QStringList *labels);
    void removeRows(int rowIndex, int removeCount, bool removeLabels);

    QPair<GLfloat, GLfloat> limitValues(int startRow, int startColumn, int rowCount,
                                        int columnCount) const;

private:
    QBarDataProxy *qptr();
    void clearRow(int rowIndex);
    void clearArray();
    void fixRowLabels(int startIndex, int count, const QStringList &newLabels, bool isInsert);

    QBarDataArray *m_dataArray;
    QStringList m_rowLabels;
    QStringList m_columnLabels;

private:
    friend class QBarDataProxy;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QBARDATAPROXY_P_H

/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
******************************************************************************/

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

QT_BEGIN_NAMESPACE

class QBarDataProxyPrivate : public QAbstractDataProxyPrivate
{
    Q_OBJECT
public:
    QBarDataProxyPrivate(QBarDataProxy *q);
    virtual ~QBarDataProxyPrivate();

    void resetArray(QBarDataArray *newArray, const QStringList *rowLabels,
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

    void setSeries(QAbstract3DSeries *series) override;

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

QT_END_NAMESPACE

#endif

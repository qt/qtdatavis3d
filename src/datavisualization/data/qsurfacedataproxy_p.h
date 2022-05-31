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

#ifndef QSURFACEDATAPROXY_P_H
#define QSURFACEDATAPROXY_P_H

#include "qsurfacedataproxy.h"
#include "qabstractdataproxy_p.h"

QT_BEGIN_NAMESPACE

class QAbstract3DAxis;

class QSurfaceDataProxyPrivate : public QAbstractDataProxyPrivate
{
    Q_OBJECT
public:
    QSurfaceDataProxyPrivate(QSurfaceDataProxy *q);
    virtual ~QSurfaceDataProxyPrivate();

    void resetArray(QSurfaceDataArray *newArray);
    void setRow(int rowIndex, QSurfaceDataRow *row);
    void setRows(int rowIndex, const QSurfaceDataArray &rows);
    void setItem(int rowIndex, int columnIndex, const QSurfaceDataItem &item);
    int addRow(QSurfaceDataRow *row);
    int addRows(const QSurfaceDataArray &rows);
    void insertRow(int rowIndex, QSurfaceDataRow *row);
    void insertRows(int rowIndex, const QSurfaceDataArray &rows);
    void removeRows(int rowIndex, int removeCount);
    void limitValues(QVector3D &minValues, QVector3D &maxValues, QAbstract3DAxis *axisX,
                     QAbstract3DAxis *axisY, QAbstract3DAxis *axisZ) const;
    bool isValidValue(float value, QAbstract3DAxis *axis) const;

    void setSeries(QAbstract3DSeries *series) override;

protected:
    QSurfaceDataArray *m_dataArray;

private:
    QSurfaceDataProxy *qptr();
    void clearRow(int rowIndex);
    void clearArray();

    friend class QSurfaceDataProxy;
};

QT_END_NAMESPACE

#endif

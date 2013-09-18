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

#ifndef QSURFACEDATAPROXY_H
#define QSURFACEDATAPROXY_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtDataVisualization/qabstractdataproxy.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

typedef qreal QSurfaceDataItem; // Let's stick to this for now, expand if needed
typedef QVector<QSurfaceDataItem> QSurfaceDataRow;
typedef QList<QSurfaceDataRow *> QSurfaceDataArray;

class QSurfaceDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QSurfaceDataProxy : public QAbstractDataProxy
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount)
    Q_PROPERTY(int columnCount READ columnCount)
    Q_PROPERTY(qreal minValueRows READ minValueRows WRITE setMinValueRows NOTIFY valueRangeRowsChanged)
    Q_PROPERTY(qreal maxValueRows READ maxValueRows WRITE setMaxValueRows NOTIFY valueRangeRowsChanged)
    Q_PROPERTY(qreal minValueColumns READ minValueColumns WRITE setMinValueColumns NOTIFY valueRangeColumnsChanged)
    Q_PROPERTY(qreal maxValueColumns READ maxValueColumns WRITE setMaxValueColumns NOTIFY valueRangeColumnsChanged)

public:
    explicit QSurfaceDataProxy(QObject *parent = 0);
    virtual ~QSurfaceDataProxy();

    int rowCount() const;
    int columnCount() const;
    const QSurfaceDataArray *array() const;
    const QSurfaceDataItem *itemAt(int index) const;

    void setValueRangeRows(qreal min, qreal max);
    void setValueRangeColumns(qreal min, qreal max);
    void setMinValueRows(qreal min);
    qreal minValueRows() const;
    void setMaxValueRows(qreal max);
    qreal maxValueRows() const;
    void setMinValueColumns(qreal min);
    qreal minValueColumns() const;
    void setMaxValueColumns(qreal max);
    qreal maxValueColumns() const;

    void resetArray(QSurfaceDataArray *newArray);
    void resetArray(QSurfaceDataArray *newArray, qreal minValueRows, qreal maxValueRows,
                    qreal minValueColumns, qreal maxValueColumns);

signals:
    void arrayReset();
    void valueRangeRowsChanged(int min, int max);
    void valueRangeColumnsChanged(int min, int max);

protected:
    explicit QSurfaceDataProxy(QSurfaceDataProxyPrivate *d, QObject *parent = 0);
    QSurfaceDataProxyPrivate *dptr();
    const QSurfaceDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QSurfaceDataProxy)

    friend class Surface3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QSURFACEDATAPROXY_H

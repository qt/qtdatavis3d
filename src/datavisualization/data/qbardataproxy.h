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

#ifndef QBARDATAPROXY_H
#define QBARDATAPROXY_H

#include <QtDataVisualization/qabstractdataproxy.h>
#include <QtDataVisualization/qbardataitem.h>
#include <QVector>
#include <QStringList>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

typedef QVector<QBarDataItem> QBarDataRow;
typedef QList<QBarDataRow *> QBarDataArray;

class QBarDataProxyPrivate;
class QBar3DSeries;

class QT_DATAVISUALIZATION_EXPORT QBarDataProxy : public QAbstractDataProxy
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount)
    Q_PROPERTY(QStringList rowLabels READ rowLabels WRITE setRowLabels NOTIFY rowLabelsChanged)
    Q_PROPERTY(QStringList columnLabels READ columnLabels WRITE setColumnLabels NOTIFY columnLabelsChanged)
    Q_PROPERTY(QBar3DSeries *series READ series NOTIFY seriesChanged)
public:
    explicit QBarDataProxy(QObject *parent = 0);
    virtual ~QBarDataProxy();

    // TODO: Replace first part of class description in docs with this once all TODOs are done:
    /*
    * QBarDataProxy is optimized for adding, inserting, and removing rows of data.
    * Adding a column essentially means modifying every row, which is comparatively very inefficient.
    * Proxy is also optimized to use cases where the only defining characteristic of an individual
    * bar is its value. Modifying other data that might be added in the future such as color of
    * individual bar requires allocating additional data object for the bar.
    */

    QBar3DSeries *series();
    int rowCount() const;

    QStringList rowLabels() const;
    void setRowLabels(const QStringList &labels);
    QStringList columnLabels() const;
    void setColumnLabels(const QStringList &labels);

    const QBarDataArray *array() const;
    const QBarDataRow *rowAt(int rowIndex) const;
    const QBarDataItem *itemAt(int rowIndex, int columnIndex) const;

    void resetArray();
    void resetArray(QBarDataArray *newArray);
    void resetArray(QBarDataArray *newArray, const QStringList &rowLabels,
                    const QStringList &columnLabels);

    void setRow(int rowIndex, QBarDataRow *row);
    void setRow(int rowIndex, QBarDataRow *row, const QString &label);
    void setRows(int rowIndex, const QBarDataArray &rows);
    void setRows(int rowIndex, const QBarDataArray &rows, const QStringList &labels);

    // Setting a column is comparatively inefficient as it changes all rows.
    // Can resize rows that are shorter than columnIndex.
    // TODO void setColumn(int columnIndex, const QBarDataRow &column);
    // TODO void setColumns(int columnIndex, const QBarDataArray &columns);

    void setItem(int rowIndex, int columnIndex, const QBarDataItem &item);
    // Change block of items
    // TODO setItems(int rowIndex, int columnIndex, QBarDataArray *items);

    int addRow(QBarDataRow *row);
    int addRow(QBarDataRow *row, const QString &label);
    int addRows(const QBarDataArray &rows);
    int addRows(const QBarDataArray &rows, const QStringList &labels);
    // TODO int addColumn(const QBarDataRow &column); // returns the index of the added column
    // TODO int addColumns(const QBarDataArray &columns); // returns the index of the first added column

    void insertRow(int rowIndex, QBarDataRow *row);
    void insertRow(int rowIndex, QBarDataRow *row, const QString &label);
    void insertRows(int rowIndex, const QBarDataArray &rows);
    void insertRows(int rowIndex, const QBarDataArray &rows, const QStringList &labels);
    // TODO void insertColumn(int columnIndex, const QBarDataRow &column);
    // TODO void insertColumns(int columnIndex, const QBarDataArray &columns);

    void removeRows(int rowIndex, int removeCount, bool removeLabels = true);
    // TODO void removeColumns(int columnIndex, int removeCount);

signals:
    void arrayReset();
    void rowsAdded(int startIndex, int count);
    void rowsChanged(int startIndex, int count);
    void rowsRemoved(int startIndex, int count);
    void rowsInserted(int startIndex, int count);
    // TODO void columnsChanged(int startIndex, int count);
    // TODO void columnsAdded(int startIndex, int count);
    // TODO void columnsRemoved(int startIndex, int count);
    // TODO void columnsInserted(int startIndex, int count);
    void itemChanged(int rowIndex, int columnIndex); // TODO remove once itemsChanged is added?
    // TODO void itemsChanged(int rowIndex, int columnIndex, int rowCount, int columnCount);

    void rowLabelsChanged();
    void columnLabelsChanged();
    void seriesChanged(QBar3DSeries *series);

protected:
    explicit QBarDataProxy(QBarDataProxyPrivate *d, QObject *parent = 0);
    QBarDataProxyPrivate *dptr();
    const QBarDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QBarDataProxy)

    friend class Bars3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QBARDATAPROXY_H

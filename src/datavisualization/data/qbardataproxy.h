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

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
    Q_PROPERTY(QStringList rowLabels READ rowLabels WRITE setRowLabels NOTIFY rowLabelsChanged)
    Q_PROPERTY(QStringList columnLabels READ columnLabels WRITE setColumnLabels NOTIFY columnLabelsChanged)
    Q_PROPERTY(QBar3DSeries *series READ series NOTIFY seriesChanged)
public:
    explicit QBarDataProxy(QObject *parent = 0);
    virtual ~QBarDataProxy();

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

    void setItem(int rowIndex, int columnIndex, const QBarDataItem &item);

    int addRow(QBarDataRow *row);
    int addRow(QBarDataRow *row, const QString &label);
    int addRows(const QBarDataArray &rows);
    int addRows(const QBarDataArray &rows, const QStringList &labels);

    void insertRow(int rowIndex, QBarDataRow *row);
    void insertRow(int rowIndex, QBarDataRow *row, const QString &label);
    void insertRows(int rowIndex, const QBarDataArray &rows);
    void insertRows(int rowIndex, const QBarDataArray &rows, const QStringList &labels);

    void removeRows(int rowIndex, int removeCount, bool removeLabels = true);

signals:
    void arrayReset();
    void rowsAdded(int startIndex, int count);
    void rowsChanged(int startIndex, int count);
    void rowsRemoved(int startIndex, int count);
    void rowsInserted(int startIndex, int count);
    void itemChanged(int rowIndex, int columnIndex);

    void rowCountChanged(int count);
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

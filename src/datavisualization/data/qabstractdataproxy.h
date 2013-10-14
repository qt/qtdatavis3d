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

#ifndef QABSTRACTDATAPROXY_H
#define QABSTRACTDATAPROXY_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QObject>
#include <QScopedPointer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QAbstractDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QAbstractDataProxy : public QObject
{
    Q_OBJECT
    Q_ENUMS(DataType)
    Q_PROPERTY(DataType type READ type)
    Q_PROPERTY(QString itemLabelFormat READ itemLabelFormat WRITE setItemLabelFormat NOTIFY itemLabelFormatChanged)

public:
    enum DataType {
        DataTypeNone = 0,
        DataTypeBar = 1,
        DataTypeScatter = 2,
        DataTypeSurface = 4
    };

protected:
    explicit QAbstractDataProxy(QAbstractDataProxyPrivate *d, QObject *parent = 0);

public:
    virtual ~QAbstractDataProxy();

    DataType type() const;

    void setItemLabelFormat(const QString &format);
    QString itemLabelFormat() const;

signals:
    void itemLabelFormatChanged();

protected:
    QScopedPointer<QAbstractDataProxyPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstractDataProxy)

    friend class Abstract3DController;
    friend class Bars3DController;
    friend class Scatter3DController;
    friend class Surface3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QABSTRACTDATAPROXY_H

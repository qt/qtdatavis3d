/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#ifndef QABSTRACTDATAPROXY_H
#define QABSTRACTDATAPROXY_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QAbstractDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QAbstractDataProxy : public QObject
{
    Q_OBJECT
    Q_ENUMS(DataType)
    Q_PROPERTY(DataType type READ type CONSTANT)

public:
    enum DataType {
        DataTypeNone = 0,
        DataTypeBar = 1,
        DataTypeScatter = 2,
        DataTypeSurface = 4
    };

protected:
    explicit QAbstractDataProxy(QAbstractDataProxyPrivate *d, QObject *parent = nullptr);

public:
    virtual ~QAbstractDataProxy();

    DataType type() const;

protected:
    QScopedPointer<QAbstractDataProxyPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstractDataProxy)

    friend class QAbstract3DSeriesPrivate;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

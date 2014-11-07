/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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
    explicit QAbstractDataProxy(QAbstractDataProxyPrivate *d, QObject *parent = 0);

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

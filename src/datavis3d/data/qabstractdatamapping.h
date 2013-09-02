/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#ifndef QABSTRACTDATAMAPPING_H
#define QABSTRACTDATAMAPPING_H

#include <QtDataVis3D/qdatavis3denums.h>
#include <QtDataVis3D/qabstractdataproxy.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QAbstractDataMappingPrivate;

class QT_DATAVIS3D_EXPORT QAbstractDataMapping : public QObject
{
    Q_OBJECT

public:
    explicit QAbstractDataMapping(QAbstractDataMappingPrivate *d, QObject *parent = 0);
    virtual ~QAbstractDataMapping();

signals:
    void mappingChanged();

private:
    QScopedPointer<QAbstractDataMappingPrivate> d_ptr;

    Q_DISABLE_COPY(QAbstractDataMapping)

    friend class QItemModelBarDataMapping;
    friend class QItemModelScatterDataMapping;
};


QT_DATAVIS3D_END_NAMESPACE

#endif

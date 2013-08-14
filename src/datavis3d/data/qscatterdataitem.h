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

#ifndef QSCATTERDATAITEM_H
#define QSCATTERDATAITEM_H

#include <QtDataVis3D/qdatavis3dnamespace.h>
#include <QVector3D>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QScatterDataItemPrivate;

class QT_DATAVIS3D_EXPORT QScatterDataItem
{
public:
    QScatterDataItem();
    QScatterDataItem(const QVector3D &position);
    QScatterDataItem(const QScatterDataItem &other);
    ~QScatterDataItem();

    QScatterDataItem &operator=(const QScatterDataItem &other);

    void setPosition(const QVector3D &position);
    const QVector3D &position() const;

    //void setSize(qreal size);
    //qreal size() const;

protected:
    virtual void createExtraData();

    QScatterDataItemPrivate *d_ptr;

private:
    QVector3D m_position;
    //qreal m_size;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

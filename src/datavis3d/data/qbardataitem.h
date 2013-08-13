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

#ifndef QBARDATAITEM_H
#define QBARDATAITEM_H

#include <QtDataVis3D/qdatavis3dnamespace.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QBarDataItemPrivate;

class QT_DATAVIS3D_EXPORT QBarDataItem
{
public:
    QBarDataItem();
    QBarDataItem(const QBarDataItem &other);
    ~QBarDataItem();

    QBarDataItem &operator=(const QBarDataItem &other);

    void setValue(qreal value);
    qreal value() const;

    // TODO Set color, label format, ...?

protected:
    virtual void createExtraData();

    QBarDataItemPrivate *d_ptr;

private:
    qreal m_value;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

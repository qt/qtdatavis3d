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

#ifndef QMAPDATAITEM_H
#define QMAPDATAITEM_H

#include <QtDataVis3D/qdatavis3denums.h>
#include <QtDataVis3D/qbardataitem.h>
#include <QPointF>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QMapDataItemPrivate;

class QT_DATAVIS3D_EXPORT QMapDataItem : public QBarDataItem
{
public:
    QMapDataItem();
    QMapDataItem(const QMapDataItem &other);
    ~QMapDataItem();

    QMapDataItem &operator=(const QMapDataItem &other);

    void setMapPosition(const QPointF &position);
    QPointF mapPosition() const;

    void setLabel(const QString &label);
    QString label() const;

protected:
    virtual void createExtraData();

    QMapDataItemPrivate *d_ptr;

private:
    QPointF m_mapPosition;
    QString m_label;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

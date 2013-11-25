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

#ifndef QSCATTERDATAITEM_H
#define QSCATTERDATAITEM_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QVector3D>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QScatterDataItemPrivate;

class QT_DATAVISUALIZATION_EXPORT QScatterDataItem
{
public:
    QScatterDataItem();
    QScatterDataItem(const QVector3D &position);
    QScatterDataItem(const QScatterDataItem &other);
    ~QScatterDataItem();

    QScatterDataItem &operator=(const QScatterDataItem &other);

    inline void setPosition(const QVector3D &position) { m_position = position; }
    inline QVector3D position() const { return m_position; }
    inline void setX(float value) { m_position.setX(value); }
    inline void setY(float value) { m_position.setY(value); }
    inline void setZ(float value) { m_position.setZ(value); }
    inline float x() const { return m_position.x(); }
    inline float y() const { return m_position.y(); }
    inline float z() const { return m_position.z(); }

    // TODO: Will we ever support item specific size? If not, remove.
    //void setSize(float size);
    //float size() const;

protected:
    virtual void createExtraData();

    QScatterDataItemPrivate *d_ptr;

private:
    QVector3D m_position;
    //float m_size;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

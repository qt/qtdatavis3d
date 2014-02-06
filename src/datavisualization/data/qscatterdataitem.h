/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#include <QtDataVisualization/qdatavisualizationglobal.h>

#include <QVector3D>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QScatterDataItemPrivate;

class QT_DATAVISUALIZATION_EXPORT QScatterDataItem
{
public:
    QScatterDataItem();
    QScatterDataItem(const QVector3D &position);
    QScatterDataItem(const QVector3D &position, const QVector3D &rotationAxis, float rotationAngle);
    QScatterDataItem(const QScatterDataItem &other);
    ~QScatterDataItem();

    QScatterDataItem &operator=(const QScatterDataItem &other);

    inline void setPosition(const QVector3D &position) { m_position = position; }
    inline QVector3D position() const { return m_position; }
    inline void setRotationAxis(const QVector3D &axis) { m_rotationAxis = axis; }
    inline QVector3D rotationAxis() const { return m_rotationAxis; }
    inline void setRotationAngle(float angle) { m_rotationAngle = angle; }
    inline float rotationAngle() const { return m_rotationAngle; }
    inline void setX(float value) { m_position.setX(value); }
    inline void setY(float value) { m_position.setY(value); }
    inline void setZ(float value) { m_position.setZ(value); }
    inline float x() const { return m_position.x(); }
    inline float y() const { return m_position.y(); }
    inline float z() const { return m_position.z(); }

protected:
    virtual void createExtraData();

    QScatterDataItemPrivate *d_ptr;

private:
    QVector3D m_position;
    QVector3D m_rotationAxis;
    float m_rotationAngle;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

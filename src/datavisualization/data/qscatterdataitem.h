/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef QSCATTERDATAITEM_H
#define QSCATTERDATAITEM_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtGui/QQuaternion>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QScatterDataItemPrivate;

class QT_DATAVISUALIZATION_EXPORT QScatterDataItem
{
public:
    QScatterDataItem();
    QScatterDataItem(const QVector3D &position);
    QScatterDataItem(const QVector3D &position, const QQuaternion &rotation);
    QScatterDataItem(const QScatterDataItem &other);
    ~QScatterDataItem();

    QScatterDataItem &operator=(const QScatterDataItem &other);

    inline void setPosition(const QVector3D &pos) { m_position = pos; }
    inline QVector3D position() const { return m_position; }
    inline void setRotation(const QQuaternion &rot) { m_rotation = rot; }
    inline QQuaternion rotation() const { return m_rotation; }
    inline void setX(float value) { m_position.setX(value); }
    inline void setY(float value) { m_position.setY(value); }
    inline void setZ(float value) { m_position.setZ(value); }
    inline float x() const { return m_position.x(); }
    inline float y() const { return m_position.y(); }
    inline float z() const { return m_position.z(); }

protected:
    void createExtraData();

    QScatterDataItemPrivate *d_ptr;

private:
    QVector3D m_position;
    QQuaternion m_rotation;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

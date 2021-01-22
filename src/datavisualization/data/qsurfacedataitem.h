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

#ifndef QSURFACEDATAITEM_H
#define QSURFACEDATAITEM_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QSurfaceDataItemPrivate;

class QT_DATAVISUALIZATION_EXPORT QSurfaceDataItem
{
public:
    QSurfaceDataItem();
    QSurfaceDataItem(const QVector3D &position);
    QSurfaceDataItem(const QSurfaceDataItem &other);
    ~QSurfaceDataItem();

    QSurfaceDataItem &operator=(const QSurfaceDataItem &other);

    inline void setPosition(const QVector3D &pos) { m_position = pos; }
    inline QVector3D position() const { return m_position; }
    inline void setX(float value) { m_position.setX(value); }
    inline void setY(float value) { m_position.setY(value); }
    inline void setZ(float value) { m_position.setZ(value); }
    inline float x() const { return m_position.x(); }
    inline float y() const { return m_position.y(); }
    inline float z() const { return m_position.z(); }

protected:
    void createExtraData();

    QSurfaceDataItemPrivate *d_ptr;

private:
    QVector3D m_position;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

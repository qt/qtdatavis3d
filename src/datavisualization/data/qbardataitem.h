/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef QBARDATAITEM_H
#define QBARDATAITEM_H

#include <QtDataVisualization/qdatavisualizationglobal.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QBarDataItemPrivate;

class QT_DATAVISUALIZATION_EXPORT QBarDataItem
{
public:
    QBarDataItem();
    QBarDataItem(float value);
    QBarDataItem(float value, float angle);
    QBarDataItem(const QBarDataItem &other);
    ~QBarDataItem();

    QBarDataItem &operator=(const QBarDataItem &other);

    inline void setValue(float val) { m_value = val; }
    inline float value() const { return m_value; }
    inline void setRotation(float angle) { m_angle = angle; }
    inline float rotation() const { return m_angle; }

protected:
    void createExtraData();

    QBarDataItemPrivate *d_ptr;

private:
    float m_value;
    float m_angle;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

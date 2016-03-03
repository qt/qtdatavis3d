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

#ifndef Q3DLIGHT_H
#define Q3DLIGHT_H

#include <QtDataVisualization/q3dobject.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DLightPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DLight : public Q3DObject
{
    Q_OBJECT

public:
    explicit Q3DLight(QObject *parent = Q_NULLPTR);
    virtual ~Q3DLight();

private:
    QScopedPointer<Q3DLightPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DLight)

    friend class Q3DLightPrivate;
    friend class Q3DScenePrivate;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

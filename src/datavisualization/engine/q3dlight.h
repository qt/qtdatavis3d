/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
******************************************************************************/

#ifndef Q3DLIGHT_H
#define Q3DLIGHT_H

#include <QtDataVisualization/q3dobject.h>

QT_BEGIN_NAMESPACE

class Q3DLightPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DLight : public Q3DObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoPosition READ isAutoPosition WRITE setAutoPosition NOTIFY autoPositionChanged REVISION 1)

public:
    explicit Q3DLight(QObject *parent = nullptr);
    virtual ~Q3DLight();

    void setAutoPosition(bool enabled);
    bool isAutoPosition();

Q_SIGNALS:
    Q_REVISION(1) void autoPositionChanged(bool autoPosition);

private:
    QScopedPointer<Q3DLightPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DLight)

    friend class Q3DLightPrivate;
    friend class Q3DScenePrivate;
};

QT_END_NAMESPACE

#endif

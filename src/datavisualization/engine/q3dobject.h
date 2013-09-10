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

#ifndef Q3DOBJECT_H
#define Q3DOBJECT_H

#include <QObject>
#include "datavisualizationglobal_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DScene;
class Q3DObjectPrivate;

class Q3DObject : public QObject
{
    Q_OBJECT

public:
    Q3DObject(QObject *parent = 0);
    virtual ~Q3DObject();

    void copyValuesFrom(const Q3DObject &source);

    virtual void setParentScene(Q3DScene *parentScene);
    virtual Q3DScene *parentScene();

    virtual void setPosition(const QVector3D &position);
    virtual QVector3D position() const;

private:
    QScopedPointer<Q3DObjectPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DObject)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // Q3DOBJECT_H

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

#ifndef Q3DOBJECT_H
#define Q3DOBJECT_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtCore/QObject>
#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DObjectPrivate;
class Q3DScene;

class QT_DATAVISUALIZATION_EXPORT Q3DObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Q3DScene* parentScene READ parentScene)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)

public:
    Q3DObject(QObject *parent = 0);
    virtual ~Q3DObject();

    virtual void copyValuesFrom(const Q3DObject &source);

    Q3DScene *parentScene();

    QVector3D position() const;
    void setPosition(const QVector3D &position);

Q_SIGNALS:
    void positionChanged(const QVector3D &position);

protected:
    void setDirty(bool dirty);
    bool isDirty() const;

private:
    QScopedPointer<Q3DObjectPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DObject)

    friend class Q3DScenePrivate;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

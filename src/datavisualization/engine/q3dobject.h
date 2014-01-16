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

#include <QtDataVisualization/q3dscene.h>

#include <QObject>
#include <QVector3D>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DObjectPrivate;

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

signals:
    void positionChanged(QVector3D position);

protected:
    void setDirty(bool dirty);
    bool isDirty() const;

private:
    QScopedPointer<Q3DObjectPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DObject)

    friend class Q3DScenePrivate;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif // Q3DOBJECT_H

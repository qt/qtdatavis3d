/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include "q3dobject.h"
#include "q3dobject_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

Q3DObject::Q3DObject(QObject *parent) :
    QObject(parent),
    d_ptr(new Q3DObjectPrivate(this))
{
}

Q3DObject::~Q3DObject()
{
}

void Q3DObject::copyValuesFrom(const Q3DObject &source)
{
    d_ptr->m_position.setX(source.d_ptr->m_position.x());
    d_ptr->m_position.setY(source.d_ptr->m_position.y());
    d_ptr->m_position.setZ(source.d_ptr->m_position.z());
}

void Q3DObject::setParentScene(Q3DScene *parentScene)
{
    d_ptr->m_parentScene = parentScene;
}

Q3DScene *Q3DObject::parentScene()
{
    return d_ptr->m_parentScene;
}

void Q3DObject::setPosition(const QVector3D &position)
{
    d_ptr->m_position = position;
}

QVector3D Q3DObject::position() const
{
    return d_ptr->m_position;
}


Q3DObjectPrivate::Q3DObjectPrivate(Q3DObject *q) :
    q_ptr(q),
    m_parentScene(0)
{
}

Q3DObjectPrivate::~Q3DObjectPrivate()
{

}

QT_DATAVIS3D_END_NAMESPACE

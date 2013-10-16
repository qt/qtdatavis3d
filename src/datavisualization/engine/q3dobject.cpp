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

#include "q3dobject.h"
#include "q3dobject_p.h"
#include "q3dscene.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
   \class Q3DObject
   \inmodule QtDataVisualization
   \brief Simple baseclass for all the objects in the 3D scene.
   \since Qt Data Visualization 1.0

    Q3DObject is a baseclass that contains only position information for an object in 3D scene.
    The object is considered to be a single point in the coordinate space without dimensions.
*/

/*!
 * Constructs a new 3D object with position set to origo by default. An
 * optional \a parent parameter can be given and is then passed to QObject constructor.
 */
Q3DObject::Q3DObject(QObject *parent) :
    QObject(parent),
    d_ptr(new Q3DObjectPrivate(this))
{
}

/*!
 *  Destroys the 3D object.
 */
Q3DObject::~Q3DObject()
{
}

/*!
 * Copies the 3D object position from the given \a source 3D object to this 3D object instance.
 */
void Q3DObject::copyValuesFrom(const Q3DObject &source)
{
    d_ptr->m_position.setX(source.d_ptr->m_position.x());
    d_ptr->m_position.setY(source.d_ptr->m_position.y());
    d_ptr->m_position.setZ(source.d_ptr->m_position.z());
    setDirty(true);
}

/*!
 * \property Q3DObject::parentScene
 *
 * This property contains the parent scene as read only value.
 * If the object has no parent scene the value is 0.
 */
Q3DScene *Q3DObject::parentScene()
{
    return qobject_cast<Q3DScene *>(parent());
}

/*!
 * \property Q3DObject::position
 *
 * This property contains the 3D position of the object.
 */
QVector3D Q3DObject::position() const
{
    return d_ptr->m_position;
}

void Q3DObject::setPosition(const QVector3D &position)
{
    if (d_ptr->m_position != position) {
        d_ptr->m_position = position;
        setDirty(true);
        emit positionChanged(d_ptr->m_position);
    }
}

/*!
 * Sets and clears the \a dirty flag that is used to track
 * when the 3D object has changed since last update.
 */
void Q3DObject::setDirty(bool dirty)
{
    d_ptr->m_isDirty = dirty;
}

/*!
 * \return flag that indicates if the 3D object has changed.
 */
bool Q3DObject::isDirty() const
{
    return d_ptr->m_isDirty;
}

Q3DObjectPrivate::Q3DObjectPrivate(Q3DObject *q) :
    q_ptr(q),
    m_isDirty(true)
{
}

Q3DObjectPrivate::~Q3DObjectPrivate()
{

}

QT_DATAVISUALIZATION_END_NAMESPACE

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

#include "datavis3dglobal_p.h"
#include "q3dbox.h"
#include <QtGui/QMatrix4x4>
#include <QtCore/QList>

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
    \class Q3DBox
    \brief The Q3DBox class represents an axis-aligned box in 3D space.

    Q3DBox can be used to represent the bounding box of objects in a 3D
    scene so that they can be easily culled if they are out of view.

    The sides of the box are always aligned with the x, y, and z axes of
    the world co-ordinate system.  Transforming a box with transformed()
    will result in the smallest axis-aligned bounding box that contains
    the transformed box.

    Boxes may be null, finite, or infinite.  A null box does not occupy
    any space and does not intersect with any other box.  A finite
    box consists of a minimum() and maximum() extent in 3D space.
    An infinite box encompasses all points in 3D space.

    The extents of a finite box are also included within the box.
    A box with minimum() and maximum() set to the same value
    contains a single point.
*/

/*!
    \fn Q3DBox::Q3DBox()

    Constructs a null box in 3D space.

    \sa isNull()
*/

/*!
    \fn Q3DBox::Q3DBox(const QVector3D& corner1, const QVector3D& corner2)

    Constructs a finite box in 3D space from \a corner1 to \a corner2.
    The minimum() and maximum() co-ordinates of the new box are set
    to the minimum and maximum x, y, and z values from \a corner1 and
    \a corner2.  The \a corner1 and \a corner2 values can be any two
    opposite corners that define the box.

    \sa isFinite(), minimum(), maximum()
*/

/*!
    \fn bool Q3DBox::isNull() const

    Returns true if this box is null; false otherwise.

    \sa isFinite(), isInfinite(), setToNull()
*/

/*!
    \fn bool Q3DBox::isFinite() const

    Returns true if this box is finite in size; false otherwise.

    \sa isNull(), isInfinite(), setExtents()
*/

/*!
    \fn bool Q3DBox::isInfinite() const

    Returns true if this box is infinite in size; false otherwise.

    \sa isNull(), isFinite(), setToInfinite()
*/

/*!
    \fn QVector3D Q3DBox::minimum() const

    Returns the minimum corner of this box.

    \sa maximum(), setExtents()
*/

/*!
    \fn QVector3D Q3DBox::maximum() const

    Returns the maximum corner of this box.

    \sa minimum(), setExtents()
*/

/*!
    \fn void Q3DBox::setExtents(const QVector3D& corner1, const QVector3D& corner2)

    Sets the extents of this box to a finite region from \a corner1 to
    \a corner2.  The minimum() and maximum() co-ordinates of the box are
    set to the minimum and maximum x, y, and z values from \a corner1 and
    \a corner2.  The \a corner1 and \a corner2 values can be any two
    opposite corners that define the box.

    \sa minimum(), maximum()
*/

/*!
    \fn void Q3DBox::setToNull()

    Sets this box to null.

    \sa isNull()
*/

/*!
    \fn void Q3DBox::setToInfinite()

    Sets this box to be infinite in size.

    \sa isInfinite()
*/

/*!
    \fn QVector3D Q3DBox::size() const

    Returns the finite size of this box.  If this box is null or
    infinite, the returned value will be zero.

    \sa center(), isNull(), isInfinite()
*/

/*!
    \fn QVector3D Q3DBox::center() const

    Returns the finite center of this box.  If this box is null
    or infinite, the returned value will be zero.

    \sa size(), isNull(), isInfinite()
*/

/*!
    \fn bool Q3DBox::contains(const QVector3D& point) const

    Returns true if this box contains \a point; false otherwise.
    Null boxes do not contain any points and infinite boxes contain
    all points.

    Containment is not a strict test: the point is contained if it
    lies on one of the faces of the box.

    \sa intersects()
*/

/*!
    \fn bool Q3DBox::contains(const Q3DBox& box) const

    Returns true if this box completely contains \a box.  If this box
    is null, then it will not contain \a box.  If this box is infinite,
    and \a box is not null, then \a box will be contained within this box.
    If \a box is infinite, then this box must also be infinite to contain it.

    \sa intersects()
*/

/*!
    Returns true if \a box intersects this box; false otherwise.

    \sa intersect(), intersected(), contains()
*/
bool Q3DBox::intersects(const Q3DBox& box) const
{
    if (boxtype == Null)
        return false;
    else if (boxtype == Infinite)
        return box.boxtype != Null;
    else if (box.boxtype == Null)
        return false;
    else if (box.boxtype == Infinite)
        return true;

    if (maxcorner.x() < box.mincorner.x())
        return false;
    if (mincorner.x() > box.maxcorner.x())
        return false;

    if (maxcorner.y() < box.mincorner.y())
        return false;
    if (mincorner.y() > box.maxcorner.y())
        return false;

    if (maxcorner.z() < box.mincorner.z())
        return false;
    if (mincorner.z() > box.maxcorner.z())
        return false;

    return true;
}

/*!
    Intersects this box with \a box.

    \sa intersected(), intersects(), unite()
*/
void Q3DBox::intersect(const Q3DBox& box)
{
    // Handle the simple cases first.
    if (boxtype == Null) {
        // Null intersected with anything is null.
        return;
    } else if (boxtype == Infinite) {
        // Infinity intersected with a box is that box.
        *this = box;
        return;
    } else if (box.boxtype == Null) {
        // Anything intersected with null is null.
        setToNull();
        return;
    } else if (box.boxtype == Infinite) {
        // Box intersected with infinity is the box.
        return;
    }

    // Intersect two finite boxes.
    QVector3D min1 = mincorner;
    QVector3D max1 = maxcorner;
    QVector3D min2 = box.mincorner;
    QVector3D max2 = box.maxcorner;
    if (min2.x() > min1.x())
        min1.setX(min2.x());
    if (min2.y() > min1.y())
        min1.setY(min2.y());
    if (min2.z() > min1.z())
        min1.setZ(min2.z());
    if (max2.x() < max1.x())
        max1.setX(max2.x());
    if (max2.y() < max1.y())
        max1.setY(max2.y());
    if (max2.z() < max1.z())
        max1.setZ(max2.z());
    if (min1.x() > max1.x() || min1.y() > max1.y() || min1.z() > max1.z()) {
        setToNull();
    } else {
        mincorner = min1;
        maxcorner = max1;
    }
}

/*!
    Returns a new box which is the intersection of this box with \a box.

    \sa intersect(), intersects(), united()
*/
Q3DBox Q3DBox::intersected(const Q3DBox& box) const
{
    Q3DBox result(*this);
    result.intersect(box);
    return result;
}

/*!
    Unites this box with \a point by expanding it to encompass \a point.
    If \a point is already contained within this box, then this box
    will be unchanged.

    \sa united(), intersect()
*/
void Q3DBox::unite(const QVector3D& point)
{
    if (boxtype == Finite) {
        if (point.x() < mincorner.x())
            mincorner.setX(point.x());
        else if (point.x() > maxcorner.x())
            maxcorner.setX(point.x());
        if (point.y() < mincorner.y())
            mincorner.setY(point.y());
        else if (point.y() > maxcorner.y())
            maxcorner.setY(point.y());
        if (point.z() < mincorner.z())
            mincorner.setZ(point.z());
        else if (point.z() > maxcorner.z())
            maxcorner.setZ(point.z());
    } else if (boxtype == Null) {
        boxtype = Finite;
        mincorner = point;
        maxcorner = point;
    }
}

/*!
    Unites this box with \a box by expanding this box to encompass the
    region defined by \a box.  If \a box is already contained within
    this box, then this box will be unchanged.

    \sa united(), intersect()
*/
void Q3DBox::unite(const Q3DBox& box)
{
    if (box.boxtype == Finite) {
        unite(box.minimum());
        unite(box.maximum());
    } else if (box.boxtype == Infinite) {
        setToInfinite();
    }
}

/*!
    Returns a new box which unites this box with \a point.  The returned
    value will be the smallest box that contains both this box and \a point.

    \sa unite(), intersected()
*/
Q3DBox Q3DBox::united(const QVector3D& point) const
{
    if (boxtype == Finite) {
        Q3DBox result(*this);
        result.unite(point);
        return result;
    } else if (boxtype == Null) {
        return Q3DBox(point, point);
    } else {
        return *this;
    }
}

/*!
    Returns a new box which unites this box with \a box.  The returned value
    will be the smallest box that contains both this box and \a box.

    \sa unite(), intersected()
*/
Q3DBox Q3DBox::united(const Q3DBox& box) const
{
    if (boxtype == Finite) {
        Q3DBox result(*this);
        result.unite(box);
        return result;
    } else if (boxtype == Null) {
        return box;
    } else {
        return *this;
    }
}

/*!
    Transforms this box according to \a matrix.  Each of the 8 box
    corners are transformed and then a new box that encompasses all
    of the transformed corner values is created.

    \sa transformed()
*/
void Q3DBox::transform(const QMatrix4x4& matrix)
{
    *this = transformed(matrix);
}

/*!
    Returns this box transformed by \a matrix.  Each of the 8 box
    corners are transformed and then a new box that encompasses all
    of the transformed corner values is returned.

    \sa transform()
*/
Q3DBox Q3DBox::transformed(const QMatrix4x4& matrix) const
{
    if (boxtype != Finite)
        return *this;
    Q3DBox result;
    result.unite(matrix * mincorner);
    result.unite(matrix * QVector3D(mincorner.x(), mincorner.y(), maxcorner.z()));
    result.unite(matrix * QVector3D(mincorner.x(), maxcorner.y(), maxcorner.z()));
    result.unite(matrix * QVector3D(mincorner.x(), maxcorner.y(), mincorner.z()));
    result.unite(matrix * QVector3D(maxcorner.x(), mincorner.y(), mincorner.z()));
    result.unite(matrix * QVector3D(maxcorner.x(), maxcorner.y(), mincorner.z()));
    result.unite(matrix * QVector3D(maxcorner.x(), mincorner.y(), maxcorner.z()));
    result.unite(matrix * maxcorner);
    return result;
}

/*!
    \fn bool Q3DBox::operator==(const Q3DBox& box) const

    Returns true if this box is identical to \a box.
*/

/*!
    \fn bool Q3DBox::operator!=(const Q3DBox& box) const

    Returns true if this box is not identical to \a box.
*/

/*!
    \fn bool qFuzzyCompare(const Q3DBox& box1, const Q3DBox& box2)
    \relates Q3DBox

    Returns true if \a box1 and \a box2 are almost equal; false otherwise.
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const Q3DBox &box)
{
    if (box.isFinite()) {
        dbg.nospace() << "Q3DBox(("
                      << box.minimum().x() << ", " << box.minimum().y() << ", "
                      << box.minimum().z() << ") - ("
                      << box.maximum().x() << ", " << box.maximum().y() << ", "
                      << box.maximum().z() << "))";
        return dbg.space();
    } else if (box.isNull()) {
        dbg << "Q3DBox(null)";
        return dbg;
    } else {
        dbg << "Q3DBox(infinite)";
        return dbg;
    }
}

#endif

#ifndef QT_NO_DATASTREAM

/*!
    \relates Q3DBox

    Writes the given \a box to the given \a stream and returns a
    reference to the stream.
*/
QDataStream &operator<<(QDataStream &stream, const Q3DBox &box)
{
    if (box.isNull()) {
        stream << int(0);
    } else if (box.isInfinite()) {
        stream << int(2);
    } else {
        stream << int(1);
        stream << box.minimum();
        stream << box.maximum();
    }
    return stream;
}

/*!
    \relates Q3DBox

    Reads a 3D box from the given \a stream into the given \a box
    and returns a reference to the stream.
*/
QDataStream &operator>>(QDataStream &stream, Q3DBox &box)
{
    int type;
    stream >> type;
    if (type == 1) {
        QVector3D minimum, maximum;
        stream >> minimum;
        stream >> maximum;
        box = Q3DBox(minimum, maximum);
    } else if (type == 2) {
        box.setToInfinite();
    } else {
        box.setToNull();
    }
    return stream;
}

#endif // QT_NO_DATASTREAM

QT_DATAVIS3D_END_NAMESPACE

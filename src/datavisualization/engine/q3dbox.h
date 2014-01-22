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

#ifndef Q3DBOX_H
#define Q3DBOX_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Q3DBox; // Needed to circumvent an issue with qdoc. If this line is removed, make docs will not work for this.

class QT_DATAVISUALIZATION_EXPORT Q3DBox
{
public:
    inline Q3DBox() : boxtype(Null), mincorner(0, 0, 0), maxcorner(0, 0, 0) {}
    inline Q3DBox(const QVector3D& corner1, const QVector3D& corner2): boxtype(Finite),
        mincorner(qMin(corner1.x(), corner2.x()),
                  qMin(corner1.y(), corner2.y()),
                  qMin(corner1.z(), corner2.z())),
        maxcorner(qMax(corner1.x(), corner2.x()),
                  qMax(corner1.y(), corner2.y()),
                  qMax(corner1.z(), corner2.z())) {}

    inline bool isNull() const { return (boxtype == Null); }
    inline bool isFinite() const { return (boxtype == Finite); }
    inline bool isInfinite() const { return (boxtype == Infinite); }

    inline QVector3D minimum() const { return mincorner; }
    inline QVector3D maximum() const { return maxcorner; }
    inline void setExtents(const QVector3D& corner1, const QVector3D& corner2)
    {
        boxtype = Finite;
        mincorner = QVector3D(qMin(corner1.x(), corner2.x()),
                              qMin(corner1.y(), corner2.y()),
                              qMin(corner1.z(), corner2.z()));
        maxcorner = QVector3D(qMax(corner1.x(), corner2.x()),
                              qMax(corner1.y(), corner2.y()),
                              qMax(corner1.z(), corner2.z()));
    }

    inline void setToNull()
    {
        boxtype = Null;
        mincorner = QVector3D(0, 0, 0);
        maxcorner = QVector3D(0, 0, 0);
    }

    inline void setToInfinite()
    {
        boxtype = Infinite;
        mincorner = QVector3D(0, 0, 0);
        maxcorner = QVector3D(0, 0, 0);
    }

    inline QVector3D size() const { return maxcorner - mincorner; }
    inline QVector3D center() const { return (mincorner + maxcorner) * 0.5f; }

    inline bool contains(const QVector3D& point) const
    {
        if (boxtype == Finite) {
            return (point.x() >= mincorner.x() && point.x() <= maxcorner.x() &&
                    point.y() >= mincorner.y() && point.y() <= maxcorner.y() &&
                    point.z() >= mincorner.z() && point.z() <= maxcorner.z());
        } else if (boxtype == Infinite) {
            return true;
        } else {
            return false;
        }
    }

    inline bool contains(const Q3DBox& box) const
    {
        if (box.boxtype == Finite)
            return contains(box.mincorner) && contains(box.maxcorner);
        else if (box.boxtype == Infinite)
            return (boxtype == Infinite);
        else
            return false;
    }

    bool intersects(const Q3DBox& box) const;
    void intersect(const Q3DBox& box);
    Q3DBox intersected(const Q3DBox& box) const;

    void unite(const QVector3D& point);
    void unite(const Q3DBox& box);

    Q3DBox united(const QVector3D& point) const;
    Q3DBox united(const Q3DBox& box) const;

    void transform(const QMatrix4x4& matrix);
    Q3DBox transformed(const QMatrix4x4& matrix) const;

    inline bool operator==(const Q3DBox& box) const
    {
        return (boxtype == box.boxtype &&
                mincorner == box.mincorner &&
                maxcorner == box.maxcorner);
    }

    inline bool operator!=(const Q3DBox& box) const
    {
        return (boxtype != box.boxtype ||
                mincorner != box.mincorner ||
                maxcorner != box.maxcorner);
    }

    friend bool qFuzzyCompare(const Q3DBox& box1, const Q3DBox& box2);

private:
    enum Type
    {
        Null,
        Finite,
        Infinite
    };

    Q3DBox::Type boxtype;
    QVector3D mincorner, maxcorner;
};

inline bool qFuzzyCompare(const Q3DBox& box1, const Q3DBox& box2)
{
    return box1.boxtype == box2.boxtype &&
            qFuzzyCompare(box1.mincorner, box2.mincorner) &&
            qFuzzyCompare(box1.maxcorner, box2.maxcorner);
}

#ifndef QT_NO_DEBUG_STREAM
QT_DATAVISUALIZATION_EXPORT QDebug operator<<(QDebug dbg, const Q3DBox &box);
#endif

#ifndef QT_NO_DATASTREAM
QT_DATAVISUALIZATION_EXPORT QDataStream &operator<<(QDataStream &stream, const Q3DBox &box);
QT_DATAVISUALIZATION_EXPORT QDataStream &operator>>(QDataStream &stream, Q3DBox &box);
#endif

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

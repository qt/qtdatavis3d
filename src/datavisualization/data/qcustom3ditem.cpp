/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#include "qcustom3ditem_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QCustom3DItem
 * \inmodule QtDataVisualization
 * \brief The QCustom3DItem class is for creating custom items to be added to a graph.
 * \since Qt Data Visualization 1.1
 *
 * This class is for creating custom items to be added to a graph. The item has a custom mesh,
 * position, scaling, rotation, and an optional texture.
 *
 * \sa QAbstract3DGraph::addCustomItem()
 */

/*!
 * \qmltype Custom3DItem
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.1
 * \ingroup datavisualization_qml
 * \instantiates QCustom3DItem
 * \brief The Custom3DItem type is for creating custom items to be added to a graph.
 *
 * This type is for creating custom items to be added to a graph. The item has a custom mesh,
 * position, scaling, rotation, and an optional texture.
 */

/*! \qmlproperty string Custom3DItem::meshFile
 *
 * Holds item mesh file name. Item in the file must be in Wavefront obj format and include
 * vertices, normals, and UVs. It also needs to be in triangles.
 */

/*! \qmlproperty string Custom3DItem::textureFile
 *
 * Holds the texture file name for the item. If left unset, a solid gray texture will be
 * used.
 */

// TODO: Position check in task QTRD-3057
/*! \qmlproperty vector3d Custom3DItem::position
 *
 * Holds the item \a position as a vector3d. Item position is in data coordinates. Defaults to
 * \c {vector3d(0.0, 0.0, 0.0)}.
 *
 * \note No validity checks are made for the position of the item, so it is up to the user to
 * provide a valid position. Items positioned outside axis ranges are still rendered.
 */

/*! \qmlproperty vector3d Custom3DItem::scaling
 *
 * Holds the item \a scaling as a vector3d. Defaults to \c {vector3d(0.1, 0.1, 0.1)}.
 */

/*! \qmlproperty quaternion Custom3DItem::rotation
 *
 * Holds the item \a rotation as a quaternion. Defaults to \c {quaternion(0.0, 0.0, 0.0, 0.0)}.
 */

/*!
 * \qmlmethod void Custom3DItem::setRotationAxisAndAngle(vector3d axis, real angle)
 *
 * A convenience function to construct rotation quaternion from \a axis and \a angle.
 *
 * \sa rotation
 */

/*!
 * Constructs QCustom3DItem with given \a parent.
 */
QCustom3DItem::QCustom3DItem(QObject *parent) :
    d_ptr(new QCustom3DItemPrivate(this, parent))
{
    setTextureImage(QImage());
}

/*!
 * Constructs QCustom3DItem with given \a meshFile, \a position, \a scaling,
 * \a rotation, \a texture image, and optional \a parent.
 */
QCustom3DItem::QCustom3DItem(const QString &meshFile, const QVector3D &position,
                             const QVector3D &scaling, const QQuaternion &rotation,
                             const QImage &texture, QObject *parent) :
    d_ptr(new QCustom3DItemPrivate(this, meshFile, position, scaling, rotation, parent))
{
    setTextureImage(texture);
}

/*!
 * Destroys QCustom3DItem.
 */
QCustom3DItem::~QCustom3DItem()
{
}

/*! \property QCustom3DItem::meshFile
 *
 * Holds item mesh file name. Item in the file must be in Wavefront obj format and include
 * vertices, normals, and UVs. It also needs to be in triangles.
 */
void QCustom3DItem::setMeshFile(const QString &meshFile)
{
    if (d_ptr->m_meshFile != meshFile) {
        d_ptr->m_meshFile = meshFile;
        d_ptr->m_dirtyBits.meshDirty = true;
        emit meshFileChanged(meshFile);
        emit d_ptr->needUpdate();
    }
}

QString QCustom3DItem::meshFile()
{
    return d_ptr->m_meshFile;
}

/*! \property QCustom3DItem::position
 *
 * Holds the item \a position as a QVector3D. Item position is in data coordinates. Defaults to
 * \c {QVector3D(0.0, 0.0, 0.0)}.
 *
 * \note No validity checks are made for the position of the item, so it is up to the user to
 * provide a valid position. Items positioned outside axis ranges are still rendered.
 */
void QCustom3DItem::setPosition(const QVector3D &position)
{
    if (d_ptr->m_position != position) {
        d_ptr->m_position = position;
        d_ptr->m_dirtyBits.positionDirty = true;
        emit positionChanged(position);
        emit d_ptr->needUpdate();
    }
}

QVector3D QCustom3DItem::position()
{
    return d_ptr->m_position;
}

/*! \property QCustom3DItem::scaling
 *
 * Holds the item \a scaling as a QVector3D. Defaults to \c {QVector3D(0.1, 0.1, 0.1)}.
 */
void QCustom3DItem::setScaling(const QVector3D &scaling)
{
    if (d_ptr->m_scaling != scaling) {
        d_ptr->m_scaling = scaling;
        d_ptr->m_dirtyBits.scalingDirty = true;
        emit scalingChanged(scaling);
        emit d_ptr->needUpdate();
    }
}

QVector3D QCustom3DItem::scaling()
{
    return d_ptr->m_scaling;
}

/*! \property QCustom3DItem::rotation
 *
 * Holds the item \a rotation as a QQuaternion. Defaults to \c {QQuaternion(0.0, 0.0, 0.0, 0.0)}.
 */
void QCustom3DItem::setRotation(const QQuaternion &rotation)
{
    if (d_ptr->m_rotation != rotation) {
        d_ptr->m_rotation = rotation;
        d_ptr->m_dirtyBits.rotationDirty = true;
        emit rotationChanged(rotation);
        emit d_ptr->needUpdate();
    }
}

QQuaternion QCustom3DItem::rotation()
{
    return d_ptr->m_rotation;
}

/*! \property QCustom3DItem::visible
 *
 * Sets the item \a visible. Defaults to \c{true}.
 */
void QCustom3DItem::setVisible(bool visible)
{
    if (d_ptr->m_visible != visible) {
        d_ptr->m_visible = visible;
        d_ptr->m_dirtyBits.visibleDirty = true;
        emit visibleChanged(visible);
        emit d_ptr->needUpdate();
    }
}

bool QCustom3DItem::isVisible()
{
    return d_ptr->m_visible;
}

/*!
 * A convenience function to construct rotation quaternion from \a axis and \a angle.
 *
 * \sa rotation
 */
void QCustom3DItem::setRotationAxisAndAngle(const QVector3D &axis, float angle)
{
    setRotation(QQuaternion::fromAxisAndAngle(axis, angle));
}

/*!
 * Set the \a textureImage as a QImage for the item. Texture defaults to solid gray.
 */
void QCustom3DItem::setTextureImage(const QImage &textureImage)
{
    if (textureImage != d_ptr->m_textureImage) {
        if (textureImage.isNull()) {
            // Make a solid gray texture
            d_ptr->m_textureImage = QImage(2, 2, QImage::Format_RGB32);
            d_ptr->m_textureImage.fill(Qt::gray);
        } else {
            d_ptr->m_textureImage = textureImage;
        }

        if (!d_ptr->m_textureFile.isEmpty()) {
            d_ptr->m_textureFile.clear();
            emit textureFileChanged(d_ptr->m_textureFile);
        }
        d_ptr->m_dirtyBits.textureDirty = true;
        emit d_ptr->needUpdate();
    }
}

/*! \property QCustom3DItem::textureFile
 *
 * Holds the texture file name for the item. If both this and textureImage are unset, a solid
 * gray texture will be used.
 */
void QCustom3DItem::setTextureFile(const QString &textureFile)
{
    if (d_ptr->m_textureFile != textureFile) {
        d_ptr->m_textureFile = textureFile;
        if (!textureFile.isEmpty()) {
            d_ptr->m_textureImage = QImage(textureFile);
        } else {
            d_ptr->m_textureImage = QImage(2, 2, QImage::Format_RGB32);
            d_ptr->m_textureImage.fill(Qt::gray);
        }
        emit textureFileChanged(textureFile);
        d_ptr->m_dirtyBits.textureDirty = true;
        emit d_ptr->needUpdate();
    }
}

QString QCustom3DItem::textureFile()
{
    return d_ptr->m_textureFile;
}

QCustom3DItemPrivate::QCustom3DItemPrivate(QCustom3DItem *q, QObject *parent) :
    QObject(parent),
    q_ptr(q),
    m_position(QVector3D(0.0f, 0.0f, 0.0f)),
    m_scaling(QVector3D(0.1f, 0.1f, 0.1f)),
    m_rotation(QQuaternion(0.0f, 0.0f, 0.0f, 0.0f)),
    m_visible(true)
{
}

QCustom3DItemPrivate::QCustom3DItemPrivate(QCustom3DItem *q, const QString &meshFile,
                                           const QVector3D &position, const QVector3D &scaling,
                                           const QQuaternion &rotation, QObject *parent) :
    QObject(parent),
    q_ptr(q),
    m_meshFile(meshFile),
    m_position(position),
    m_scaling(scaling),
    m_rotation(rotation),
    m_visible(true)
{
}

QCustom3DItemPrivate::~QCustom3DItemPrivate()
{
}

QImage QCustom3DItemPrivate::textureImage()
{
    return m_textureImage;
}

void QCustom3DItemPrivate::clearTextureImage()
{
    m_textureImage = QImage();
}

QT_END_NAMESPACE_DATAVISUALIZATION

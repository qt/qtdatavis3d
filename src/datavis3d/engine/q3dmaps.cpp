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

#include "q3dmaps.h"
#include "q3dmaps_p.h"
#include "maps3dcontroller_p.h"

#include <QMouseEvent>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class Q3DMaps
 * \inmodule QtDataVis3D
 * \brief The Q3DMaps class provides methods for rendering 3D bars on maps or other planes.
 * \since 1.0.0
 *
 * This class enables developers to render bars or objects on maps or other planes in 3D and to
 * view them by rotating the scene freely. Methods are provided for changing object types, themes
 * and so on.
 *
 * See methods themselves for more complete description.
 *
 * \sa Q3DBars, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * Constructs Q3DMaps.
 */
Q3DMaps::Q3DMaps()
    : d_ptr(new Q3DMapsPrivate(this, geometry()))
{
    d_ptr->m_shared->initializeOpenGL();
}

/*!
 * Destructs Q3DMaps.
 */
Q3DMaps::~Q3DMaps()
{
}

/*!
 * \internal
 */
void Q3DMaps::render()
{
    d_ptr->m_shared->render();
}

#if defined(Q_OS_ANDROID)
/*!
 * \internal
 */
void Q3DMaps::mouseDoubleClickEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseDoubleClickEvent(event);
}

/*!
 * \internal
 */
void Q3DMaps::touchEvent(QTouchEvent *event)
{
    d_ptr->m_shared->touchEvent(event);
}
#endif

/*!
 * \internal
 */
void Q3DMaps::mousePressEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mousePressEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DMaps::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseReleaseEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DMaps::mouseMoveEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseMoveEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DMaps::wheelEvent(QWheelEvent *event)
{
    d_ptr->m_shared->wheelEvent(event);
}

/*!
 * \internal
 */
void Q3DMaps::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    d_ptr->m_shared->setWidth(width());
    d_ptr->m_shared->setHeight(height());
    d_ptr->m_shared->resizeNotify();
}

// TODO: Document
// Size
void Q3DMaps::setWidth(const int width)
{
    d_ptr->m_shared->setWidth(width);
    QWindow::setWidth(width);
}

void Q3DMaps::setHeight(const int height)
{
    d_ptr->m_shared->setHeight(height);
    QWindow::setHeight(height);
}

void Q3DMaps::setBarSpecs(const QVector3D &thickness, AdjustmentDirection direction)
{
    d_ptr->m_shared->setBarSpecs(thickness, direction);
}

void Q3DMaps::setBarType(QDataVis::MeshStyle style, bool smooth)
{
    d_ptr->m_shared->setBarType(style, smooth);
}

void Q3DMaps::setMeshFileName(const QString &objFileName)
{
    d_ptr->m_shared->setMeshFileName(objFileName);
}

void Q3DMaps::setCameraPreset(QDataVis::CameraPreset preset)
{
    d_ptr->m_shared->setCameraPreset(preset);
}

void Q3DMaps::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    d_ptr->m_shared->setCameraPosition(horizontal, vertical, distance);
}

void Q3DMaps::setTheme(QDataVis::ColorTheme theme)
{
    d_ptr->m_shared->setTheme(theme);
}

void Q3DMaps::setBarColor(const QColor &baseColor, const QColor &heightColor, bool uniform)
{
    d_ptr->m_shared->setBarColor(baseColor, heightColor, uniform);
}

void Q3DMaps::setAreaSpecs(const QRect &areaRect, const QImage &image)
{
    d_ptr->m_shared->setAreaSpecs(areaRect, image);
}

void Q3DMaps::setImage(const QImage &image)
{
    d_ptr->m_shared->setImage(image);
}

void Q3DMaps::setSelectionMode(QDataVis::SelectionMode mode)
{
    d_ptr->m_shared->setSelectionMode(mode);
}

QDataVis::SelectionMode Q3DMaps::selectionMode() const
{
    return d_ptr->m_shared->selectionMode();
}

void Q3DMaps::setFont(const QFont &font)
{
    d_ptr->m_shared->setFont(font);
}

QFont Q3DMaps::font() const
{
    return d_ptr->m_shared->font();
}

void Q3DMaps::setLabelTransparency(QDataVis::LabelTransparency transparency)
{
    d_ptr->m_shared->setLabelTransparency(transparency);
}

QDataVis::LabelTransparency Q3DMaps::labelTransparency() const
{
    return d_ptr->m_shared->labelTransparency();
}

QDataVis::ShadowQuality Q3DMaps::setShadowQuality(QDataVis::ShadowQuality quality)
{
    return d_ptr->m_shared->setShadowQuality(quality);
}

QDataVis::ShadowQuality Q3DMaps::shadowQuality() const
{
    return d_ptr->m_shared->shadowQuality();
}

void Q3DMaps::setDataProxy(QMapDataProxy *proxy)
{
    d_ptr->m_shared->setDataProxy(proxy);
}

QMapDataProxy *Q3DMaps::dataProxy()
{
    return d_ptr->m_shared->dataProxy();
}

Q3DMapsPrivate::Q3DMapsPrivate(Q3DMaps *q, const QRect &rect)
    : q_ptr(q),
      m_shared(new Maps3DController(rect))
{
}

Q3DMapsPrivate::~Q3DMapsPrivate()
{
    qDebug() << "Destroying Q3DMapsPrivate";
    delete m_shared;
}

QT_DATAVIS3D_END_NAMESPACE

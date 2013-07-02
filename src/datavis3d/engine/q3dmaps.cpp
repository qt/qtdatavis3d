/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
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

void Q3DMaps::setBarType(BarStyle style, bool smooth)
{
    d_ptr->m_shared->setBarType(style, smooth);
}

void Q3DMaps::setMeshFileName(const QString &objFileName)
{
    d_ptr->m_shared->setMeshFileName(objFileName);
}

void Q3DMaps::setCameraPreset(CameraPreset preset)
{
    d_ptr->m_shared->setCameraPreset(preset);
}

void Q3DMaps::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    d_ptr->m_shared->setCameraPosition(horizontal, vertical, distance);
}

void Q3DMaps::setTheme(ColorTheme theme)
{
    d_ptr->m_shared->setTheme(theme);
}

void Q3DMaps::setBarColor(QColor baseColor, QColor heightColor, bool uniform)
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

void Q3DMaps::setSelectionMode(SelectionMode mode)
{
    d_ptr->m_shared->setSelectionMode(mode);
}

SelectionMode Q3DMaps::selectionMode()
{
    return d_ptr->m_shared->selectionMode();
}

void Q3DMaps::setWindowTitle(const QString &title)
{
    setTitle(title);
}

QString Q3DMaps::windowTitle()
{
    return title();
}

void Q3DMaps::setFontSize(float fontsize)
{
    d_ptr->m_shared->setFontSize(fontsize);
}

float Q3DMaps::fontSize()
{
    return d_ptr->m_shared->fontSize();
}

void Q3DMaps::setFont(const QFont &font)
{
    d_ptr->m_shared->setFont(font);
}

QFont Q3DMaps::font()
{
    return d_ptr->m_shared->font();
}

void Q3DMaps::setLabelTransparency(LabelTransparency transparency)
{
    d_ptr->m_shared->setLabelTransparency(transparency);
}

LabelTransparency Q3DMaps::labelTransparency()
{
    return d_ptr->m_shared->labelTransparency();
}

ShadowQuality Q3DMaps::setShadowQuality(ShadowQuality quality)
{
    return d_ptr->m_shared->setShadowQuality(quality);
}

ShadowQuality Q3DMaps::shadowQuality()
{
    return d_ptr->m_shared->shadowQuality();
}

bool Q3DMaps::addDataItem(QDataItem* dataItem)
{
    return d_ptr->m_shared->addDataItem(dataItem);
}

bool Q3DMaps::addData(const QVector<QDataItem*> &data)
{
    return d_ptr->m_shared->addData(data);
}

bool Q3DMaps::addData(const QDataRow &dataRow)
{
    return d_ptr->m_shared->addData(dataRow);
}

bool Q3DMaps::setData(const QVector<QDataItem*> &dataRow)
{
    return d_ptr->m_shared->setData(dataRow);
}

bool Q3DMaps::setData(QDataRow *dataRow)
{
    return d_ptr->m_shared->setData(dataRow);
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

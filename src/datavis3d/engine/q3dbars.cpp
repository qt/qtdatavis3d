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

#include "q3dbars.h"
#include "q3dbars_p.h"
#include "bars3dshared_p.h"

#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QScreen>
#include <QMouseEvent>

#include <qmath.h>

#include <QDebug>

QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE

Q3DBars::Q3DBars(GLuint fbohandle, const QSize &windowsize)
    : d_ptr(new Q3DBarsPrivate(this, geometry(), fbohandle))
{
}

Q3DBars::~Q3DBars()
{
}

void Q3DBars::initialize()
{
    qDebug() << "Q3DBars::initialize()";
    d_ptr->m_shared->setWidth(width());
    d_ptr->m_shared->setHeight(height());
    d_ptr->m_shared->initializeOpenGL();
}

void Q3DBars::render()
{
    d_ptr->m_shared->render();
}

#if defined(Q_OS_ANDROID)
void Q3DBars::mouseDoubleClickEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseDoubleClickEvent(event);
}

void Q3DBars::touchEvent(QTouchEvent *event)
{
    d_ptr->m_shared->touchEvent(event);
}
#endif

void Q3DBars::mousePressEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mousePressEvent(event);
}

void Q3DBars::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseReleaseEvent(event);
}

void Q3DBars::mouseMoveEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseMoveEvent(event);
}

void Q3DBars::wheelEvent(QWheelEvent *event)
{
    d_ptr->m_shared->wheelEvent(event);
}

void Q3DBars::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    d_ptr->m_shared->setWidth(width());
    d_ptr->m_shared->setHeight(height());
    d_ptr->m_shared->resizeNotify();
}

// Size
void Q3DBars::setWidth(const int width)
{
    d_ptr->m_shared->setWidth(width);
    QWindow::setWidth(width);
}

void Q3DBars::setHeight(const int height)
{
    d_ptr->m_shared->setHeight(height);
    QWindow::setHeight(height);
}

void Q3DBars::setBarSpecs(QSizeF thickness, QSizeF spacing, bool relative)
{
    d_ptr->m_shared->setBarSpecs(thickness, spacing, relative);
}

void Q3DBars::setBarType(BarStyle style, bool smooth)
{
    d_ptr->m_shared->setBarType(style, smooth);
}

void Q3DBars::setupSampleSpace(int samplesRow, int samplesColumn, const QString &labelRow,
                               const QString &labelColumn, const QString &labelHeight)
{
    d_ptr->m_shared->setupSampleSpace(samplesRow, samplesColumn, labelRow, labelColumn, labelHeight);
}


void Q3DBars::setCameraPreset(CameraPreset preset)
{
    d_ptr->m_shared->setCameraPreset(preset);
}

void Q3DBars::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    d_ptr->m_shared->setCameraPosition(horizontal, vertical, distance);
}

void Q3DBars::setTheme(ColorTheme theme)
{
    d_ptr->m_shared->setTheme(theme);
}

void Q3DBars::setBarColor(QColor baseColor, QColor heightColor, QColor depthColor, bool uniform)
{
    d_ptr->m_shared->setBarColor(baseColor, heightColor, depthColor, uniform);
}

void Q3DBars::setSelectionMode(SelectionMode mode)
{
    d_ptr->m_shared->setSelectionMode(mode);
}

SelectionMode Q3DBars::selectionMode()
{
    return d_ptr->m_shared->selectionMode();
}

void Q3DBars::setWindowTitle(const QString &title)
{
    setTitle(title);
}

QString Q3DBars::windowTitle()
{
    return title();
}

void Q3DBars::setMeshFileName(const QString &objFileName)
{
    d_ptr->m_shared->setMeshFileName(objFileName);
}

void Q3DBars::setFontSize(float fontsize)
{
    d_ptr->m_shared->setFontSize(fontsize);
}

float Q3DBars::fontSize()
{
    return d_ptr->m_shared->fontSize();
}

void Q3DBars::setFont(const QFont &font)
{
    d_ptr->m_shared->setFont(font);
}

QFont Q3DBars::font()
{
    return d_ptr->m_shared->font();
}

void Q3DBars::setLabelTransparency(LabelTransparency transparency)
{
    d_ptr->m_shared->setLabelTransparency(transparency);
}

LabelTransparency Q3DBars::labelTransparency()
{
    return d_ptr->m_shared->labelTransparency();
}

void Q3DBars::setGridEnabled(bool enable)
{
    d_ptr->m_shared->setGridEnabled(enable);
}

bool Q3DBars::gridEnabled()
{
    return d_ptr->m_shared->gridEnabled();
}

void Q3DBars::setBackgroundEnabled(bool enable)
{
    d_ptr->m_shared->setBackgroundEnabled(enable);
}

bool Q3DBars::backgroundEnabled()
{
    return d_ptr->m_shared->backgroundEnabled();
}

void Q3DBars::setShadowQuality(ShadowQuality quality)
{
    d_ptr->m_shared->setShadowQuality(quality);
}

ShadowQuality Q3DBars::shadowQuality()
{
    return d_ptr->m_shared->shadowQuality();
}

void Q3DBars::setTickCount(GLint tickCount, GLfloat step, GLfloat minimum)
{
    d_ptr->m_shared->setTickCount(tickCount, step, minimum);
}

void Q3DBars::addDataRow(const QVector<float> &dataRow, const QString &labelRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_shared->addDataRow(dataRow, labelRow, labelsColumn);
}

void Q3DBars::addDataRow(const QVector<QDataItem*> &dataRow, const QString &labelRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_shared->addDataRow(dataRow, labelRow, labelsColumn);
}

void Q3DBars::addDataRow(QDataRow *dataRow)
{
    d_ptr->m_shared->addDataRow(dataRow);
}

void Q3DBars::addDataSet(const QVector< QVector<float> > &data, const QVector<QString> &labelsRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_shared->addDataSet(data, labelsRow,labelsColumn);
}

void Q3DBars::addDataSet(const QVector< QVector<QDataItem*> > &data,
                         const QVector<QString> &labelsRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_shared->addDataSet(data, labelsRow, labelsColumn);
}

void Q3DBars::addDataSet(QDataSet* dataSet)
{
    d_ptr->m_shared->addDataSet(dataSet);
}

Q3DBarsPrivate::Q3DBarsPrivate(Q3DBars *q, QRect rect, GLuint fbohandle)
    : q_ptr(q),
      m_shared(new Bars3dShared(rect, fbohandle))
{
}

Q3DBarsPrivate::~Q3DBarsPrivate()
{
    qDebug() << "Destroying Q3DBarsPrivate";
    delete m_shared;
}

QTENTERPRISE_DATAVIS3D_END_NAMESPACE

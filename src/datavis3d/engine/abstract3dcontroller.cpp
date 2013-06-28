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
#include "abstract3dcontroller_p.h"
#include "camerahelper_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

Abstract3DController::Abstract3DController(QRect boundRect, QObject *parent) :
    QObject(parent),
    m_boundingRect(boundRect.x(), boundRect.y(), boundRect.width(), boundRect.height()),
    m_cameraHelper(new CameraHelper()),
    m_horizontalRotation(-45.0f),
    m_verticalRotation(15.0f),
    m_zoomLevel(100),
    m_theme(),
    m_shadowQuality(ShadowLow),
    m_labelTransparency(TransparencyFromTheme)

{
    m_theme.useColorTheme(ThemeSystem);
}

Abstract3DController::~Abstract3DController()
{
    delete m_cameraHelper;
}

void Abstract3DController::setSize(const int width, const int height)
{
    m_boundingRect.setWidth(width);
    m_boundingRect.setHeight(height);

    //qDebug() << "Abstract3DController::setSize " << m_boundingRect.width() << "x" <<m_boundingRect.height();
    emit boundingRectChanged(m_boundingRect);
}

const QSize Abstract3DController::size()
{
    return m_boundingRect.size();
}

const QRect Abstract3DController::boundingRect()
{
    return m_boundingRect;
}

void Abstract3DController::setBoundingRect(const QRect boundingRect)
{
    m_boundingRect = boundingRect;

    //qDebug() << "Abstract3DController::setBoundingRect " << m_boundingRect.width() << "x" <<m_boundingRect.height();
    emit boundingRectChanged(m_boundingRect);
}

void Abstract3DController::setWidth(const int width)
{
    m_boundingRect.setWidth(width);
    emit sizeChanged(m_boundingRect);
}

int Abstract3DController::width()
{
    return m_boundingRect.width();
}

void Abstract3DController::setHeight(const int height)
{
    m_boundingRect.setHeight(height);
    emit sizeChanged(m_boundingRect);
}

int Abstract3DController::height()
{
    return m_boundingRect.height();
}

void Abstract3DController::setX(const int x)
{
    m_boundingRect.setX(x);
    emit positionChanged(m_boundingRect);
}

int Abstract3DController::x()
{
    return m_boundingRect.x();
}

void Abstract3DController::setY(const int y)
{
    m_boundingRect.setY(y);
    emit positionChanged(m_boundingRect);
}

int Abstract3DController::y()
{
    return m_boundingRect.y();
}

int Abstract3DController::zoomLevel()
{
    return m_zoomLevel;
}

void Abstract3DController::setZoomLevel(int zoomLevel)
{
    m_zoomLevel = zoomLevel;
    emit zoomLevelChanged(zoomLevel);
}

void Abstract3DController::setCameraPreset(CameraPreset preset)
{
    m_cameraHelper->setCameraPreset(preset);
}

void Abstract3DController::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    m_horizontalRotation = qBound(-180.0f, horizontal, 180.0f);
    m_verticalRotation = qBound(0.0f, vertical, 90.0f);
    m_zoomLevel = qBound(10, distance, 500);
    m_cameraHelper->setCameraRotation(QPointF(m_horizontalRotation,
                                              m_verticalRotation));
    //qDebug() << "camera rotation set to" << m_horizontalRotation << m_verticalRotation;
}

void Abstract3DController::setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                                   bool uniform)
{
    m_theme.m_baseColor = baseColor;
    m_theme.m_heightColor = heightColor;
    m_theme.m_depthColor = depthColor;
    m_theme.m_uniformColor = uniform;

    emit themeChanged(m_theme);
}

void Abstract3DController::setColorTheme(ColorTheme colorTheme)
{
    m_theme.useColorTheme(colorTheme);
    emit themeChanged(m_theme);
}

Theme Abstract3DController::theme()
{
    return m_theme;
}

void Abstract3DController::setShadowQuality(ShadowQuality quality)
{
    m_shadowQuality = quality;
    emit shadowQualityChanged(m_shadowQuality);
}

ShadowQuality Abstract3DController::shadowQuality()
{
    return m_shadowQuality;
}


void Abstract3DController::setLabelTransparency(LabelTransparency transparency)
{
    m_labelTransparency = transparency;
    emit labelTransparencyUpdated(m_labelTransparency);
}

LabelTransparency Abstract3DController::labelTransparency()
{
    return m_labelTransparency;
}

QT_DATAVIS3D_END_NAMESPACE

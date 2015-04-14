/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include "colorgradient_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

ColorGradientStop::ColorGradientStop(QObject *parent)
    : QObject(parent)
{
}

qreal ColorGradientStop::position() const
{
    return m_position;
}

void ColorGradientStop::setPosition(qreal position)
{
    m_position = position;
    updateGradient();
    emit positionChanged(position);
}

QColor ColorGradientStop::color() const
{
    return m_color;
}

void ColorGradientStop::setColor(const QColor &color)
{
    m_color = color;
    updateGradient();
    emit colorChanged(color);
}

void ColorGradientStop::updateGradient()
{
    if (ColorGradient *grad = qobject_cast<ColorGradient*>(parent()))
        grad->doUpdate();
}

ColorGradient::ColorGradient(QObject *parent)
: QObject(parent)
{
}

ColorGradient::~ColorGradient()
{
}

QQmlListProperty<ColorGradientStop> ColorGradient::stops()
{
    return QQmlListProperty<ColorGradientStop>(this, m_stops);
}

void ColorGradient::doUpdate()
{
    emit updated();
}

QT_END_NAMESPACE_DATAVISUALIZATION

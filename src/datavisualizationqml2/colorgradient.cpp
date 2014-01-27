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

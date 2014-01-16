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

#include "declarativecolor_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

DeclarativeColor::DeclarativeColor(QObject *parent)
    : QObject(parent)
{
}

void DeclarativeColor::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged(color);
    }
}

QColor DeclarativeColor::color() const
{
    return m_color;
}

QT_END_NAMESPACE_DATAVISUALIZATION

/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef STAR_H
#define STAR_H

#include <QtCore/qglobal.h>
#include <QtGui/QVector2D>

class Star
{
public:
    Star();
    const void calcXY();

    qreal m_theta;    // position auf der ellipse
//    qreal m_velTheta; // angular velocity
    qreal m_angle;    // Schräglage der Ellipse
    qreal m_a;        // kleine halbachse
    qreal m_b;        // große halbachse
//    qreal m_temp;     // star temperature
//    qreal m_mag;      // brightness;
    QVector2D  m_center;   // center of the elliptical orbit
//    QVector2D  m_vel;      // Current velocity (calculated)
    QVector2D  m_pos;      // current position in kartesion koordinates
};

#endif // STAR_H

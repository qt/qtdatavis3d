/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

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

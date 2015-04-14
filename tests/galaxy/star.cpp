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

#include "star.h"

#include <QtCore/qmath.h>

static const double DEG_TO_RAD = M_PI / 180.0;

Star::Star()
    : m_theta(0),
      m_a(0),
      m_b(0),
      m_center(QVector2D(0.0f, 0.0f))
{
}

const void Star::calcXY()
{
    qreal &a = m_a;
    qreal &b = m_b;
    qreal &theta = m_theta;
    const QVector2D &p = m_center;

    qreal beta  = -m_angle;
    qreal alpha = theta * DEG_TO_RAD;

    // temporaries to save cpu time
    qreal cosalpha = qCos(alpha);
    qreal sinalpha = qSin(alpha);
    qreal cosbeta  = qCos(beta);
    qreal sinbeta  = qSin(beta);

    m_pos = QVector2D(p.x() + (a * cosalpha * cosbeta - b * sinalpha * sinbeta),
                      p.y() + (a * cosalpha * sinbeta + b * sinalpha * cosbeta));
}

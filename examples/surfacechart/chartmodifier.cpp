/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "chartmodifier.h"

#include <qmath.h>

#include <QDebug>

QT_DATAVIS3D_USE_NAMESPACE

ChartModifier::ChartModifier(Q3DSurface *chart)
    : m_chart(chart),
      m_xCount(10),
      m_zCount(10)
{
}

ChartModifier::~ChartModifier()
{
    delete m_chart;
}

void ChartModifier::toggleSmooth(bool enabled)
{
    qDebug() << "ChartModifier::toggleSmooth " << enabled;
    m_chart->setSmoothSurface(enabled);
}

void ChartModifier::toggleSurfaceGrid(bool enable)
{
    qDebug() << "ChartModifier::toggleSurfaceGrid" << enable;
    m_chart->setSurfaceGrid(enable);
}

void ChartModifier::toggleSqrtSin(bool enable)
{
    qreal biggest = -9999.0;
    qreal smallest = 9999.0;
    QList<qreal> series;

    if (enable) {
        qDebug() << "Create Sqrt&Sin surface, (" << m_xCount << ", " << m_zCount << ")";

        qreal stepZ = 16.0 / qreal(m_zCount);
        qreal stepX = 16.0 / qreal(m_xCount);

        for (qreal i = -8.0 + stepZ / 2.0 ; i < 8.0 ; i += stepZ) {
            for (qreal j = -8.0 + stepX / 2.0; j < 8.0; j += stepX) {
                qreal R = qSqrt(i*i + j*j) + 0.01;
                qreal y = (sin(R)/R + 0.24) * 1.61;
                series << y;
                if (y > biggest) biggest = y;
                if (y < smallest) smallest = y;
            }
        }

        m_chart->setTickCount(4, 0.5f);
        m_chart->appendSeries(series, m_xCount, m_zCount);

        qDebug() << "biggest = " << biggest << ", smallest = " << smallest;
    } else {
        qDebug() << "Remove surface";
    }
}

void ChartModifier::togglePlane(bool enable)
{
    qDebug() << "ChartModifier::togglePlane " << enable;
    if (enable) {
        QList<qreal> series;

        qreal y = 2.0 / qreal(m_xCount - 1);
        for (int i = 0; i < m_zCount; i++) {
            for (int j = 0; j < m_xCount; j++) {
                series << j * y;
            }
        }

        m_chart->setTickCount(4, 0.5f);
        m_chart->appendSeries(series, m_xCount, m_zCount);
    }
}

void ChartModifier::toggleGridSliderLock(bool enable)
{
    m_gridSlidersLocked = enable;
    if (m_gridSlidersLocked) {
        m_gridSliderZ->setEnabled(false);
        m_gridSliderZ->setValue(m_gridSliderX->value());
    } else {
        m_gridSliderZ->setEnabled(true);
    }
}

void ChartModifier::adjustXCount(int count)
{
    m_xCount = count;
    if (m_gridSlidersLocked)
        m_gridSliderZ->setValue(count);

    qDebug() << "X count = " << count;
}

void ChartModifier::adjustZCount(int count)
{
    m_zCount = count;

    qDebug() << "Z count = " << count;
}


/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include "chartmodifier.h"
#include <Q3DValueAxis>
#include <QSurfaceDataProxy>

#include <qmath.h>

#include <QDebug>

QT_DATAVIS3D_USE_NAMESPACE

ChartModifier::ChartModifier(Q3DSurface *chart)
    : m_chart(chart),
      m_xCount(10),
      m_zCount(10)
{
    m_chart->setAxisX(new Q3DValueAxis);
    m_chart->setAxisY(new Q3DValueAxis);
    m_chart->setAxisZ(new Q3DValueAxis);

    QSurfaceDataProxy *proxy = new QSurfaceDataProxy;
    m_chart->setActiveDataProxy(proxy);
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

    if (enable) {
        qDebug() << "Create Sqrt&Sin surface, (" << m_xCount << ", " << m_zCount << ")";

        qreal stepZ = 16.0 / qreal(m_zCount);
        qreal stepX = 16.0 / qreal(m_xCount);

        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        dataArray->reserve(m_zCount);
        for (qreal i = -8.0 + stepZ / 2.0 ; i < 8.0 ; i += stepZ) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            int index = 0;
            for (qreal j = -8.0 + stepX / 2.0; j < 8.0; j += stepX) {
                qreal R = qSqrt(i*i + j*j) + 0.01;
                qreal y = (sin(R)/R + 0.24) * 1.61;
                (*newRow)[index++] = y;
                if (y > biggest) biggest = y;
                if (y < smallest) smallest = y;
            }
            *dataArray << newRow;
        }

        m_chart->axisX()->setRange(0.0, qreal(m_xCount - 1));
        m_chart->axisY()->setRange(0.0, 2.0);
        m_chart->axisZ()->setRange(0.0, qreal(m_zCount - 1));
        m_chart->activeDataProxy()->resetArray(dataArray);

        //qDebug() << "biggest = " << biggest << ", smallest = " << smallest;
    } else {
        qDebug() << "Remove surface";
    }
}

void ChartModifier::togglePlane(bool enable)
{
    qDebug() << "ChartModifier::togglePlane " << enable;

    if (enable) {
        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        qreal y = 2.0 / qreal(m_zCount - 1);
        dataArray->reserve(m_zCount);
        for (int i = 0; i < m_zCount; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            for (int j = 0; j < m_xCount; j++)
                (*newRow)[j] = i * y;
            *dataArray << newRow;
        }

        m_chart->axisX()->setSegmentCount(3);
        m_chart->axisX()->setRange(0.0, qreal(m_xCount - 1));
        //m_chart->axisY()->setSegmentCount(4);
        m_chart->axisY()->setRange(0.0, 2.0);
        //m_chart->axisZ()->setSegmentCount(4/*m_zCount - 1*/);
        m_chart->axisZ()->setRange(0.0, qreal(m_zCount - 1));

        m_chart->activeDataProxy()->resetArray(dataArray);
    }
}

void ChartModifier::setHeightMapData(bool enable)
{
    if (enable) {
        QImage image(":/maps/map");

        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        uchar *bits = image.bits();

        int p = image.width() * 4 * (image.height() - 1);
        dataArray->reserve(image.height());
        qDebug() << image.height() << image.width();
        for (int i = image.height(); i > 0; i--, p -= image.width() * 4) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(image.width());
            for (int j = 0; j < image.width(); j++)
                (*newRow)[j] = (qreal(bits[p + (j * 4)]) + 1.0) / 1.0;
            *dataArray << newRow;
        }

        m_chart->axisX()->setRange(0.0, qreal(image.width() - 1));
        m_chart->axisY()->setRange(0.0, 255.0);
        m_chart->axisZ()->setRange(0.0, qreal(image.height() - 1));

        m_chart->activeDataProxy()->resetArray(dataArray);
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


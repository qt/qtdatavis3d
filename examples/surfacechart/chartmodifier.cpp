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

#include "chartmodifier.h"
#include <QtDataVisualization/Q3DValueAxis>
#include <QtDataVisualization/QSurfaceDataProxy>

#include <qmath.h>
#include <QLinearGradient>
#include <QDebug>

QT_DATAVISUALIZATION_USE_NAMESPACE

//#define JITTER_PLANE
//#define WONKY_PLANE

ChartModifier::ChartModifier(Q3DSurface *chart)
    : m_chart(chart),
      m_gridSliderX(0),
      m_gridSliderZ(0),
      m_axisRangeSliderX(0),
      m_axisRangeSliderZ(0),
      m_axisMinSliderX(0),
      m_axisMinSliderZ(0),
      m_xCount(50),
      m_zCount(50),
      m_activeSample(0),
      m_fontSize(40),
      m_rangeX(16.0),
      m_rangeZ(16.0),
      m_minX(-8.0),
      m_minZ(-8.0),
      m_planeArray(0)
{
    m_chart->setAxisX(new Q3DValueAxis);
    m_chart->setAxisY(new Q3DValueAxis);
    m_chart->setAxisZ(new Q3DValueAxis);
    m_chart->axisX()->setRange(m_minX, m_minX + m_rangeX);
    m_chart->axisZ()->setRange(m_minZ, m_minZ + m_rangeZ);
    changeStyle();

    connect(&m_timer, &QTimer::timeout, this, &ChartModifier::timeout);
}

ChartModifier::~ChartModifier()
{
    delete m_chart;
}

void ChartModifier::toggleSmooth(bool enabled)
{
    qDebug() << "ChartModifier::toggleSmooth " << enabled;
    m_chart->setSmoothSurfaceEnabled(enabled);
}

void ChartModifier::toggleSurfaceGrid(bool enable)
{
    qDebug() << "ChartModifier::toggleSurfaceGrid" << enable;
    m_chart->setSurfaceGridEnabled(enable);
}

void ChartModifier::toggleSqrtSin(bool enable)
{
    if (enable) {
        qDebug() << "Create Sqrt&Sin surface, (" << m_xCount << ", " << m_zCount << ")";

        float minX = -10.0;
        float maxX = 10.0;
        float minZ = -10.0;
        float maxZ = 10.0;
        float stepX = (maxX - minX) / float(m_xCount - 1);
        float stepZ = (maxZ - minZ) / float(m_zCount - 1);

        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        dataArray->reserve(m_zCount);
        for (float i = 0; i < m_zCount; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            for (float j = 0; j < m_xCount; j++) {
                float x = j * stepX + minX;
                float z = i * stepZ + minZ;
                float R = qSqrt(x * x + z * z) + 0.01f;
                float y = (qSin(R) / R + 0.24f) * 1.61f + 1.0f;
                (*newRow)[j].setPosition(QVector3D(x, y, z));
                qDebug() << x << y << z;
            }
            *dataArray << newRow;
        }

        m_chart->axisY()->setRange(1.0, 3.0);
        m_chart->axisX()->setLabelFormat("%.2f");
        m_chart->axisZ()->setLabelFormat("%.2f");

        resetArrayAndSliders(dataArray, minZ, maxZ, minX, maxX);

        m_activeSample = ChartModifier::SqrtSin;
    } else {
        qDebug() << "Remove surface";
    }
}

void ChartModifier::togglePlane(bool enable)
{
    qDebug() << "ChartModifier::togglePlane " << enable;

    if (enable) {
        m_planeArray = new QSurfaceDataArray;

#ifdef JITTER_PLANE
        m_timer.start(0);
#endif
        m_chart->axisY()->setRange(0.0, 1.0);
        m_chart->axisX()->setLabelFormat("%.2f");
        m_chart->axisZ()->setLabelFormat("%.2f");

        m_planeArray->reserve(m_zCount);
        float minX = -10.0;
        float maxX = 20.0;
        float minZ = -10.0;
        float maxZ = 10.0;
        float stepX = (maxX - minX) / float(m_xCount - 1);
        float stepZ = (maxZ - minZ) / float(m_zCount - 1);
#ifdef WONKY_PLANE
        float halfZ = m_zCount / 2;
        float wonkyFactor = 0.01f;
        float maxStepX = 0.0f;
        for (float i = 0; i < m_zCount; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            if (i < halfZ) {
                stepX += wonkyFactor;
                maxStepX = stepX;
            } else {
                stepX -= wonkyFactor;
            }
            for (float j = 0; j < m_xCount; j++) {
                (*newRow)[j].setPosition(QVector3D(j * stepX + minX, -0.04f,
                                                   i * stepZ + minZ));

            }
            *m_planeArray << newRow;
        }

        resetArrayAndSliders(m_planeArray, minZ, maxZ, minX, m_xCount * maxStepX + minZ);
#else
        for (float i = 0; i < m_zCount; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            for (float j = 0; j < m_xCount; j++)
                (*newRow)[j].setPosition(QVector3D(j * stepX + minX, -0.04f, i * stepZ + minZ));

            *m_planeArray << newRow;
        }

        resetArrayAndSliders(m_planeArray, minZ, maxZ, minX, maxX);
#endif

        m_activeSample = ChartModifier::Plane;
    }
#ifdef JITTER_PLANE
    else {
        m_timer.stop();
    }
#endif
}

void ChartModifier::setHeightMapData(bool enable)
{
    if (enable) {
        // Do the height map the hard way.
        // Easier alternative would be to use the QHeightMapSurfaceDataProxy.
        QImage image(":/maps/map");

        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        uchar *bits = image.bits();

        int p = image.width() * 4 * (image.height() - 1);
        dataArray->reserve(image.height());
        float minX = 34.0;
        float maxX = 40.0;
        float minZ = 18.0;
        float maxZ = 24.0;
        float xMul = (maxX - minX) / float(image.width() - 1);
        float zMul = (maxZ - minZ) / float(image.height() - 1);
        for (int i = 0; i < image.height(); i++, p -= image.width() * 4) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(image.width());
            for (int j = 0; j < image.width(); j++) {
                (*newRow)[j].setPosition(QVector3D((float(j) * xMul) + minX,
                                                   (float(bits[p + (j * 4)]) + 1.0f) / 1.0f,
                                                   (float(i) * zMul) + minZ));
            }
            *dataArray << newRow;
        }

        m_chart->axisY()->setAutoAdjustRange(true);
        m_chart->axisX()->setLabelFormat("%.1f N");
        m_chart->axisZ()->setLabelFormat("%.1f E");

        resetArrayAndSliders(dataArray, minZ, maxZ, minX, maxX);

        m_activeSample = ChartModifier::Map;
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

    updateSamples();

    qDebug() << "X count =" << count;
}

void ChartModifier::adjustZCount(int count)
{
    m_zCount = count;

    updateSamples();

    qDebug() << "Z count =" << count;
}

void ChartModifier::adjustXRange(int range)
{
    m_rangeX = range;
    m_chart->axisX()->setRange(m_minX, m_minX + m_rangeX);

    qDebug() << "X Range =" << range;
}

void ChartModifier::adjustZRange(int range)
{
    m_rangeZ = range;
    m_chart->axisZ()->setRange(m_minZ, m_minZ + m_rangeZ);

    qDebug() << "Z Range =" << range;
}

void ChartModifier::adjustXMin(int min)
{
    m_minX = min;
    m_chart->axisX()->setRange(m_minX, m_minX + m_rangeX);

    qDebug() << "X Minimum =" << min;
}

void ChartModifier::adjustZMin(int min)
{
    m_minZ = min;
    m_chart->axisZ()->setRange(m_minZ, m_minZ + m_rangeZ);

    qDebug() << "Z Minimum =" << min;
}

void ChartModifier::gradientPressed()
{
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_chart->setGradient(gradient);
}

void ChartModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    m_chart->setFont(newFont);
}

void ChartModifier::changeStyle()
{
    static int style = QDataVis::LabelStyleFromTheme;

    m_chart->setLabelStyle((QDataVis::LabelStyle)style);

    if (++style > QDataVis::LabelStyleTransparent)
        style = QDataVis::LabelStyleOpaque;
}

void ChartModifier::changeTheme(int theme)
{
    m_chart->setTheme((QDataVis::Theme)theme);
}

void ChartModifier::timeout()
{
    int rows = m_planeArray->size();
    int columns = m_planeArray->at(0)->size();

    // Induce minor random jitter to the existing plane array
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            (*m_planeArray->at(i))[j].setX(m_planeArray->at(i)->at(j).x()
                                           * ((float((rand() % 10) + 5.0f) / 10000.0f) + 0.999f));
            (*m_planeArray->at(i))[j].setY(m_planeArray->at(i)->at(j).y()
                                           * ((float((rand() % 10) + 5.0f) / 1000.0f) + 0.99f) + 0.0001f);
            (*m_planeArray->at(i))[j].setZ(m_planeArray->at(i)->at(j).z()
                                           * ((float((rand() % 10) + 5.0f) / 10000.0f) + 0.999f));
        }
    }

    // Reset same array to make it redraw
    m_chart->activeDataProxy()->resetArray(m_planeArray);
}

void ChartModifier::resetArrayAndSliders(QSurfaceDataArray *array, qreal minZ, qreal maxZ, qreal minX, qreal maxX)
{
    m_axisMinSliderX->setValue(minX);
    m_axisMinSliderZ->setValue(minZ);
    m_axisRangeSliderX->setValue(maxX - minX);
    m_axisRangeSliderZ->setValue(maxZ - minZ);

    m_chart->activeDataProxy()->resetArray(array);
}

void ChartModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowQuality(quality);
    m_chart->setShadowQuality(sq);
}

void ChartModifier::changeSelectionMode(int mode)
{
    switch (mode) {
    case 0:
        qDebug() << "QDataVis::ModeNone";
        m_chart->setSelectionMode(QDataVis::ModeNone);
        break;
    case 1:
        qDebug() << "QDataVis::ModeItem";
        m_chart->setSelectionMode(QDataVis::ModeItem);
        break;
    case 2:
        qDebug() << "QDataVis::ModeSliceRow";
        m_chart->setSelectionMode(QDataVis::ModeSliceRow);
        break;
    case 3:
        qDebug() << "QDataVis::ModeSliceColumn";
        m_chart->setSelectionMode(QDataVis::ModeSliceColumn);
        break;
    default:
        qDebug() << __FUNCTION__ << " Unsupported selection mode.";
        break;
    }
}

void ChartModifier::updateSamples()
{
    switch (m_activeSample) {
    case SqrtSin:
        toggleSqrtSin(true);
        break;

    case Plane:
        togglePlane(true);
        break;

    default:
        break;
    }
}


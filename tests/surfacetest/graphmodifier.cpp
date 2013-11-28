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

#include "graphmodifier.h"
#include <QtDataVisualization/Q3DValueAxis>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/Q3DTheme>

#include <qmath.h>
#include <QLinearGradient>
#include <QDebug>
#include <QComboBox>

QT_DATAVISUALIZATION_USE_NAMESPACE

//#define JITTER_PLANE
//#define WONKY_PLANE

GraphModifier::GraphModifier(Q3DSurface *graph)
    : m_graph(graph),
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
      m_planeArray(0),
      m_theSeries(new QSurface3DSeries)
{
    m_graph->setAxisX(new Q3DValueAxis);
    m_graph->setAxisY(new Q3DValueAxis);
    m_graph->setAxisZ(new Q3DValueAxis);
    m_graph->axisX()->setRange(m_minX, m_minX + m_rangeX);
    m_graph->axisZ()->setRange(m_minZ, m_minZ + m_rangeZ);
    m_graph->addSeries(m_theSeries);
    changeStyle();

    connect(&m_timer, &QTimer::timeout, this, &GraphModifier::timeout);
    connect(m_theSeries, &QSurface3DSeries::selectedPointChanged, this, &GraphModifier::selectedPointChanged);
}

GraphModifier::~GraphModifier()
{
    delete m_graph;
}

void GraphModifier::toggleSmooth(bool enabled)
{
    qDebug() << "GraphModifier::toggleSmooth " << enabled;
    m_graph->setSmoothSurfaceEnabled(enabled);
}

void GraphModifier::toggleSurfaceGrid(bool enable)
{
    qDebug() << "GraphModifier::toggleSurfaceGrid" << enable;
    m_graph->setSurfaceGridEnabled(enable);
}

void GraphModifier::toggleSurface(bool enable)
{
    qDebug() << "GraphModifier::toggleSurface" << enable;
    m_theSeries->setVisible(enable);
}

void GraphModifier::toggleSqrtSin(bool enable)
{
    if (enable) {
        qDebug() << "Create Sqrt&Sin surface, (" << m_xCount << ", " << m_zCount << ")";

        float minX = -10.0f;
        float maxX = 10.0f;
        float minZ = -10.0f;
        float maxZ = 10.0f;
        float stepX = (maxX - minX) / float(m_xCount - 1);
        float stepZ = (maxZ - minZ) / float(m_zCount - 1);

        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        dataArray->reserve(m_zCount);
        for (float i = 0; i < m_zCount; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            // Keep values within range bounds, since just adding step can cause minor drift due
            // to the rounding errors.
            float z = qMin(maxZ, (i * stepZ + minZ));
            for (float j = 0; j < m_xCount; j++) {
                float x = qMin(maxX, (j * stepX + minX));
                float R = qSqrt(x * x + z * z) + 0.01f;
                float y = (qSin(R) / R + 0.24f) * 1.61f + 1.0f;
                (*newRow)[j].setPosition(QVector3D(x, y, z));
            }
            *dataArray << newRow;
        }

        m_graph->axisY()->setRange(1.0, 3.0);
        m_graph->axisX()->setLabelFormat("%.2f");
        m_graph->axisZ()->setLabelFormat("%.2f");

        resetArrayAndSliders(dataArray, minZ, maxZ, minX, maxX);

        m_activeSample = GraphModifier::SqrtSin;
    } else {
        qDebug() << "Remove surface";
    }
}

void GraphModifier::togglePlane(bool enable)
{
    qDebug() << "GraphModifier::togglePlane " << enable;

    if (enable) {
        m_planeArray = new QSurfaceDataArray;

#ifdef JITTER_PLANE
        m_timer.start(0);
#endif
        m_graph->axisY()->setRange(0.0, 1.0);
        m_graph->axisX()->setLabelFormat("%.2f");
        m_graph->axisZ()->setLabelFormat("%.2f");

        m_planeArray->reserve(m_zCount);
        float minX = -10.0f;
        float maxX = 20.0f;
        float minZ = -10.0f;
        float maxZ = 10.0f;
        float stepX = (maxX - minX) / float(m_xCount - 1);
        float stepZ = (maxZ - minZ) / float(m_zCount - 1);
#ifdef WONKY_PLANE
        float halfZ = m_zCount / 2;
        float wonkyFactor = 0.01f;
        float maxStepX = 0.0f;
        float add = 0.0f;
        for (float i = 0; i < m_zCount; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            if (i < halfZ) {
                stepX += wonkyFactor;
                maxStepX = stepX;
            } else {
                stepX -= wonkyFactor;
            }
            add = 0.0f;
            for (float j = 0; j < m_xCount; j++) {
                (*newRow)[j].setPosition(QVector3D(j * stepX + minX, -0.04f,
                                                   i * stepZ + minZ + add));
                add += 0.5f;

            }
            *m_planeArray << newRow;
        }

        resetArrayAndSliders(m_planeArray, minZ, maxZ + add, minX, m_xCount * maxStepX + minX);
#else
        for (float i = 0; i < m_zCount; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            // Keep values within range bounds, since just adding step can cause minor drift due
            // to the rounding errors.
            float zVal;
            if (i == (m_zCount - 1))
                zVal = maxZ;
            else
                zVal = i * stepZ + minZ;

            float j = 0;
            for (; j < m_xCount - 1; j++)
                (*newRow)[j].setPosition(QVector3D(j * stepX + minX, -0.04f, zVal));
            (*newRow)[j].setPosition(QVector3D(maxX, -0.04f, zVal));

            *m_planeArray << newRow;
        }

        resetArrayAndSliders(m_planeArray, minZ, maxZ, minX, maxX);
#endif

        m_activeSample = GraphModifier::Plane;
    }
#ifdef JITTER_PLANE
    else {
        m_timer.stop();
    }
#endif
}

void GraphModifier::setHeightMapData(bool enable)
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

        m_graph->axisY()->setAutoAdjustRange(true);
        m_graph->axisX()->setLabelFormat("%.1f N");
        m_graph->axisZ()->setLabelFormat("%.1f E");

        resetArrayAndSliders(dataArray, minZ, maxZ, minX, maxX);

        m_activeSample = GraphModifier::Map;
    }
}

void GraphModifier::toggleGridSliderLock(bool enable)
{
    m_gridSlidersLocked = enable;
    if (m_gridSlidersLocked) {
        m_gridSliderZ->setEnabled(false);
        m_gridSliderZ->setValue(m_gridSliderX->value());
    } else {
        m_gridSliderZ->setEnabled(true);
    }
}

void GraphModifier::adjustXCount(int count)
{
    m_xCount = count;
    if (m_gridSlidersLocked)
        m_gridSliderZ->setValue(count);

    updateSamples();

    qDebug() << "X count =" << count;
}

void GraphModifier::adjustZCount(int count)
{
    m_zCount = count;

    updateSamples();

    qDebug() << "Z count =" << count;
}

void GraphModifier::adjustXRange(int range)
{
    m_rangeX = range;
    m_graph->axisX()->setRange(m_minX, m_minX + m_rangeX);

    qDebug() << "X Range =" << range;
}

void GraphModifier::adjustZRange(int range)
{
    m_rangeZ = range;
    m_graph->axisZ()->setRange(m_minZ, m_minZ + m_rangeZ);

    qDebug() << "Z Range =" << range;
}

void GraphModifier::adjustXMin(int min)
{
    m_minX = min;
    m_graph->axisX()->setRange(m_minX, m_minX + m_rangeX);

    qDebug() << "X Minimum =" << min;
}

void GraphModifier::adjustZMin(int min)
{
    m_minZ = min;
    m_graph->axisZ()->setRange(m_minZ, m_minZ + m_rangeZ);

    qDebug() << "Z Minimum =" << min;
}

void GraphModifier::gradientPressed()
{
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_graph->setGradient(gradient);
}

void GraphModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    m_graph->theme()->setFont(newFont);
}

void GraphModifier::changeStyle()
{
    m_graph->theme()->setLabelBackgroundEnabled(!m_graph->theme()->isLabelBackgroundEnabled());
}

void GraphModifier::selectButtonClicked()
{
    QSurfaceDataProxy *proxy = m_theSeries->dataProxy();
    int row = rand() % proxy->rowCount();
    int col = rand() % proxy->columnCount();

    m_theSeries->setSelectedPoint(QPoint(row, col));
}

void GraphModifier::selectedPointChanged(const QPoint &point)
{
    QString labelText = QStringLiteral("Selected row: %1, column: %2").arg(point.x()).arg(point.y());
    m_selectionInfoLabel->setText(labelText);
}

void GraphModifier::changeTheme(int theme)
{
    m_graph->setTheme(new Q3DTheme(QDataVis::Theme(theme)));
}


void GraphModifier::flipViews()
{
    m_graph->scene()->setSecondarySubviewOnTop(!m_graph->scene()->isSecondarySubviewOnTop());
}

void GraphModifier::timeout()
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
    m_theSeries->dataProxy()->resetArray(m_planeArray);
}

void GraphModifier::resetArrayAndSliders(QSurfaceDataArray *array, float minZ, float maxZ, float minX, float maxX)
{
    m_axisMinSliderX->setValue(minX);
    m_axisMinSliderZ->setValue(minZ);
    m_axisRangeSliderX->setValue(maxX - minX);
    m_axisRangeSliderZ->setValue(maxZ - minZ);

    m_theSeries->dataProxy()->resetArray(array);
}

void GraphModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
}

void GraphModifier::changeSelectionMode(int mode)
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (comboBox) {
        int flags = comboBox->itemData(mode).toInt();
        m_graph->setSelectionMode(QDataVis::SelectionFlags(flags));
    }
}

void GraphModifier::changeRow()
{
    if (m_activeSample == GraphModifier::SqrtSin) {
        qDebug() << "Generating new values to a row at randon pos";
        float minX = -10.0f;
        float maxX = 10.0f;
        float minZ = -10.0f;
        float maxZ = 10.0f;
        float stepX = (maxX - minX) / float(m_xCount - 1);
        float stepZ = (maxZ - minZ) / float(m_zCount - 1);
        float i = float(rand() % m_zCount);

        QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
        for (float j = 0; j < m_xCount; j++) {
            float x = qMin(maxX, (j * stepX + minX));
            float z = qMin(maxZ, (i * stepZ + minZ));
            float R = qSqrt(x * x + z * z) + 0.01f;
            float y = (qSin(R) / R + 0.24f) * 1.61f + 1.2f;
            (*newRow)[j].setPosition(QVector3D(x, y, z));
        }

        m_theSeries->dataProxy()->setRow(int(i), newRow);
    } else {
        qDebug() << "Change row function active only for SqrtSin";
    }
}

void GraphModifier::changeRows()
{
    if (m_activeSample == GraphModifier::SqrtSin) {
        qDebug() << "Generating new values to 3 rows from randon pos";

        float minX = -10.0f;
        float maxX = 10.0f;
        float minZ = -10.0f;
        float maxZ = 10.0f;
        float stepX = (maxX - minX) / float(m_xCount - 1);
        float stepZ = (maxZ - minZ) / float(m_zCount - 1);
        float start = float(rand() % (m_zCount - 3));

        QSurfaceDataArray dataArray;

        for (float i = start; i < (start + 3.0f); i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(m_xCount);
            float z = qMin(maxZ, (i * stepZ + minZ));
            for (float j = 0; j < m_xCount; j++) {
                float x = qMin(maxX, (j * stepX + minX));
                float R = qSqrt(x * x + z * z) + 0.01f;
                float y = (qSin(R) / R + 0.24f) * 1.61f + 1.2f;
                (*newRow)[j].setPosition(QVector3D(x, y, z));
            }
            dataArray.append(newRow);
        }

        m_theSeries->dataProxy()->setRows(int(start), dataArray);
    } else {
        qDebug() << "Change row function active only for SqrtSin";
    }
}

void GraphModifier::updateSamples()
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


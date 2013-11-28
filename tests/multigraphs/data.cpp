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

#define NOMINMAX

#include "data.h"
#include <QtDataVisualization/Q3DValueAxis>
#include <QtDataVisualization/Q3DCamera>
#include <QtDataVisualization/QBar3DSeries>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/Q3DTheme>
#include <QScrollBar>
#include <QSize>
#include <QImage>

QT_DATAVISUALIZATION_USE_NAMESPACE

Data::Data(Q3DSurface *surface, Q3DScatter *scatter, Q3DBars *bars,
           QTextEdit *statusArea, QWidget *widget) :
    m_surface(surface),
    m_scatter(scatter),
    m_bars(bars),
    m_statusArea(statusArea),
    m_widget(widget),
    m_resize(true),
    m_resolution(QSize(300, 300)),
    m_resolutionLevel(0),
    m_mode(Surface),
    m_started(false)
{
    // Initialize surface
    m_surface->setTheme(new Q3DTheme(QDataVis::ThemeIsabelle));
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_surface->setSelectionMode(QDataVis::SelectionNone);
    m_surface->theme()->setGridEnabled(false);
    m_surface->setGradient(gradient);
    m_surface->setSurfaceGridEnabled(false);
    m_surface->theme()->setBackgroundEnabled(false);
    m_surface->setSmoothSurfaceEnabled(false);
    m_surface->scene()->activeCamera()->setCameraPosition(0.0, 90.0, 150);
    m_surface->addSeries(new QSurface3DSeries(new QHeightMapSurfaceDataProxy()));

    // Initialize scatter
    m_scatter->setTheme(new Q3DTheme(QDataVis::ThemeStoneMoss));
    m_scatter->setSelectionMode(QDataVis::SelectionNone);
    m_scatter->theme()->setGridEnabled(false);
    m_scatter->setObjectType(QDataVis::MeshPoint);
    m_scatter->setShadowQuality(QDataVis::ShadowQualitySoftLow);
    m_scatter->scene()->activeCamera()->setCameraPosition(0.0, 85.0, 150);
    m_scatter->addSeries(new QScatter3DSeries);

    // Initialize bars
    m_bars->setTheme(new Q3DTheme(QDataVis::ThemeQt));
    m_bars->setSelectionMode(QDataVis::SelectionItemAndRow | QDataVis::SelectionSlice);
    m_bars->theme()->setGridEnabled(false);
    m_bars->setBarType(QDataVis::MeshBar, false);
    m_bars->setShadowQuality(QDataVis::ShadowQualityLow);
    m_bars->setBarSpacing(QSizeF(0.0, 0.0));
    m_bars->scene()->activeCamera()->setCameraPosition(0.0, 75.0, 150);
    m_bars->addSeries(new QBar3DSeries);

    // Hide scroll bar
    m_statusArea->verticalScrollBar()->setVisible(false);
}

Data::~Data()
{   // HACK: The current context needs to be destroyed last
    // TODO: Fix properly in datavis code somehow
    if (m_mode == Surface) {
        delete m_scatter;
        delete m_bars;
        delete m_surface;
    } else if (m_mode == Bars) {
        delete m_scatter;
        delete m_surface;
        delete m_bars;
    } else {
        delete m_bars;
        delete m_surface;
        delete m_scatter;
    }
    delete m_widget;
}

void Data::updateData()
{
    if (!m_started) {
        m_statusArea->append(QStringLiteral("<i>We are stopped. The changes will take effect once started.</i>"));
        return;
    }
    QImage depthMap = QImage(":/australia.png");
    if (m_resize) // Resize for better performance
        depthMap = depthMap.scaled(m_resolution);
    if (m_mode != Surface)
        setData(depthMap);
    else
        static_cast<QHeightMapSurfaceDataProxy *>(m_surface->seriesList().at(0)->dataProxy())->setHeightMap(
                depthMap);
}

void Data::clearData()
{
    m_bars->seriesList().at(0)->dataProxy()->resetArray(0);
    m_scatter->seriesList().at(0)->dataProxy()->resetArray(0);
    m_surface->seriesList().at(0)->dataProxy()->resetArray(0);
}

void Data::setResolution(int selection)
{
    m_resolutionLevel = selection;
    switch (selection) {
    case 0: {
        m_resize = true;
        m_resolution = QSize(300, 300);
        break;
    }
    case 1: {
        m_resize = true;
        m_resolution = QSize(600, 600);
        break;
    }
    case 2: {
        m_resize = true;
        m_resolution = QSize(800, 800);
        break;
    }
    case 3: {
        m_resize = false;
        m_resolution = QSize(1020, 1020); // image size
        break;
    }
    };
    if (m_mode == Scatter) {
        m_resize = true;
        m_resolution /= 3;
        m_scatterDataArray = new QScatterDataArray;
        m_scatterDataArray->resize(m_resolution.width() * m_resolution.height());
    } else if (m_mode == Bars) {
        m_resize = true;
        m_resolution /= 6;
        m_barDataArray = new QBarDataArray;
        m_barDataArray->reserve(m_resolution.height());
        for (int i = 0; i < m_resolution.height(); i++) {
            QBarDataRow *newProxyRow = new QBarDataRow(m_resolution.width());
            m_barDataArray->append(newProxyRow);
        }
    }

    m_statusArea->append(QString(QStringLiteral("<b>Resolution:</b> %1 x %2")).arg(
                             m_resolution.width()).arg(m_resolution.height()));

    updateData();
}

void Data::scrollDown()
{
    QScrollBar *scrollbar = m_statusArea->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void Data::useGradientOne()
{
    m_surface->setTheme(new Q3DTheme(QDataVis::ThemeIsabelle));
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(0.33, Qt::blue);
    gradient.setColorAt(0.67, Qt::red);
    gradient.setColorAt(1.0, Qt::yellow);
    m_surface->setGradient(gradient);
    m_statusArea->append(QStringLiteral("<b>Colors:</b> Thermal image imitation"));
}

void Data::useGradientTwo()
{
    m_surface->setTheme(new Q3DTheme(QDataVis::ThemeQt));
    QLinearGradient gradient;
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(0.8, Qt::red);
    gradient.setColorAt(1.0, Qt::green);
    m_surface->setGradient(gradient);
    m_statusArea->append(QStringLiteral("<b>Colors:</b> Highlight foreground"));
}

void Data::setData(const QImage &image)
{
    QImage heightImage = image;

    uchar *bits = heightImage.bits();

    int imageHeight = heightImage.height();
    int imageWidth = heightImage.width();
    int bitCount = imageWidth * 4 * (imageHeight - 1);
    int widthBits = imageWidth * 4;

    if (m_mode == Scatter) {
        QScatterDataItem *ptrToDataArray = &m_scatterDataArray->first();

        int limitsX = imageWidth / 2;
        int limitsZ = imageHeight / 2;
        float height = 0;
        int count = 0;

        for (int i = -limitsZ; i < limitsZ; i++, bitCount -= widthBits) {
            for (int j = -limitsX; j < limitsX; j++) {
                height = float(bits[bitCount + ((j + limitsX) * 4)]) - 128.0;
                if (height > -128) {
                    ptrToDataArray->setPosition(QVector3D(float(j), height, float(i)));
                    ptrToDataArray++;
                    count++;
                }
            }
        }

        QScatterDataArray *dataArray = new QScatterDataArray(m_scatterDataArray->mid(0, count));
        m_scatter->seriesList().at(0)->dataProxy()->resetArray(dataArray);
    } else {
        QBarDataArray *dataArray = m_barDataArray;
        for (int i = 0; i < imageHeight; i++, bitCount -= widthBits) {
            QBarDataRow &newRow = *dataArray->at(i);
            for (int j = 0; j < imageWidth; j++)
                newRow[j] = float(bits[bitCount + (j * 4)]);
        }

        m_bars->seriesList().at(0)->dataProxy()->resetArray(dataArray);
    }
}

void Data::changeMode(int mode)
{
    m_mode = VisualizationMode(mode);
    switch (m_mode) {
    case Surface: {
        m_statusArea->append(QStringLiteral("<b>Visualization Type:</b> Surface"));
        break;
    }
    case Scatter: {
        m_statusArea->append(QStringLiteral("<b>Visualization Type:</b> Scatter"));
        break;
    }
    default: {
        m_statusArea->append(QStringLiteral("<b>Visualization Type:</b> Bars"));
        break;
    }
    }
    // Reset resolution after mode change
    setResolution(m_resolutionLevel);
}

void Data::start()
{
    m_started = true;
    updateData();
    m_statusArea->append(QStringLiteral("<b>Started<\b>"));
}

void Data::stop()
{
    m_started = false;
    clearData();
    m_statusArea->append(QStringLiteral("<b>Stopped<\b>"));
}

ContainerChanger::ContainerChanger(QWidget *surface, QWidget *scatter, QWidget *bars,
                                   QWidget *buttonOne, QWidget *buttonTwo)
    : m_surface(surface),
      m_scatter(scatter),
      m_bars(bars),
      m_button1(buttonOne),
      m_button2(buttonTwo)
{
}

ContainerChanger::~ContainerChanger()
{
}

void ContainerChanger::changeContainer(int container)
{
    switch (container) {
    case 0: {
        m_scatter->setVisible(false);
        m_bars->setVisible(false);
        m_surface->setVisible(true);
        m_button1->setEnabled(true);
        m_button2->setEnabled(true);
        break;
    }
    case 1: {
        m_surface->setVisible(false);
        m_bars->setVisible(false);
        m_scatter->setVisible(true);
        m_button1->setEnabled(false);
        m_button2->setEnabled(false);
        break;
    }
    case 2: {
        m_surface->setVisible(false);
        m_scatter->setVisible(false);
        m_bars->setVisible(true);
        m_button1->setEnabled(false);
        m_button2->setEnabled(false);
        break;
    }
    }
}

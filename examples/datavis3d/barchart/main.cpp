/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
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

#include "q3dbars.h"

#include <QGuiApplication>
#include <QTimer>
#include <QDebug>

//#define CYCLE_THROUGH_STYLES
#define CYCLE_THROUGH_PRESET_CAMERAS
#define USE_STATIC_DATA

using namespace QtDataVis3D;

class ChartDataGenerator : public QObject
{
public:
    explicit ChartDataGenerator(Q3DBars *barchart);
    ~ChartDataGenerator();

    void addDataSet();
    void addBars();
    void changeStyle();
    void changePresetCamera();
    void start();

private:
    Q3DBars *m_chart;
    QTimer *m_dataTimer;
    QTimer *m_styleTimer;
    QTimer *m_presetTimer;
    int m_columnCount;
    int m_rowCount;
};

ChartDataGenerator::ChartDataGenerator(Q3DBars *barchart)
    : m_chart(barchart)
    , m_dataTimer(0)
    , m_styleTimer(0)
    , m_presetTimer(0)
    , m_columnCount(10)
    , m_rowCount(10)
{
    // Set up bar specifications; make the bars twice as wide as they are deep,
    // and add a small space between the bars
    m_chart->setBarSpecs(QPointF(1.0f, 1.0f), QPointF(0.2f, 0.2f), true);
    // Set up sample space; make it twice as deep as it's wide
    m_chart->setupSampleSpace(QPoint(m_columnCount, m_rowCount));
    // Set bar type to smooth bar
#ifndef CYCLE_THROUGH_STYLES
    m_chart->setBarType(Q3DBars::Bars, false);
#endif
    // Set selection mode to full
    m_chart->setSelectionMode(Q3DBars::BarRowAndColumn);
    // Set bar colors
    m_chart->setBarColor(QColor(Qt::black), QColor(Qt::cyan), QColor(Qt::green), true);
    //m_chart->setBarColor(QColor(Qt::black), QColor(Qt::red), QColor(Qt::darkBlue));
}

ChartDataGenerator::~ChartDataGenerator()
{
    if (m_dataTimer) {
        m_dataTimer->stop();
        delete m_dataTimer;
    }
    if (m_styleTimer) {
        m_styleTimer->stop();
        delete m_styleTimer;
    }
    if (m_presetTimer) {
        m_presetTimer->stop();
        delete m_presetTimer;
    }
    delete m_chart;
}

void ChartDataGenerator::start()
{
#ifndef USE_STATIC_DATA
    m_dataTimer = new QTimer();
    m_dataTimer->setTimerType(Qt::CoarseTimer);
    m_dataTimer->setInterval(100);
    QObject::connect(m_dataTimer, &QTimer::timeout, this, &ChartDataGenerator::addBars);
    m_dataTimer->start(100);
#else
    addDataSet();
#endif

#ifdef CYCLE_THROUGH_STYLES
    // Change bar style every 10 seconds
    m_styleTimer = new QTimer();
    m_styleTimer->setTimerType(Qt::CoarseTimer);
    m_styleTimer->setInterval(10000);
    QObject::connect(m_styleTimer, &QTimer::timeout, this, &ChartDataGenerator::changeStyle);
    m_styleTimer->start(10000);
#endif

#ifdef CYCLE_THROUGH_PRESET_CAMERAS
    // Change preset camera every 5 seconds
    m_presetTimer = new QTimer();
    m_presetTimer->setTimerType(Qt::CoarseTimer);
    m_presetTimer->setInterval(5000);
    QObject::connect(m_presetTimer, &QTimer::timeout, this
                     , &ChartDataGenerator::changePresetCamera);
    m_presetTimer->start(5000);
#endif
}

void ChartDataGenerator::addDataSet()
{
    QVector< QVector<float> > data;
    QVector<float> row;
    for (int j = 0; j < m_rowCount; j++) {
        for (int i = 0; i < m_columnCount; i++) {
            row.append(((float)i / (float)m_columnCount) / 1.5f + (float)(rand() % 30) / 100);
            //row.append(1.0f);
        }
        data.append(row);
        row.clear();
    }
    m_chart->addDataSet(data);
}

void ChartDataGenerator::addBars()
{
    QVector<float> data;
    for (int i = 0; i < m_columnCount; i++) {
        data.append(((float)i / (float)m_columnCount) / 2.0f + (float)(rand() % 30) / 100);
    }
    m_chart->addDataRow(data);
}

void ChartDataGenerator::changeStyle()
{
    static int model = 0;
    switch (model) {
    case 0:
        m_chart->setBarType(Q3DBars::Cylinders, false);
        break;
    case 1:
        m_chart->setBarType(Q3DBars::Cylinders, true);
        break;
    case 2:
        m_chart->setBarType(Q3DBars::Cones, false);
        break;
    case 3:
        m_chart->setBarType(Q3DBars::Cones, true);
        break;
    case 4:
        m_chart->setBarType(Q3DBars::Bars, false);
        break;
    case 5:
        m_chart->setBarType(Q3DBars::Bars, true);
        break;
    case 6:
        m_chart->setBarType(Q3DBars::Pyramids, false);
        break;
    case 7:
        m_chart->setBarType(Q3DBars::Pyramids, true);
        break;
    }
    model++;
    if (model > 7)
        model = 0;
}

void ChartDataGenerator::changePresetCamera()
{
    static int preset = 0;

    m_chart->setCameraPreset((Q3DBars::CameraPreset)preset);

    if (++preset > (int)Q3DBars::PresetDirectlyAboveCCW45)
        preset = 0;
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    Q3DBars barchart;
    barchart.resize(800, 600);
    barchart.show();

    ChartDataGenerator *generator = new ChartDataGenerator(&barchart);
    generator->start();

    return app.exec();
}

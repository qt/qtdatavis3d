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

using namespace QtDataVis3D;

class ChartDataGenerator : public QObject
{
public:
    explicit ChartDataGenerator(Q3DBars *barchart);
    ~ChartDataGenerator();

    void addBars();
    void changeStyle();
    void start();

private:
    Q3DBars *m_chart;
    QTimer *m_dataTimer;
    QTimer *m_testTimer;
    int m_columnCount;
};

ChartDataGenerator::ChartDataGenerator(Q3DBars *barchart)
    : m_chart(barchart)
    , m_dataTimer(0)
    , m_testTimer(0)
    , m_columnCount(20)
{
    // Set up bar specifications; make the bars twice as wide as they are deep,
    // and add a small space between the bars
    m_chart->setBarSpecs(QPointF(2.0f, 1.0f), QPointF(0.2f, 0.2f), true);
    // Set up sample space; make it twice as deep as it's wide
    m_chart->setupSampleSpace(QPoint(m_columnCount, m_columnCount*2));
    // Set bar type to smooth bar
    //m_chart->setBarType(Q3DBars::Bars, true);
}

ChartDataGenerator::~ChartDataGenerator()
{
    if (m_dataTimer) {
        m_dataTimer->stop();
        delete m_dataTimer;
    }
    if (m_testTimer) {
        m_testTimer->stop();
        delete m_testTimer;
    }
    delete m_chart;
}

void ChartDataGenerator::start()
{
    m_dataTimer = new QTimer();
    m_dataTimer->setTimerType(Qt::CoarseTimer);
    m_dataTimer->setInterval(100);
    QObject::connect(m_dataTimer, &QTimer::timeout, this, &ChartDataGenerator::addBars);
    m_dataTimer->start(100);

    // Uncomment this to enable style changing every 10 seconds
//    m_testTimer = new QTimer();
//    m_testTimer->setTimerType(Qt::CoarseTimer);
//    m_testTimer->setInterval(10000);
//    QObject::connect(m_testTimer, &QTimer::timeout, this, &ChartDataGenerator::changeStyle);
//    m_testTimer->start(10000);
}

void ChartDataGenerator::addBars()
{
    QVector<float> data;
    for (int i = 0; i < m_columnCount; i++) {
        data.append(((float)i / (float)m_columnCount) / 2.0f + (float)(rand() % 30) / 100);
    }
    m_chart->addDataSet(data);
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
        m_chart->setBarType(QtDataVis3D::Q3DBars::Pyramids, true);
        break;
    }
    model++;
    if (model > 7)
        model = 0;
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    Q3DBars barchart;
    barchart.resize(1024, 768);
    barchart.show();

    ChartDataGenerator *generator = new ChartDataGenerator(&barchart);
    generator->start();

    return app.exec();
}

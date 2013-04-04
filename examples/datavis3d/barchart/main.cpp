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
#include "qdataitem.h"

#include <QGuiApplication>
#include <QTimer>
#include <QDebug>

//#define CYCLE_THROUGH_STYLES
//#define CYCLE_THROUGH_PRESET_CAMERAS
//#define CYCLE_THROUGH_THEMES
#define USE_ACTUAL_DATA

#ifdef USE_ACTUAL_DATA
const int rows = 13;
const int columns = 12;
#else
const int rows = 9;
const int columns = 9;
#endif

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
    void changeTheme();
    void start();

private:
    Q3DBars *m_chart;
    QTimer *m_dataTimer;
    QTimer *m_styleTimer;
    QTimer *m_presetTimer;
    QTimer *m_themeTimer;
    int m_columnCount;
    int m_rowCount;
};

ChartDataGenerator::ChartDataGenerator(Q3DBars *barchart)
    : m_chart(barchart)
    , m_dataTimer(0)
    , m_styleTimer(0)
    , m_presetTimer(0)
    , m_themeTimer(0)
    , m_columnCount(columns)
    , m_rowCount(rows)
{
    // Set up bar specifications; make the bars as wide as they are deep,
    // and add a small space between the bars
    m_chart->setBarSpecs(QPointF(1.0f, 1.0f), QPointF(0.2f, 0.2f), true);

#ifndef USE_ACTUAL_DATA
    // Set up sample space; make it as deep as it's wide
    m_chart->setupSampleSpace(QPoint(m_columnCount, m_rowCount));
#else
    // Set up sample space; make it match actual data size
    m_chart->setupSampleSpace(QPoint(m_columnCount, m_rowCount)
                              , QStringLiteral("year"), QStringLiteral("month")
                              , QStringLiteral("rainfall (in mm)"));
#endif

    // Set bar type to smooth bar
#ifndef CYCLE_THROUGH_STYLES
#ifdef USE_ACTUAL_DATA
    m_chart->setBarType(Q3DBars::Cylinders, true);
#else
    m_chart->setBarType(Q3DBars::Bars, false);
#endif
#endif

#ifdef USE_ACTUAL_DATA
    // Set selection mode to bar and column
    m_chart->setSelectionMode(Q3DBars::BarAndColumn);
#else
    // Set selection mode to full
    m_chart->setSelectionMode(Q3DBars::BarRowAndColumn);
#endif

#ifndef CYCLE_THROUGH_THEMES
#ifndef USE_ACTUAL_DATA
    // Set bar colors
    m_chart->setBarColor(QColor(Qt::black), QColor(Qt::red), QColor(Qt::darkBlue));
#else
    // Set theme
    m_chart->setTheme(Q3DBars::ThemeBlueNcs);
#endif
#endif

    // Set preset camera position
    m_chart->setCameraPreset(Q3DBars::PresetFront);
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
    if (m_themeTimer) {
        m_themeTimer->stop();
        delete m_themeTimer;
    }
    delete m_chart;
}

void ChartDataGenerator::start()
{
#ifndef USE_ACTUAL_DATA
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

#ifdef CYCLE_THROUGH_THEMES
    // Change theme every 2 seconds
    m_themeTimer = new QTimer();
    m_themeTimer->setTimerType(Qt::CoarseTimer);
    m_themeTimer->setInterval(6000);//2000);
    QObject::connect(m_themeTimer, &QTimer::timeout, this
                     , &ChartDataGenerator::changeTheme);
    m_themeTimer->start(6000);//2000);
#endif
}

void ChartDataGenerator::addDataSet()
{
#if 0
    QVector< QVector<float> > data;
    QVector<float> row;
    // TODO: Keep here for testing
    for (int j = 0; j < m_rowCount; j++) {
        for (int i = 0; i < m_columnCount; i++) {
            //row.prepend(((float)i / (float)m_columnCount) / 1.5f + (float)(rand() % 30) / 100);
            row.prepend(1.0f);
        }
        data.prepend(row);
        row.clear();
    }
#else
    m_chart->setWindowTitle(QStringLiteral("Monthly rainfall in Northern Finland (2000-2012)"));
    // Fill in rainfall per month from 2000 to 2012 in Northern Finland (Sodankylä, Utsjoki, Kuusamo)
    QVector< QVector<QDataItem*> > data;
    QVector<QDataItem*> row;
    // 2000
    row.prepend(new QDataItem(72, "mm, January 2000"));
    row.prepend(new QDataItem(47, "mm, February 2000"));
    row.prepend(new QDataItem(37, "mm, March 2000"));
    row.prepend(new QDataItem(79, "mm, April 2000"));
    row.prepend(new QDataItem(42, "mm, May 2000"));
    row.prepend(new QDataItem(73, "mm, June 2000"));
    row.prepend(new QDataItem(94, "mm, July 2000"));
    row.prepend(new QDataItem(37, "mm, August 2000"));
    row.prepend(new QDataItem(17, "mm, September 2000"));
    row.prepend(new QDataItem(69, "mm, October 2000"));
    row.prepend(new QDataItem(42, "mm, November 2000"));
    row.prepend(new QDataItem(42, "mm, December 2000"));
    data.prepend(row);
    row.clear();
    // 2001
    row.prepend(new QDataItem(25, "mm, January 2001"));
    row.prepend(new QDataItem(47, "mm, February 2001"));
    row.prepend(new QDataItem(20, "mm, March 2001"));
    row.prepend(new QDataItem(70, "mm, April 2001"));
    row.prepend(new QDataItem(27, "mm, May 2001"));
    row.prepend(new QDataItem(40, "mm, June 2001"));
    row.prepend(new QDataItem(123, "mm, July 2001"));
    row.prepend(new QDataItem(39, "mm, August 2001"));
    row.prepend(new QDataItem(66, "mm, September 2001"));
    row.prepend(new QDataItem(55, "mm, October 2001"));
    row.prepend(new QDataItem(29, "mm, November 2001"));
    row.prepend(new QDataItem(12, "mm, December 2001"));
    data.prepend(row);
    row.clear();
    // 2002
    row.prepend(new QDataItem(24, "mm, January 2002"));
    row.prepend(new QDataItem(45, "mm, February 2002"));
    row.prepend(new QDataItem(27, "mm, March 2002"));
    row.prepend(new QDataItem(30, "mm, April 2002"));
    row.prepend(new QDataItem(16, "mm, May 2002"));
    row.prepend(new QDataItem(98, "mm, June 2002"));
    row.prepend(new QDataItem(122, "mm, July 2002"));
    row.prepend(new QDataItem(20, "mm, August 2002"));
    row.prepend(new QDataItem(50, "mm, September 2002"));
    row.prepend(new QDataItem(24, "mm, October 2002"));
    row.prepend(new QDataItem(22, "mm, November 2002"));
    row.prepend(new QDataItem(12, "mm, December 2002"));
    data.prepend(row);
    row.clear();
    // 2003
    row.prepend(new QDataItem(43, "mm, January 2003"));
    row.prepend(new QDataItem(17, "mm, February 2003"));
    row.prepend(new QDataItem(26, "mm, March 2003"));
    row.prepend(new QDataItem(22, "mm, April 2003"));
    row.prepend(new QDataItem(60, "mm, May 2003"));
    row.prepend(new QDataItem(14, "mm, June 2003"));
    row.prepend(new QDataItem(86, "mm, July 2003"));
    row.prepend(new QDataItem(77, "mm, August 2003"));
    row.prepend(new QDataItem(69, "mm, September 2003"));
    row.prepend(new QDataItem(49, "mm, October 2003"));
    row.prepend(new QDataItem(23, "mm, November 2003"));
    row.prepend(new QDataItem(44, "mm, December 2003"));
    data.prepend(row);
    row.clear();
    // 2004
    row.prepend(new QDataItem(15, "mm, January 2004"));
    row.prepend(new QDataItem(19, "mm, February 2004"));
    row.prepend(new QDataItem(10, "mm, March 2004"));
    row.prepend(new QDataItem(11, "mm, April 2004"));
    row.prepend(new QDataItem(41, "mm, May 2004"));
    row.prepend(new QDataItem(29, "mm, June 2004"));
    row.prepend(new QDataItem(49, "mm, July 2004"));
    row.prepend(new QDataItem(72, "mm, August 2004"));
    row.prepend(new QDataItem(50, "mm, September 2004"));
    row.prepend(new QDataItem(18, "mm, October 2004"));
    row.prepend(new QDataItem(19, "mm, November 2004"));
    row.prepend(new QDataItem(40, "mm, December 2004"));
    data.prepend(row);
    row.clear();
    // 2005
    row.prepend(new QDataItem(60, "mm, January 2005"));
    row.prepend(new QDataItem(24, "mm, February 2005"));
    row.prepend(new QDataItem(12, "mm, March 2005"));
    row.prepend(new QDataItem(50, "mm, April 2005"));
    row.prepend(new QDataItem(88, "mm, May 2005"));
    row.prepend(new QDataItem(32, "mm, June 2005"));
    row.prepend(new QDataItem(76, "mm, July 2005"));
    row.prepend(new QDataItem(55, "mm, August 2005"));
    row.prepend(new QDataItem(92, "mm, September 2005"));
    row.prepend(new QDataItem(35, "mm, October 2005"));
    row.prepend(new QDataItem(105, "mm, November 2005"));
    row.prepend(new QDataItem(59, "mm, December 2005"));
    data.prepend(row);
    row.clear();
    // 2006
    row.prepend(new QDataItem(27, "mm, January 2006"));
    row.prepend(new QDataItem(18, "mm, February 2006"));
    row.prepend(new QDataItem(17, "mm, March 2006"));
    row.prepend(new QDataItem(26, "mm, April 2006"));
    row.prepend(new QDataItem(24, "mm, May 2006"));
    row.prepend(new QDataItem(18, "mm, June 2006"));
    row.prepend(new QDataItem(35, "mm, July 2006"));
    row.prepend(new QDataItem(28, "mm, August 2006"));
    row.prepend(new QDataItem(80, "mm, September 2006"));
    row.prepend(new QDataItem(52, "mm, October 2006"));
    row.prepend(new QDataItem(43, "mm, November 2006"));
    row.prepend(new QDataItem(44, "mm, December 2006"));
    data.prepend(row);
    row.clear();
    // 2007
    row.prepend(new QDataItem(41, "mm, January 2007"));
    row.prepend(new QDataItem(21, "mm, February 2007"));
    row.prepend(new QDataItem(30, "mm, March 2007"));
    row.prepend(new QDataItem(20, "mm, April 2007"));
    row.prepend(new QDataItem(53, "mm, May 2007"));
    row.prepend(new QDataItem(29, "mm, June 2007"));
    row.prepend(new QDataItem(139, "mm, July 2007"));
    row.prepend(new QDataItem(52, "mm, August 2007"));
    row.prepend(new QDataItem(51, "mm, September 2007"));
    row.prepend(new QDataItem(24, "mm, October 2007"));
    row.prepend(new QDataItem(47, "mm, November 2007"));
    row.prepend(new QDataItem(33, "mm, December 2007"));
    data.prepend(row);
    row.clear();
    // 2008
    row.prepend(new QDataItem(67, "mm, January 2008"));
    row.prepend(new QDataItem(19, "mm, February 2008"));
    row.prepend(new QDataItem(30, "mm, March 2008"));
    row.prepend(new QDataItem(31, "mm, April 2008"));
    row.prepend(new QDataItem(29, "mm, May 2008"));
    row.prepend(new QDataItem(79, "mm, June 2008"));
    row.prepend(new QDataItem(75, "mm, July 2008"));
    row.prepend(new QDataItem(99, "mm, August 2008"));
    row.prepend(new QDataItem(34, "mm, September 2008"));
    row.prepend(new QDataItem(52, "mm, October 2008"));
    row.prepend(new QDataItem(60, "mm, November 2008"));
    row.prepend(new QDataItem(20, "mm, December 2008"));
    data.prepend(row);
    row.clear();
    // 2009
    row.prepend(new QDataItem(9, "mm, January 2009"));
    row.prepend(new QDataItem(22, "mm, February 2009"));
    row.prepend(new QDataItem(11, "mm, March 2009"));
    row.prepend(new QDataItem(10, "mm, April 2009"));
    row.prepend(new QDataItem(69, "mm, May 2009"));
    row.prepend(new QDataItem(30, "mm, June 2009"));
    row.prepend(new QDataItem(78, "mm, July 2009"));
    row.prepend(new QDataItem(93, "mm, August 2009"));
    row.prepend(new QDataItem(70, "mm, September 2009"));
    row.prepend(new QDataItem(32, "mm, October 2009"));
    row.prepend(new QDataItem(56, "mm, November 2009"));
    row.prepend(new QDataItem(23, "mm, December 2009"));
    data.prepend(row);
    row.clear();
    // 2010
    row.prepend(new QDataItem(12, "mm, January 2010"));
    row.prepend(new QDataItem(28, "mm, February 2010"));
    row.prepend(new QDataItem(55, "mm, March 2010"));
    row.prepend(new QDataItem(20, "mm, April 2010"));
    row.prepend(new QDataItem(65, "mm, May 2010"));
    row.prepend(new QDataItem(26, "mm, June 2010"));
    row.prepend(new QDataItem(134, "mm, July 2010"));
    row.prepend(new QDataItem(57, "mm, August 2010"));
    row.prepend(new QDataItem(51, "mm, September 2010"));
    row.prepend(new QDataItem(53, "mm, October 2010"));
    row.prepend(new QDataItem(8, "mm, November 2010"));
    row.prepend(new QDataItem(9, "mm, December 2010"));
    data.prepend(row);
    row.clear();
    // 2011
    row.prepend(new QDataItem(34, "mm, January 2011"));
    row.prepend(new QDataItem(20, "mm, February 2011"));
    row.prepend(new QDataItem(30, "mm, March 2011"));
    row.prepend(new QDataItem(31, "mm, April 2011"));
    row.prepend(new QDataItem(42, "mm, May 2011"));
    row.prepend(new QDataItem(78, "mm, June 2011"));
    row.prepend(new QDataItem(85, "mm, July 2011"));
    row.prepend(new QDataItem(33, "mm, August 2011"));
    row.prepend(new QDataItem(42, "mm, September 2011"));
    row.prepend(new QDataItem(87, "mm, October 2011"));
    row.prepend(new QDataItem(41, "mm, November 2011"));
    row.prepend(new QDataItem(72, "mm, December 2011"));
    data.prepend(row);
    row.clear();
    // 2012
    row.prepend(new QDataItem(32, "mm, January 2012"));
    row.prepend(new QDataItem(42, "mm, February 2012"));
    row.prepend(new QDataItem(30, "mm, March 2012"));
    row.prepend(new QDataItem(50, "mm, April 2012"));
    row.prepend(new QDataItem(30, "mm, May 2012"));
    row.prepend(new QDataItem(70, "mm, June 2012"));
    row.prepend(new QDataItem(52, "mm, July 2012"));
    row.prepend(new QDataItem(20, "mm, August 2012"));
    row.prepend(new QDataItem(99, "mm, September 2012"));
    row.prepend(new QDataItem(70, "mm, October 2012"));
    row.prepend(new QDataItem(69, "mm, November 2012"));
    row.prepend(new QDataItem(49, "mm, December 2012"));
    data.prepend(row);
    row.clear();
    // Set up row and column names
    QVector<QString> months;
    months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    QVector<QString> years;
    years << "2000" << "2001" << "2002" << "2003" << "2004" << "2005" << "2006" << "2007" << "2008" << "2009" << "2010" << "2011" << "2012";
#endif
    m_chart->addDataSet(data, years, months);
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

void ChartDataGenerator::changeTheme()
{
    static int theme = 1; // TODO: System theme not yet implemented

    m_chart->setTheme((Q3DBars::ColorTheme)theme);

    if (++theme > (int)Q3DBars::ThemeLight)
        theme = 1; // TODO: System theme not yet implemented
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

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "rainfallchart.h"
#include "qcategoryaxis.h"
#include "qvalueaxis.h"
#include <QGuiApplication>
#include <QFont>
#include <QDebug>
#include <QTextStream>
#include <QFile>

#define CHANGE_CITY_WITH_TIMER

using namespace QtDataVis3D;

RainfallChart::RainfallChart(Q3DBars *rainfall)
    : m_chart(rainfall),
      m_city(2)
{
    // In data file the months are in numeric format, so create custom list
    for (int i = 1; i <= 12; i++)
        m_numericMonths << QString::number(i);

    m_columnCount = m_numericMonths.size();

    m_proxy = new QVariantBarDataProxy;
    m_proxy->setItemLabelFormat(QStringLiteral(" mm"));
    m_chart->setDataProxy(m_proxy);

    updateYearsList(2000, 2012);

    // Set up bar specifications; make the bars as wide as they are deep,
    // and add a small space between the bars
    m_chart->setBarSpecs(QSizeF(1.0f, 1.0f), QSizeF(0.2f, 0.2f), true);

    // Set axis labels and titles
    QStringList months;
    months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    m_chart->rowAxis()->setTitle("Year");
    m_chart->columnAxis()->setTitle("Month");
    m_chart->valueAxis()->setTitle(QString("rainfall (in mm) in city %1").arg(m_city - 1));
    m_chart->rowAxis()->setCategoryLabels(m_years);
    m_chart->columnAxis()->setCategoryLabels(months);

    // Set bar type to cylinder
    m_chart->setBarType(Cylinders, false);

    // Set shadows to medium
    m_chart->setShadowQuality(ShadowMedium);

    // Set font
    m_chart->setFont(QFont("Century Gothic", 40));

    // Set selection mode to bar and column
    //m_chart->setSelectionMode(ModeBarAndColumn);
    m_chart->setSelectionMode(ModeZoomColumn);

    // Set theme
    m_chart->setTheme(ThemeBlueNcs);

    // Set preset camera position
    m_chart->setCameraPreset(PresetIsometricRightHigh);

    // Disable grid
    m_chart->setGridVisible(false);

    // Set window title
    m_chart->setWindowTitle(QStringLiteral("Monthly rainfall in various cities"));
}

RainfallChart::~RainfallChart()
{
    delete m_mapping;
    delete m_dataSet;
    delete m_chart;
}

void RainfallChart::start()
{
    addDataSet();

#ifdef CHANGE_CITY_WITH_TIMER
    connect(&m_timer, &QTimer::timeout, this, &RainfallChart::timeout);
    m_timer.start(3000);
#endif
}

void RainfallChart::timeout()
{
    if (++m_city > 4)
        m_city = 2;

    m_proxy->mapping()->setValueIndex(m_city);
    m_chart->valueAxis()->setTitle(QString("rainfall (in mm) in city %1").arg(m_city - 1));
}

void RainfallChart::updateYearsList(int start, int end)
{
    m_years.clear();
    for (int i = start; i <= end; i++)
        m_years << QString::number(i);

    m_rowCount = m_years.size();

    // Set up sample space; make it match actual resolved data size
    m_chart->setupSampleSpace(m_rowCount, m_columnCount);
}

void RainfallChart::addDataSet()
{
    m_dataSet =  new QVariantDataSet;
    QVariantDataItemList *itemList = new QVariantDataItemList;
    QTextStream stream;
    QFile dataFile(":/data/raindata.txt");
    if (dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        stream.setDevice(&dataFile);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.startsWith("#"))
                continue;
            QStringList strList = line.split(",", QString::SkipEmptyParts);
            if (strList.size() < 5) {
                qWarning() << "Invalid row read from data:" << line;
                continue;
            }
            QVariantDataItem *newItem = new QVariantDataItem;
            for (int i = 0; i < 2; i++)
                newItem->append(strList.at(i).trimmed());
            for (int i = 2; i < 5; i++)
                newItem->append(strList.at(i).trimmed().toDouble());
            itemList->append(newItem);
        }
    } else {
        qWarning() << "Unable to open data file:" << dataFile.fileName();
    }

    m_dataSet->addItems(itemList);

    m_proxy->setDataSet(m_dataSet);

    m_mapping =  new QVariantBarDataMapping(0, 1, m_city, m_years, m_numericMonths);
    m_proxy->setMapping(m_mapping);
}

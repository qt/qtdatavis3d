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

#include "q3dbars.h"
#include "qcategoryaxis.h"
#include "qvariantbardataproxy.h"

#include <QGuiApplication>
#include <QFont>
#include <QDebug>
#include <QTextStream>
#include <QFile>

using namespace QtDataVis3D;

class RainfallChart : public QObject
{
public:
    explicit RainfallChart(Q3DBars *rainfall);
    ~RainfallChart();

    void addDataSet();
    void start();

private:
    Q3DBars *m_chart;
    int m_columnCount;
    int m_rowCount;
    QStringList m_years;
};

RainfallChart::RainfallChart(Q3DBars *rainfall)
    : m_chart(rainfall),
      m_columnCount(12),
      m_rowCount(13)
{
    // Set up bar specifications; make the bars as wide as they are deep,
    // and add a small space between the bars
    m_chart->setBarSpecs(QSizeF(1.0f, 1.0f), QSizeF(0.2f, 0.2f), true);

    // Set up sample space; make it match actual data size
    m_chart->setupSampleSpace(m_rowCount, m_columnCount);

    // Set axis labels and titles
    QStringList months;
    months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    m_years << "2000" << "2001" << "2002" << "2003" << "2004" << "2005" << "2006" << "2007" << "2008" << "2009" << "2010" << "2011" << "2012";

    m_chart->rowAxis()->setTitle("Year");
    m_chart->columnAxis()->setTitle("Month");
    m_chart->valueAxis()->setTitle("rainfall (in mm)");
    m_chart->rowAxis()->setLabels(m_years);
    m_chart->columnAxis()->setLabels(months);

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
    m_chart->setWindowTitle(QStringLiteral("Monthly rainfall in Northern Finland (2000-2012)"));

    QVariantBarDataProxy *proxy = new QVariantBarDataProxy;
    m_chart->setDataProxy(proxy);
}

RainfallChart::~RainfallChart()
{
    delete m_chart;
}

void RainfallChart::start()
{
    addDataSet();
}

void RainfallChart::addDataSet()
{
    QVariantDataSet *dataSet =  new QVariantDataSet;
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
            if (strList.size() < 3) {
                qWarning() << "Invalid row read from data:" << line;
                continue;
            }
            QVariantDataItem *newItem = new QVariantDataItem;
            for (int i = 0; i < 3; i++)
                newItem->append(strList.at(i));
            itemList->append(newItem);
        }
    } else {
        qWarning() << "Unable to open data file:" << dataFile.fileName();
    }

    dataSet->addItems(itemList);

    static_cast<QVariantBarDataProxy *>(m_chart->dataProxy())->setDataSet(dataSet);

    // In data file the months are in numeric format, so create custom list
    QStringList numericMonths;
    for (int i = 1; i <= 12; i++)
        numericMonths << QString::number(i);

    QVariantBarMapping mappings;
    mappings[QVariantBarMappingItem::MapRow] = QVariantBarMappingItem(0, m_years);
    mappings[QVariantBarMappingItem::MapColumn] = QVariantBarMappingItem(1, numericMonths);
    mappings[QVariantBarMappingItem::MapValue] = QVariantBarMappingItem(2);
    static_cast<QVariantBarDataProxy *>(m_chart->dataProxy())->setMappings(mappings);
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    Q3DBars rainfall;
    rainfall.resize(1280, 800);
    rainfall.setPosition(QPoint(10, 30));
    rainfall.show();

    RainfallChart *rainfallchart = new RainfallChart(&rainfall);
    rainfallchart->start();

    return app.exec();
}

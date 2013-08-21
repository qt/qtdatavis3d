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

#include "rainfallchart.h"
#include <QtDataVis3D/qcategoryaxis.h>
#include <QtDataVis3D/qvalueaxis.h>
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

    m_proxy = new VariantBarDataProxy;
    m_chart->setDataProxy(m_proxy);

    updateYearsList(2000, 2012);

    // Set up bar specifications; make the bars as wide as they are deep,
    // and add a small space between the bars
    m_chart->setBarSpecs(1.0, QSizeF(0.2, 0.2), true);

    // Set axis labels and titles
    QStringList months;
    months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    m_chart->rowAxis()->setTitle("Year");
    m_chart->columnAxis()->setTitle("Month");
    m_chart->valueAxis()->setTitle(QString("rainfall in city %1").arg(m_city - 1));
    m_chart->valueAxis()->setLabelFormat("%d mm");
    m_chart->rowAxis()->setCategoryLabels(m_years);
    m_chart->columnAxis()->setCategoryLabels(months);

    // Set bar type to cylinder
    m_chart->setBarType(QDataVis::Cylinders, false);

    // Set shadows to medium
    m_chart->setShadowQuality(QDataVis::ShadowMedium);

    // Set font
    m_chart->setFont(QFont("Century Gothic", 40));

    // Set selection mode to bar and column
    //m_chart->setSelectionMode(ModeItemAndColumn);
    m_chart->setSelectionMode(QDataVis::ModeSliceColumn);

    // Set theme
    m_chart->setTheme(QDataVis::ThemeBlueNcs);

    // Set preset camera position
    m_chart->setCameraPreset(QDataVis::PresetIsometricRightHigh);

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
    m_chart->valueAxis()->setTitle(QString("rainfall in city %1").arg(m_city - 1));
}

void RainfallChart::updateYearsList(int start, int end)
{
    m_years.clear();
    for (int i = start; i <= end; i++)
        m_years << QString::number(i);

    m_rowCount = m_years.size();

    // Set up sample space; make it match actual resolved data size
    m_chart->setDataWindow(m_rowCount, m_columnCount);
}

void RainfallChart::addDataSet()
{
    m_dataSet =  new VariantDataSet;
    VariantDataItemList *itemList = new VariantDataItemList;
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
            VariantDataItem *newItem = new VariantDataItem;
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

    m_mapping =  new VariantBarDataMapping(0, 1, m_city, m_years, m_numericMonths);
    m_proxy->setMapping(m_mapping);
}

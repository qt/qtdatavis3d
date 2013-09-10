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

#include "chart.h"
#include <QtDataVisualization/q3dcategoryaxis.h>
#include <QtDataVisualization/q3dvalueaxis.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QTime>

QT_DATAVISUALIZATION_USE_NAMESPACE

const QString celsiusString = QString(QChar(0xB0)) + "C";

ChartModifier::ChartModifier(Q3DBars *barchart)
    : m_chart(barchart),
      m_xRotation(0.0f),
      m_yRotation(0.0f),
      m_fontSize(20),
      m_segments(4),
      m_subSegments(3),
      m_minval(-20.0), // TODO Barchart Y-axis currently only properly supports zero-centered ranges
      m_maxval(20.0),
      m_temperatureAxis(new Q3DValueAxis),
      m_yearAxis(new Q3DCategoryAxis),
      m_monthAxis(new Q3DCategoryAxis),
      m_temperatureData(new QBarDataProxy)
{
    m_months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    m_years << "2006" << "2007" << "2008" << "2009" << "2010" << "2011" << "2012";

    m_chart->setBackgroundVisible(false);

    m_temperatureAxis->setTitle("Average temperature");
    m_temperatureAxis->setSegmentCount(m_segments);
    m_temperatureAxis->setSubSegmentCount(m_subSegments);
    m_temperatureAxis->setRange(m_minval, m_maxval);
    m_temperatureAxis->setLabelFormat(QString(QStringLiteral("%d ") + celsiusString));

    m_yearAxis->setTitle("Year");
    m_yearAxis->setCategoryLabels(m_years);

    m_monthAxis->setTitle("Month");
    m_monthAxis->setCategoryLabels(m_months);

    m_chart->addAxis(m_temperatureAxis);
    m_chart->addAxis(m_yearAxis);
    m_chart->addAxis(m_monthAxis);

    m_chart->setShadowQuality(QDataVis::ShadowSoftMedium);

    m_temperatureData->setItemLabelFormat(QStringLiteral("@valueTitle for @colLabel @rowLabel: @valueLabel"));

    m_chart->addDataProxy(m_temperatureData);

    m_chart->setFont(QFont("Times Roman", 20));

    m_chart->setSelectionMode(QDataVis::ModeItem);

    resetTemperatureData();
}

ChartModifier::~ChartModifier()
{
    delete m_chart;
}

void ChartModifier::start()
{
    m_chart->setActiveDataProxy(m_temperatureData);

    m_chart->setTitle(QStringLiteral("Average temperatures in Oulu, Finland (2006-2012)"));

    m_chart->setValueAxis(m_temperatureAxis);
    m_chart->setRowAxis(m_yearAxis);
    m_chart->setColumnAxis(m_monthAxis);

    m_chart->setDataWindow(m_years.size(), m_months.size());
}

void ChartModifier::resetTemperatureData()
{
    // Set up data
    static const float temp[7][12] = {
        {-6.7f, -11.7f, -9.7f, 3.3f, 9.2f, 14.0f, 16.3f, 17.8f, 10.2f, 2.1f, -2.6f, -0.3f},    // 2006
        {-6.8f, -13.3f, 0.2f, 1.5f, 7.9f, 13.4f, 16.1f, 15.5f, 8.2f, 5.4f, -2.6f, -0.8f},      // 2007
        {-4.2f, -4.0f, -4.6f, 1.9f, 7.3f, 12.5f, 15.0f, 12.8f, 7.6f, 5.1f, -0.9f, -1.3f},      // 2008
        {-7.8f, -8.8f, -4.2f, 0.7f, 9.3f, 13.2f, 15.8f, 15.5f, 11.2f, 0.6f, 0.7f, -8.4f},      // 2009
        {-14.4f, -12.1f, -7.0f, 2.3f, 11.0f, 12.6f, 18.8f, 13.8f, 9.4f, 3.9f, -5.6f, -13.0f},  // 2010
        {-9.0f, -15.2f, -3.8f, 2.6f, 8.3f, 15.9f, 18.6f, 14.9f, 11.1f, 5.3f, 1.8f, -0.2f},     // 2011
        {-8.7f, -11.3f, -2.3f, 0.4f, 7.5f, 12.2f, 16.4f, 14.1f, 9.2f, 3.1f, 0.3f, -12.1f}      // 2012
    };

    // Create data rows
    QBarDataArray *dataSet = new QBarDataArray;
    QBarDataRow *dataRow;

    dataSet->reserve(m_years.size());
    for (int year = 0; year < m_years.size(); year++) {
        dataRow = new QBarDataRow(m_months.size());
        // Create data items
        for (int month = 0; month < m_months.size(); month++) {
            // Add data to rows
            (*dataRow)[month].setValue(temp[year][month]);
        }
        // Add row to set
        dataSet->append(dataRow);
    }

    // Add data to chart (chart assumes ownership)
    m_temperatureData->resetArray(dataSet);
}

void ChartModifier::changeStyle()
{
    static int model = 9;
    switch (model) {
    case 0:
        m_chart->setBarType(QDataVis::Cylinders, false);
        break;
    case 1:
        m_chart->setBarType(QDataVis::Cylinders, true);
        break;
    case 2:
        m_chart->setBarType(QDataVis::Cones, false);
        break;
    case 3:
        m_chart->setBarType(QDataVis::Cones, true);
        break;
    case 4:
        m_chart->setBarType(QDataVis::Bars, false);
        break;
    case 5:
        m_chart->setBarType(QDataVis::Bars, true);
        break;
    case 6:
        m_chart->setBarType(QDataVis::Pyramids, false);
        break;
    case 7:
        m_chart->setBarType(QDataVis::Pyramids, true);
        break;
    case 8:
        m_chart->setBarType(QDataVis::BevelBars, false);
        break;
    case 9:
        m_chart->setBarType(QDataVis::BevelBars, true);
        break;
    }
    model++;
    if (model > 9)
        model = 0;
}

void ChartModifier::changePresetCamera()
{
    static int preset = QDataVis::PresetFrontLow;

    m_chart->setCameraPreset((QDataVis::CameraPreset)preset);

    if (++preset > QDataVis::PresetDirectlyBelow)
        preset = QDataVis::PresetFrontLow;
}

void ChartModifier::changeTheme(int theme)
{
    m_chart->setTheme((QDataVis::ColorTheme)theme);
}

void ChartModifier::changeTransparency()
{
    static int transparency = QDataVis::TransparencyNone;

    m_chart->setLabelTransparency((QDataVis::LabelTransparency)transparency);

    if (++transparency > QDataVis::TransparencyNoBackground)
        transparency = QDataVis::TransparencyNone;
}

void ChartModifier::changeSelectionMode()
{
    static int selectionMode = m_chart->selectionMode();

    if (++selectionMode > QDataVis::ModeSliceColumn)
        selectionMode = QDataVis::ModeNone;

    m_chart->setSelectionMode((QDataVis::SelectionMode)selectionMode);
}

void ChartModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSize(m_fontSize);
    m_chart->setFont(newFont);
}

void ChartModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_chart->font();
    font.setPointSize(m_fontSize);
    m_chart->setFont(font);
}

void ChartModifier::shadowQualityUpdatedByVisual(QDataVis::ShadowQuality sq)
{
    int quality = int(sq);
    // Updates the UI component to show correct shadow quality
    emit shadowQualityChanged(quality);
}

void ChartModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowQuality(quality);
    m_chart->setShadowQuality(sq);
    emit shadowQualityChanged(quality);
}

void ChartModifier::rotateX(int rotation)
{
    m_xRotation = rotation;
    m_chart->setCameraPosition(m_xRotation, m_yRotation);
}

void ChartModifier::rotateY(int rotation)
{
    m_yRotation = rotation;
    m_chart->setCameraPosition(m_xRotation, m_yRotation);
}

void ChartModifier::setBackgroundEnabled(int enabled)
{
    m_chart->setBackgroundVisible((bool)enabled);
}

void ChartModifier::setGridEnabled(int enabled)
{
    m_chart->setGridVisible((bool)enabled);
}

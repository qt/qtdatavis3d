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
#include <QtDataVisualization/q3dcategoryaxis.h>
#include <QtDataVisualization/q3dvalueaxis.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qbar3dseries.h>
#include <QTime>
#include <QComboBox>

QT_DATAVISUALIZATION_USE_NAMESPACE

const QString celsiusString = QString(QChar(0xB0)) + "C";

//! [0]
GraphModifier::GraphModifier(Q3DBars *bargraph)
    : m_graph(bargraph),
      m_xRotation(0.0),
      m_yRotation(0.0),
      m_fontSize(30),
      m_segments(4),
      m_subSegments(3),
      m_minval(-20.0),
      m_maxval(20.0),
      //! [1]
      m_temperatureAxis(new Q3DValueAxis),
      m_yearAxis(new Q3DCategoryAxis),
      m_monthAxis(new Q3DCategoryAxis),
      m_primaryData(new QBarDataProxy),
      m_secondaryData(new QBarDataProxy),
      //! [1]
      m_style(QDataVis::MeshStyleBevelBars),
      m_smooth(false)
{
    //! [2]
    m_graph->setBackgroundVisible(false);
    m_graph->setShadowQuality(QDataVis::ShadowQualitySoftMedium);
    m_graph->setFont(QFont("Times New Roman", m_fontSize));
    m_graph->setLabelStyle(QDataVis::LabelStyleFromTheme);
    //! [2]

    m_months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    m_years << "2006" << "2007" << "2008" << "2009" << "2010" << "2011" << "2012";

    //! [3]
    m_temperatureAxis->setTitle("Average temperature");
    m_temperatureAxis->setSegmentCount(m_segments);
    m_temperatureAxis->setSubSegmentCount(m_subSegments);
    m_temperatureAxis->setRange(m_minval, m_maxval);
    m_temperatureAxis->setLabelFormat(QString(QStringLiteral("%d ") + celsiusString));

    m_yearAxis->setTitle("Year");
    m_monthAxis->setTitle("Month");

    m_graph->addAxis(m_temperatureAxis);
    m_graph->addAxis(m_yearAxis);
    m_graph->addAxis(m_monthAxis);
    //! [3]

    QBar3DSeries *series = new QBar3DSeries(m_primaryData);
    series->setItemLabelFormat(QStringLiteral("Oulu - @colLabel @rowLabel: @valueLabel"));
    QBar3DSeries *series2 = new QBar3DSeries(m_secondaryData);
    series2->setItemLabelFormat(QStringLiteral("Helsinki - @colLabel @rowLabel: @valueLabel"));
    series2->setVisible(false);

    //! [4]
    m_graph->addSeries(series);
    m_graph->addSeries(series2);
    //! [4]

    changePresetCamera();
    resetTemperatureData();
}
//! [0]

GraphModifier::~GraphModifier()
{
    delete m_graph;
}

void GraphModifier::start()
{
    //! [6]
    m_graph->setValueAxis(m_temperatureAxis);
    m_graph->setRowAxis(m_yearAxis);
    m_graph->setColumnAxis(m_monthAxis);
    //! [6]
}

void GraphModifier::resetTemperatureData()
{
    //! [5]
    // Set up data
    static const qreal tempOulu[7][12] = {
        {-6.7, -11.7, -9.7, 3.3, 9.2, 14.0, 16.3, 17.8, 10.2, 2.1, -2.6, -0.3},    // 2006
        {-6.8, -13.3, 0.2, 1.5, 7.9, 13.4, 16.1, 15.5, 8.2, 5.4, -2.6, -0.8},      // 2007
        {-4.2, -4.0, -4.6, 1.9, 7.3, 12.5, 15.0, 12.8, 7.6, 5.1, -0.9, -1.3},      // 2008
        {-7.8, -8.8, -4.2, 0.7, 9.3, 13.2, 15.8, 15.5, 11.2, 0.6, 0.7, -8.4},      // 2009
        {-14.4, -12.1, -7.0, 2.3, 11.0, 12.6, 18.8, 13.8, 9.4, 3.9, -5.6, -13.0},  // 2010
        {-9.0, -15.2, -3.8, 2.6, 8.3, 15.9, 18.6, 14.9, 11.1, 5.3, 1.8, -0.2},     // 2011
        {-8.7, -11.3, -2.3, 0.4, 7.5, 12.2, 16.4, 14.1, 9.2, 3.1, 0.3, -12.1}      // 2012
    };

    static const qreal tempHelsinki[7][12] = {
        {-3.7, -7.8, -5.4, 3.4, 10.7, 15.4, 18.6, 18.7, 14.3, 8.5, 2.9, 4.1},      // 2006
        {-1.2, -7.5, 3.1, 5.5, 10.3, 15.9, 17.4, 17.9, 11.2, 7.3, 1.1, 0.5},       // 2007
        {-0.6, 1.2, 0.2, 6.3, 10.2, 13.8, 18.1, 15.1, 10.1, 9.4, 2.5, 0.4},        // 2008
        {-2.9, -3.5, -0.9, 4.7, 10.9, 14.0, 17.4, 16.8, 13.2, 4.1, 2.6, -2.3},     // 2009
        {-10.2, -8.0, -1.9, 6.6, 11.3, 14.5, 21.0, 18.8, 12.6, 6.1, -0.5, -7.3},   // 2010
        {-4.4, -9.1, -2.0, 5.5, 9.9, 15.6, 20.8, 17.8, 13.4, 8.9, 3.6, 1.5},       // 2011
        {-3.5, -3.2, -0.7, 4.0, 11.1, 13.4, 17.3, 15.8, 13.1, 6.4, 4.1, -5.1}      // 2012
    };

    // Create data arrays
    QBarDataArray *dataSet = new QBarDataArray;
    QBarDataArray *dataSet2 = new QBarDataArray;
    QBarDataRow *dataRow;
    QBarDataRow *dataRow2;

    dataSet->reserve(m_years.size());
    for (int year = 0; year < m_years.size(); year++) {
        // Create a data row
        dataRow = new QBarDataRow(m_months.size());
        dataRow2 = new QBarDataRow(m_months.size());
        for (int month = 0; month < m_months.size(); month++) {
            // Add data to the row
            (*dataRow)[month].setValue(tempOulu[year][month]);
            (*dataRow2)[month].setValue(tempHelsinki[year][month]);
        }
        // Add the row to the set
        dataSet->append(dataRow);
        dataSet2->append(dataRow2);
    }

    // Add data to the graph (the graph assumes ownership of it)
    m_primaryData->resetArray(dataSet, m_years, m_months);
    m_secondaryData->resetArray(dataSet2, m_years, m_months);
    //! [5]
}

void GraphModifier::changeStyle(int style)
{
    m_style = QDataVis::MeshStyle(style);
    m_graph->setBarType(m_style, m_smooth);
}

void GraphModifier::changePresetCamera()
{
    static int preset = QDataVis::CameraPresetFront;

    m_graph->scene()->activeCamera()->setCameraPreset((QDataVis::CameraPreset)preset);

    if (++preset > QDataVis::CameraPresetDirectlyBelow)
        preset = QDataVis::CameraPresetFrontLow;
}

void GraphModifier::changeTheme(int theme)
{
    m_graph->setTheme((QDataVis::Theme)theme);
}

void GraphModifier::changeLabelStyle()
{
    static int style = QDataVis::LabelStyleFromTheme;

    m_graph->setLabelStyle((QDataVis::LabelStyle)style);

    if (++style > QDataVis::LabelStyleTransparent)
        style = QDataVis::LabelStyleOpaque;
}

void GraphModifier::changeSelectionMode(int selectionMode)
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (comboBox) {
        int flags = comboBox->itemData(selectionMode).toInt();
        m_graph->setSelectionMode(QDataVis::SelectionFlags(flags));
    }
}

void GraphModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSize(m_fontSize);
    m_graph->setFont(newFont);
}

void GraphModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_graph->font();
    font.setPointSize(m_fontSize);
    m_graph->setFont(font);
}

void GraphModifier::shadowQualityUpdatedByVisual(QDataVis::ShadowQuality sq)
{
    int quality = int(sq);
    // Updates the UI component to show correct shadow quality
    emit shadowQualityChanged(quality);
}

void GraphModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
    emit shadowQualityChanged(quality);
}

//! [7]
void GraphModifier::rotateX(int rotation)
{
    m_xRotation = rotation;
    m_graph->scene()->activeCamera()->setCameraPosition(m_xRotation, m_yRotation);
}

void GraphModifier::rotateY(int rotation)
{
    m_yRotation = rotation;
    m_graph->scene()->activeCamera()->setCameraPosition(m_xRotation, m_yRotation);
}
//! [7]

void GraphModifier::setBackgroundEnabled(int enabled)
{
    m_graph->setBackgroundVisible(bool(enabled));
}

void GraphModifier::setGridEnabled(int enabled)
{
    m_graph->setGridVisible(bool(enabled));
}

void GraphModifier::setSmoothBars(int smooth)
{
    m_smooth = bool(smooth);
    m_graph->setBarType(m_style, m_smooth);
}

void GraphModifier::setSeriesVisibility(int enabled)
{
    m_graph->seriesList().at(1)->setVisible(bool(enabled));
    if (enabled) {
        m_graph->setBarThickness(2.0);
        m_graph->setBarSpacing(QSizeF(1.0, 3.0));
    } else {
        m_graph->setBarThickness(1.0);
        m_graph->setBarSpacing(QSizeF(1.0, 1.0));
    }
}

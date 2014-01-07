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
#include <QtDataVisualization/q3dtheme.h>
#include <QTime>
#include <QComboBox>

QT_DATAVISUALIZATION_USE_NAMESPACE

const QString celsiusString = QString(QChar(0xB0)) + "C";

//! [0]
GraphModifier::GraphModifier(Q3DBars *bargraph)
    : m_graph(bargraph),
      m_xRotation(0.0f),
      m_yRotation(0.0f),
      m_fontSize(30),
      m_segments(4),
      m_subSegments(3),
      m_minval(-20.0f),
      m_maxval(20.0f),
      //! [1]
      m_temperatureAxis(new Q3DValueAxis),
      m_yearAxis(new Q3DCategoryAxis),
      m_monthAxis(new Q3DCategoryAxis),
      m_primaryData(new QBarDataProxy),
      m_secondaryData(new QBarDataProxy),
      //! [1]
      m_primaryStyle(QAbstract3DSeries::MeshBevelBar),
      m_secondaryStyle(QAbstract3DSeries::MeshSphere),
      m_smooth(false)
{
    //! [2]
    m_graph->setShadowQuality(QDataVis::ShadowQualitySoftMedium);
    m_graph->activeTheme()->setBackgroundEnabled(false);
    m_graph->activeTheme()->setFont(QFont("Times New Roman", m_fontSize));
    m_graph->activeTheme()->setLabelBackgroundEnabled(true);
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

    m_graph->setValueAxis(m_temperatureAxis);
    m_graph->setRowAxis(m_yearAxis);
    m_graph->setColumnAxis(m_monthAxis);
    //! [3]

    //! [8]
    QBar3DSeries *series = new QBar3DSeries(m_primaryData);
    series->setItemLabelFormat(QStringLiteral("Oulu - @colLabel @rowLabel: @valueLabel"));
    series->setMesh(QAbstract3DSeries::MeshBevelBar);
    series->setMeshSmooth(false);

    QBar3DSeries *series2 = new QBar3DSeries(m_secondaryData);
    series2->setItemLabelFormat(QStringLiteral("Helsinki - @colLabel @rowLabel: @valueLabel"));
    series2->setMesh(QAbstract3DSeries::MeshSphere);
    series2->setMeshSmooth(false);
    series2->setVisible(false);
    //! [8]

    //! [4]
    m_graph->addSeries(series);
    m_graph->addSeries(series2);
    //! [4]

    //! [6]
    changePresetCamera();
    //! [6]

    //! [9]
    resetTemperatureData();
    //! [9]
}
//! [0]

GraphModifier::~GraphModifier()
{
    delete m_graph;
}

void GraphModifier::resetTemperatureData()
{
    //! [5]
    // Set up data
    static const float tempOulu[7][12] = {
        {-6.7f, -11.7f, -9.7f, 3.3f, 9.2f, 14.0f, 16.3f, 17.8f, 10.2f, 2.1f, -2.6f, -0.3f},    // 2006
        {-6.8f, -13.3f, 0.2f, 1.5f, 7.9f, 13.4f, 16.1f, 15.5f, 8.2f, 5.4f, -2.6f, -0.8f},      // 2007
        {-4.2f, -4.0f, -4.6f, 1.9f, 7.3f, 12.5f, 15.0f, 12.8f, 7.6f, 5.1f, -0.9f, -1.3f},      // 2008
        {-7.8f, -8.8f, -4.2f, 0.7f, 9.3f, 13.2f, 15.8f, 15.5f, 11.2f, 0.6f, 0.7f, -8.4f},      // 2009
        {-14.4f, -12.1f, -7.0f, 2.3f, 11.0f, 12.6f, 18.8f, 13.8f, 9.4f, 3.9f, -5.6f, -13.0f},  // 2010
        {-9.0f, -15.2f, -3.8f, 2.6f, 8.3f, 15.9f, 18.6f, 14.9f, 11.1f, 5.3f, 1.8f, -0.2f},     // 2011
        {-8.7f, -11.3f, -2.3f, 0.4f, 7.5f, 12.2f, 16.4f, 14.1f, 9.2f, 3.1f, 0.3f, -12.1f}      // 2012
    };

    static const float tempHelsinki[7][12] = {
        {-3.7f, -7.8f, -5.4f, 3.4f, 10.7f, 15.4f, 18.6f, 18.7f, 14.3f, 8.5f, 2.9f, 4.1f},      // 2006
        {-1.2f, -7.5f, 3.1f, 5.5f, 10.3f, 15.9f, 17.4f, 17.9f, 11.2f, 7.3f, 1.1f, 0.5f},       // 2007
        {-0.6f, 1.2f, 0.2f, 6.3f, 10.2f, 13.8f, 18.1f, 15.1f, 10.1f, 9.4f, 2.5f, 0.4f},        // 2008
        {-2.9f, -3.5f, -0.9f, 4.7f, 10.9f, 14.0f, 17.4f, 16.8f, 13.2f, 4.1f, 2.6f, -2.3f},     // 2009
        {-10.2f, -8.0f, -1.9f, 6.6f, 11.3f, 14.5f, 21.0f, 18.8f, 12.6f, 6.1f, -0.5f, -7.3f},   // 2010
        {-4.4f, -9.1f, -2.0f, 5.5f, 9.9f, 15.6f, 20.8f, 17.8f, 13.4f, 8.9f, 3.6f, 1.5f},       // 2011
        {-3.5f, -3.2f, -0.7f, 4.0f, 11.1f, 13.4f, 17.3f, 15.8f, 13.1f, 6.4f, 4.1f, -5.1f}      // 2012
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
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (comboBox) {
        m_primaryStyle = QAbstract3DSeries::Mesh(comboBox->itemData(style).toInt());
        if (m_graph->seriesList().size())
            m_graph->seriesList().at(0)->setMesh(m_primaryStyle);
    }
}

void GraphModifier::changePresetCamera()
{
    //! [10]
    static int preset = Q3DCamera::CameraPresetFront;

    m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);

    if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
        preset = Q3DCamera::CameraPresetFrontLow;
    //! [10]
}

void GraphModifier::changeTheme(int theme)
{
    Q3DTheme *currentTheme = m_graph->activeTheme();
    currentTheme->setType(Q3DTheme::Theme(theme));
    emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
    emit gridEnabledChanged(currentTheme->isGridEnabled());
    emit fontChanged(currentTheme->font());
    emit fontSizeChanged(currentTheme->font().pointSize());
}

void GraphModifier::changeLabelBackground()
{
    m_graph->activeTheme()->setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
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
    m_graph->activeTheme()->setFont(newFont);
}

void GraphModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(m_fontSize);
    m_graph->activeTheme()->setFont(font);
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
    m_graph->activeTheme()->setBackgroundEnabled(bool(enabled));
}

void GraphModifier::setGridEnabled(int enabled)
{
    m_graph->activeTheme()->setGridEnabled(bool(enabled));
}

void GraphModifier::setSmoothBars(int smooth)
{
    m_smooth = bool(smooth);
    if (m_graph->seriesList().size()) {
        m_graph->seriesList().at(0)->setMeshSmooth(m_smooth);
        m_graph->seriesList().at(1)->setMeshSmooth(m_smooth);
    }
}

void GraphModifier::setSeriesVisibility(int enabled)
{
    m_graph->seriesList().at(1)->setVisible(bool(enabled));
    if (enabled) {
        m_graph->setBarThickness(2.0f);
        m_graph->setBarSpacing(QSizeF(1.0, 3.0));
    } else {
        m_graph->setBarThickness(1.0f);
        m_graph->setBarSpacing(QSizeF(1.0, 1.0));
    }
}

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

#include "chart.h"
#include <QtDataVis3D/qcategoryaxis.h>
#include <QtDataVis3D/qvalueaxis.h>
#include <QtDataVis3D/qbardataproxy.h>
#include <QTime>

QT_DATAVIS3D_USE_NAMESPACE

const QString celsiusString = QString(QChar(0xB0)) + "C";

ChartModifier::ChartModifier(Q3DBars *barchart)
    : m_chart(barchart),
      m_columnCount(21),
      m_rowCount(21),
      m_xRotation(0.0f),
      m_yRotation(0.0f),
      m_static(true),
      m_barThicknessRatio(1.0),
      m_barSpacingX(0.1f),
      m_barSpacingZ(0.1f),
      m_fontSize(20),
      m_segments(4),
      m_subSegments(3),
      m_minval(-20.0), // TODO Barchart Y-axis currently only properly supports zero-centered ranges
      m_maxval(20.0),
      m_selectedBarPos(-1, -1),
      m_autoAdjustingAxis(new QValueAxis),
      m_fixedRangeAxis(new QValueAxis),
      m_temperatureAxis(new QValueAxis),
      m_yearAxis(new QCategoryAxis),
      m_monthAxis(new QCategoryAxis),
      m_genericRowAxis(new QCategoryAxis),
      m_genericColumnAxis(new QCategoryAxis),
      m_temperatureData(new QBarDataProxy),
      m_genericData(new QBarDataProxy),
      m_currentAxis(m_fixedRangeAxis)
{
    // Don't set any styles or specifications, start from defaults
    // Generate generic labels
    QStringList genericRowLabels;
    QStringList genericColumnLabels;
    for (int i = 0; i < 200; i++) {
        if (i % 5)
            genericRowLabels << QString();
        else
            genericRowLabels << QStringLiteral("Row %1").arg(i);
    }
    for (int i = 0; i < 200; i++) {
        if (i % 5)
            genericColumnLabels << QString();
        else
            genericColumnLabels << QStringLiteral("Column %1").arg(i);
    }

    m_months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    m_years << "2006" << "2007" << "2008" << "2009" << "2010" << "2011" << "2012";

    QString labelFormat(QStringLiteral("%.3f"));
    QString axisTitle("Generic Value");

    m_autoAdjustingAxis->setLabelFormat(labelFormat);
    m_autoAdjustingAxis->setTitle(axisTitle);
    m_autoAdjustingAxis->setSegmentCount(m_segments * 2);
    m_autoAdjustingAxis->setSubSegmentCount(1);
    m_autoAdjustingAxis->setAutoAdjustRange(true);

    m_fixedRangeAxis->setLabelFormat(labelFormat);
    m_fixedRangeAxis->setTitle(axisTitle);
    m_fixedRangeAxis->setSegmentCount(m_segments);
    m_fixedRangeAxis->setSubSegmentCount(m_subSegments);
    m_fixedRangeAxis->setRange(0.0, 100.0);

    m_genericRowAxis->setTitle("Generic Row");
    m_genericRowAxis->setCategoryLabels(genericRowLabels);

    m_genericColumnAxis->setTitle("Generic Column");
    m_genericColumnAxis->setCategoryLabels(genericColumnLabels);

    m_temperatureAxis->setTitle("Average temperature");
    m_temperatureAxis->setSegmentCount(m_segments);
    m_temperatureAxis->setSubSegmentCount(m_subSegments);
    m_temperatureAxis->setRange(m_minval, m_maxval);
    m_temperatureAxis->setLabelFormat(QString(QStringLiteral("%d ") + celsiusString));

    m_yearAxis->setTitle("Year");
    m_yearAxis->setCategoryLabels(m_years);

    m_monthAxis->setTitle("Month");
    m_monthAxis->setCategoryLabels(m_months);

    m_chart->addAxis(m_autoAdjustingAxis);
    m_chart->addAxis(m_fixedRangeAxis);
    m_chart->addAxis(m_temperatureAxis);
    m_chart->addAxis(m_yearAxis);
    m_chart->addAxis(m_monthAxis);
    m_chart->addAxis(m_genericRowAxis);
    m_chart->addAxis(m_genericColumnAxis);

    m_temperatureData->setItemLabelFormat(QStringLiteral("@valueTitle for @colLabel @rowLabel: @valueLabel"));
    m_genericData->setItemLabelFormat(QStringLiteral("@valueTitle for (@rowIdx, @colIdx): @valueLabel"));

    m_chart->addDataProxy(m_temperatureData);
    m_chart->addDataProxy(m_genericData);

    m_chart->setFont(QFont("Times Roman", 20));

    resetTemperatureData();
}

ChartModifier::~ChartModifier()
{
    delete m_chart;
}

void ChartModifier::start()
{
    restart(false);
}

void ChartModifier::restart(bool dynamicData)
{
    m_static = !dynamicData;

    if (m_static) {
        m_chart->setActiveDataProxy(m_temperatureData);

        m_chart->setTitle(QStringLiteral("Average temperatures in Oulu, Finland (2006-2012)"));

        m_chart->setValueAxis(m_temperatureAxis);
        m_chart->setRowAxis(m_yearAxis);
        m_chart->setColumnAxis(m_monthAxis);

        m_chart->setDataWindow(m_years.size(), m_months.size());
        m_chart->setSelectionMode(QDataVis::ModeItem);
    } else {
        m_chart->setActiveDataProxy(m_genericData);

        m_chart->setTitle(QStringLiteral("Generic data"));

        m_chart->setValueAxis(m_currentAxis);
        m_chart->setRowAxis(m_genericRowAxis);
        m_chart->setColumnAxis(m_genericColumnAxis);

        m_chart->setDataWindow(m_rowCount, m_columnCount);
        m_chart->setSelectionMode(QDataVis::ModeItem);
    }
}

void ChartModifier::selectBar()
{
    QPoint targetBar(5, 5);
    QPoint noSelection(-1, -1);
    if (m_selectedBarPos != targetBar)
        m_chart->setSelectedBarPos(targetBar);
    else
        m_chart->setSelectedBarPos(noSelection);
}

void ChartModifier::swapAxis()
{
    static int counter = 0;
    int state = ++counter % 3;

    if (state == 0) {
        m_currentAxis = m_fixedRangeAxis;
        qDebug() << "Fixed range axis";
    } else if (state == 1) {
        m_currentAxis = m_autoAdjustingAxis;
        qDebug() << "Automatic range axis";
    } else {
        m_currentAxis = 0;
        qDebug() << "default axis";
    }

    m_chart->setValueAxis(m_currentAxis);
}

void ChartModifier::releaseAxes()
{
    // Releases all axes - results in default axes for all dimensions.
    // Axes reset when the graph is switched as set*Axis calls are made, which
    // implicitly add axes.
    m_chart->releaseAxis(m_autoAdjustingAxis);
    m_chart->releaseAxis(m_fixedRangeAxis);
    m_chart->releaseAxis(m_temperatureAxis);
    m_chart->releaseAxis(m_yearAxis);
    m_chart->releaseAxis(m_monthAxis);
    m_chart->releaseAxis(m_genericRowAxis);
    m_chart->releaseAxis(m_genericColumnAxis);
}

void ChartModifier::releaseProxies()
{
    // Releases all proxies - results in default proxy.
    // Proxies will get readded back when graph is switched as setDataProxy call is made.
    m_chart->releaseDataProxy(m_temperatureData);
    m_chart->releaseDataProxy(m_genericData);
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

void ChartModifier::addRow()
{
    QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
    for (float i = 0; i < m_columnCount; i++) {
        (*dataRow)[i].setValue(((i + 1) / (float)m_columnCount) * (float)(rand() % 100));
        //(*dataRow)[i].setValue(i + m_chart->dataProxy()->rowCount());
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = qMax(m_selectedBarPos.x(), 0);
    m_chart->activeDataProxy()->insertRow(row, dataRow);
}

void ChartModifier::addRows()
{
    QTime timer;
    timer.start();
    QBarDataArray dataArray;
    for (int i = 0; i < m_rowCount; i++) {
        QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
        for (int j = 0; j < m_columnCount; j++)
            (*dataRow)[j].setValue(qreal(j + i + m_chart->activeDataProxy()->rowCount()));
        dataArray.append(dataRow);
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = qMax(m_selectedBarPos.x(), 0);
    m_chart->activeDataProxy()->insertRows(row, dataArray);
    qDebug() << "Added" << m_rowCount << "rows, time:" << timer.elapsed();
}

void ChartModifier::changeItem()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBarPos.x();
    int column = m_selectedBarPos.y();
    if (row >= 0 && column >= 0) {
        QBarDataItem item(qreal(rand() % 100));
        m_chart->activeDataProxy()->setItem(row, column, item);
    }
}

void ChartModifier::changeRow()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBarPos.x();
    if (row >= 0) {
        QBarDataRow *newRow = new QBarDataRow(m_chart->activeDataProxy()->rowAt(row)->size());
        for (int i = 0; i < newRow->size(); i++)
            (*newRow)[i].setValue(qreal(rand() % 100));
        m_chart->activeDataProxy()->setRow(row, newRow);
    }
}

void ChartModifier::changeRows()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBarPos.x();
    if (row >= 0) {
        int startRow = qMax(row - 2, 0);
        QBarDataArray newArray;
        for (int i = startRow; i <= row; i++ ) {
            QBarDataRow *newRow = new QBarDataRow(m_chart->activeDataProxy()->rowAt(i)->size());
            for (int j = 0; j < newRow->size(); j++)
                (*newRow)[j].setValue(qreal(rand() % 100));
            newArray.append(newRow);
        }
        m_chart->activeDataProxy()->setRows(startRow, newArray);
    }
}

void ChartModifier::removeRow()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBarPos.x();
    if (row >= 0)
        m_chart->activeDataProxy()->removeRows(row, 1);
}

void ChartModifier::removeRows()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBarPos.x();
    if (row >= 0) {
        int startRow = qMax(row - 2, 0);
        m_chart->activeDataProxy()->removeRows(startRow, 3);
    }
}

void ChartModifier::changeStyle()
{
    static int model = 0;
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

void ChartModifier::changeTheme()
{
    static int theme = QDataVis::ThemeSystem;

    m_chart->setTheme((QDataVis::ColorTheme)theme);

    if (++theme > QDataVis::ThemeLight)
        theme = QDataVis::ThemeSystem;
}

void ChartModifier::changeTransparency()
{
    static int transparency = QDataVis::TransparencyNone;

    m_chart->setLabelTransparency((QDataVis::LabelTransparency)transparency);

    if (++transparency > QDataVis::TransparencyNoBackground)
        transparency = QDataVis::TransparencyFromTheme;
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
    int quality = 0;
    switch (sq) {
    case QDataVis::ShadowLow:
        quality = 1;
        break;
    case QDataVis::ShadowMedium:
        quality = 2;
        break;
    case QDataVis::ShadowHigh:
        quality = 3;
        break;
    }

    // Updates the UI component to show correct shadow quality
    emit shadowQualityChanged(quality);
}

void ChartModifier::handleSelectionChange(const QPoint &position)
{
    m_selectedBarPos = position;
    qDebug() << "Selected bar position:" << position;
}

void ChartModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowNone;
    switch (quality) {
    case 1:
        sq = QDataVis::ShadowLow;
        break;
    case 2:
        sq = QDataVis::ShadowMedium;
        break;
    case 3:
        sq = QDataVis::ShadowHigh;
        break;
    }
    m_chart->setShadowQuality(sq);
    emit shadowQualityChanged(quality);
}

void ChartModifier::setBackgroundEnabled(int enabled)
{
    m_chart->setBackgroundVisible((bool)enabled);
}

void ChartModifier::setGridEnabled(int enabled)
{
    m_chart->setGridVisible((bool)enabled);
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

void ChartModifier::setSpecsRatio(int barwidth)
{
    m_barThicknessRatio = (float)barwidth / 30.0f;
    m_chart->setBarSpecs(m_barThicknessRatio, QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSpacingSpecsX(int spacing)
{
    m_barSpacingX = (float)spacing / 100.0f;
    m_chart->setBarSpecs(m_barThicknessRatio, QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSpacingSpecsZ(int spacing)
{
    m_barSpacingZ = (float)spacing / 100.0f;
    m_chart->setBarSpecs(m_barThicknessRatio, QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSampleCountX(int samples)
{
    m_columnCount = samples;
    m_chart->setDataWindow(m_rowCount, m_columnCount);
}

void ChartModifier::setSampleCountZ(int samples)
{
    m_rowCount = samples;
    m_chart->setDataWindow(m_rowCount, m_columnCount);
}

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
      m_columnCount(21),
      m_rowCount(21),
      m_xRotation(0.0),
      m_yRotation(0.0),
      m_static(true),
      m_barSpacingX(0.1),
      m_barSpacingZ(0.1),
      m_fontSize(20),
      m_segments(4),
      m_subSegments(3),
      m_minval(-20.0), // TODO Barchart Y-axis currently only properly supports zero-centered ranges
      m_maxval(20.0),
      m_selectedBarPos(-1, -1),
      m_autoAdjustingAxis(new Q3DValueAxis),
      m_fixedRangeAxis(new Q3DValueAxis),
      m_temperatureAxis(new Q3DValueAxis),
      m_yearAxis(new Q3DCategoryAxis),
      m_monthAxis(new Q3DCategoryAxis),
      m_genericRowAxis(new Q3DCategoryAxis),
      m_genericColumnAxis(new Q3DCategoryAxis),
      m_temperatureData(new QBarDataProxy),
      m_genericData(new QBarDataProxy),
      m_currentAxis(m_fixedRangeAxis)
{
    // Generate generic labels
    QStringList genericColumnLabels;
    for (int i = 0; i < 400; i++) {
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
    m_genericRowAxis->setRange(0, m_rowCount - 1);

    m_genericColumnAxis->setTitle("Generic Column");
    m_genericColumnAxis->setRange(0, m_columnCount - 1);

    m_temperatureAxis->setTitle("Average temperature");
    m_temperatureAxis->setSegmentCount(m_segments);
    m_temperatureAxis->setSubSegmentCount(m_subSegments);
    m_temperatureAxis->setRange(m_minval, m_maxval);
    m_temperatureAxis->setLabelFormat(QString(QStringLiteral("%d ") + celsiusString));

    m_yearAxis->setTitle("Year");

    m_monthAxis->setTitle("Month");

    m_chart->addAxis(m_autoAdjustingAxis);
    m_chart->addAxis(m_fixedRangeAxis);
    m_chart->addAxis(m_temperatureAxis);
    m_chart->addAxis(m_yearAxis);
    m_chart->addAxis(m_monthAxis);
    m_chart->addAxis(m_genericRowAxis);
    m_chart->addAxis(m_genericColumnAxis);

    m_chart->setShadowQuality(QDataVis::ShadowSoftMedium);

    m_temperatureData->setItemLabelFormat(QStringLiteral("@valueTitle for @colLabel @rowLabel: @valueLabel"));
    m_genericData->setItemLabelFormat(QStringLiteral("@valueTitle for (@rowIdx, @colIdx): @valueLabel"));
    m_genericData->setColumnLabels(genericColumnLabels);

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

        m_chart->setSelectionMode(QDataVis::ModeItem);
    } else {
        m_chart->setActiveDataProxy(m_genericData);

        m_chart->setTitle(QStringLiteral("Generic data"));

        m_chart->setValueAxis(m_currentAxis);
        m_chart->setRowAxis(m_genericRowAxis);
        m_chart->setColumnAxis(m_genericColumnAxis);

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

void ChartModifier::createMassiveArray()
{
    const int arrayDimension = 1000;
    QTime timer;
    timer.start();

    QStringList genericColumnLabels;
    for (int i = 0; i < arrayDimension; i++) {
        if (i % 5)
            genericColumnLabels << QString();
        else
            genericColumnLabels << QStringLiteral("Column %1").arg(i);
    }

    QStringList genericRowLabels;
    for (int i = 0; i < arrayDimension; i++) {
        if (i % 5)
            genericRowLabels << QString();
        else
            genericRowLabels << QStringLiteral("Row %1").arg(i);
    }

    QBarDataArray *dataArray = new QBarDataArray;
    dataArray->reserve(arrayDimension);
    for (int i = 0; i < arrayDimension; i++) {
        QBarDataRow *dataRow = new QBarDataRow(arrayDimension);
        for (int j = 0; j < arrayDimension; j++)
            (*dataRow)[j].setValue((qreal(i % 300 + 1) / 300.0) * qreal(rand() % 100));
        dataArray->append(dataRow);
    }

    m_chart->activeDataProxy()->resetArray(dataArray, genericRowLabels, genericColumnLabels);

    qDebug() << "Created Massive Array (" << arrayDimension << "), time:" << timer.elapsed();
}

void ChartModifier::resetTemperatureData()
{

    // Set up data
    static const qreal temp[7][12] = {
        {-6.7, -11.7, -9.7, 3.3, 9.2, 14.0, 16.3, 17.8, 10.2, 2.1, -2.6, -0.3},    // 2006
        {-6.8, -13.3, 0.2, 1.5, 7.9, 13.4, 16.1, 15.5, 8.2, 5.4, -2.6, -0.8},      // 2007
        {-4.2, -4.0, -4.6, 1.9, 7.3, 12.5, 15.0, 12.8, 7.6, 5.1, -0.9, -1.3},      // 2008
        {-7.8, -8.8, -4.2, 0.7, 9.3, 13.2, 15.8, 15.5, 11.2, 0.6, 0.7, -8.4},      // 2009
        {-14.4, -12.1, -7.0, 2.3, 11.0, 12.6, 18.8, 13.8, 9.4, 3.9, -5.6, -13.0},  // 2010
        {-9.0, -15.2, -3.8, 2.6, 8.3, 15.9, 18.6, 14.9, 11.1, 5.3, 1.8, -0.2},     // 2011
        {-8.7, -11.3, -2.3, 0.4, 7.5, 12.2, 16.4, 14.1, 9.2, 3.1, 0.3, -12.1}      // 2012
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
    m_temperatureData->resetArray(dataSet, m_years, m_months);
}


static int addCounter = 0;
static int insertCounter = 0;
static int changeCounter = 0;

void ChartModifier::addRow()
{
    QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
    for (qreal i = 0; i < m_columnCount; i++)
        (*dataRow)[i].setValue(((i + 1) / (qreal)m_columnCount) * (qreal)(rand() % 100));

    // TODO Needs to be changed to account for data window offset once it is implemented.
    QString label = QStringLiteral("Add %1").arg(addCounter++);
    m_chart->activeDataProxy()->addRow(dataRow, label);
}

void ChartModifier::addRows()
{
    QBarDataArray dataArray;
    QStringList labels;
    for (int i = 0; i < m_rowCount; i++) {
        QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
        for (int j = 0; j < m_columnCount; j++)
            (*dataRow)[j].setValue(qreal(j + i + m_chart->activeDataProxy()->rowCount()));
        dataArray.append(dataRow);
        labels.append(QStringLiteral("Add %1").arg(addCounter++));
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    m_chart->activeDataProxy()->addRows(dataArray, labels);
}

void ChartModifier::insertRow()
{
    QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
    for (qreal i = 0; i < m_columnCount; i++)
        (*dataRow)[i].setValue(((i + 1) / (qreal)m_columnCount) * (qreal)(rand() % 100));

    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = qMax(m_selectedBarPos.x(), 0);
    QString label = QStringLiteral("Insert %1").arg(insertCounter++);
    m_chart->activeDataProxy()->insertRow(row, dataRow, label);
}

void ChartModifier::insertRows()
{
    QTime timer;
    timer.start();
    QBarDataArray dataArray;
    QStringList labels;
    for (int i = 0; i < m_rowCount; i++) {
        QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
        for (int j = 0; j < m_columnCount; j++)
            (*dataRow)[j].setValue(qreal(j + i + m_chart->activeDataProxy()->rowCount()));
        dataArray.append(dataRow);
        labels.append(QStringLiteral("Insert %1").arg(insertCounter++));
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = qMax(m_selectedBarPos.x(), 0);
    m_chart->activeDataProxy()->insertRows(row, dataArray, labels);
    qDebug() << "Inserted" << m_rowCount << "rows, time:" << timer.elapsed();
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
        QString label = QStringLiteral("Change %1").arg(changeCounter++);
        m_chart->activeDataProxy()->setRow(row, newRow, label);
    }
}

void ChartModifier::changeRows()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBarPos.x();
    if (row >= 0) {
        int startRow = qMax(row - 2, 0);
        QBarDataArray newArray;
        QStringList labels;
        for (int i = startRow; i <= row; i++ ) {
            QBarDataRow *newRow = new QBarDataRow(m_chart->activeDataProxy()->rowAt(i)->size());
            for (int j = 0; j < newRow->size(); j++)
                (*newRow)[j].setValue(qreal(rand() % 100));
            newArray.append(newRow);
            labels.append(QStringLiteral("Change %1").arg(changeCounter++));
        }
        m_chart->activeDataProxy()->setRows(startRow, newArray, labels);
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
    static int theme = QDataVis::ThemeQt;

    m_chart->setTheme((QDataVis::ColorTheme)theme);

    if (++theme > QDataVis::ThemeIsabelle)
        theme = QDataVis::ThemeQt;
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

void ChartModifier::handleSelectionChange(const QPoint &position)
{
    m_selectedBarPos = position;
    qDebug() << "Selected bar position:" << position;
}

void ChartModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowQuality(quality);
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
    m_chart->setBarThickness((qreal)barwidth / 30.0);
}

void ChartModifier::setSpacingSpecsX(int spacing)
{
    m_barSpacingX = (qreal)spacing / 100.0;
    m_chart->setBarSpacing(QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSpacingSpecsZ(int spacing)
{
    m_barSpacingZ = (qreal)spacing / 100.0;
    m_chart->setBarSpacing(QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSampleCountX(int samples)
{
    m_columnCount = samples;
    m_genericColumnAxis->setRange(m_genericRowAxis->min(), m_genericRowAxis->min() + samples - 1);
}

void ChartModifier::setSampleCountZ(int samples)
{
    m_rowCount = samples;
    m_genericRowAxis->setRange(m_genericColumnAxis->min(), m_genericColumnAxis->min() + samples - 1);
}

void ChartModifier::setMinX(int min)
{
    m_genericRowAxis->setRange(min, min + m_rowCount - 1);
}

void ChartModifier::setMinZ(int min)
{
    m_genericColumnAxis->setRange(min, min + m_rowCount - 1);
}

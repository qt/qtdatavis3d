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
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QTime>

QT_DATAVISUALIZATION_USE_NAMESPACE

const QString celsiusString = QString(QChar(0xB0)) + "C";

GraphModifier::GraphModifier(Q3DBars *barchart)
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
      m_minval(-16.0),
      m_maxval(20.0),
      m_selectedBar(-1, -1),
      m_autoAdjustingAxis(new Q3DValueAxis),
      m_fixedRangeAxis(new Q3DValueAxis),
      m_temperatureAxis(new Q3DValueAxis),
      m_yearAxis(new Q3DCategoryAxis),
      m_monthAxis(new Q3DCategoryAxis),
      m_genericRowAxis(new Q3DCategoryAxis),
      m_genericColumnAxis(new Q3DCategoryAxis),
      m_temperatureData(new QBar3DSeries),
      m_temperatureData2(new QBar3DSeries),
      m_genericData(new QBar3DSeries),
      m_currentAxis(m_fixedRangeAxis),
      m_negativeValuesOn(false),
      m_useNullInputHandler(false),
      m_defaultInputHandler(0)
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

    m_chart->setTheme(QDataVis::ThemeStoneMoss);
    m_chart->setShadowQuality(QDataVis::ShadowQualitySoftMedium);

    m_temperatureData->setItemLabelFormat(QStringLiteral("@valueTitle for @colLabel @rowLabel: @valueLabel"));
    m_temperatureData2->setItemLabelFormat(QStringLiteral("** @valueTitle for @colLabel @rowLabel: @valueLabel **"));
    m_genericData->setItemLabelFormat(QStringLiteral("@valueTitle for (@rowIdx, @colIdx): @valueLabel"));
    m_genericData->dataProxy()->setColumnLabels(genericColumnLabels);

    m_chart->setFont(QFont("Times Roman", 20));

    // Release and store the default input handler.
    m_defaultInputHandler = m_chart->activeInputHandler();
    m_chart->releaseInputHandler(m_defaultInputHandler);
    m_chart->setActiveInputHandler(m_defaultInputHandler);

    resetTemperatureData();
}

GraphModifier::~GraphModifier()
{
    delete m_chart;
    delete m_defaultInputHandler;
}

void GraphModifier::start()
{
    restart(false);
}

void GraphModifier::restart(bool dynamicData)
{
    m_static = !dynamicData;

    if (m_static) {
        m_chart->addSeries(m_temperatureData);
        m_chart->addSeries(m_temperatureData2);
        m_chart->removeSeries(m_genericData);

        m_chart->setTitle(QStringLiteral("Average temperatures in Oulu, Finland (2006-2012)"));

        m_chart->setValueAxis(m_temperatureAxis);
        m_chart->setRowAxis(m_yearAxis);
        m_chart->setColumnAxis(m_monthAxis);

        m_chart->setSelectionMode(QDataVis::SelectionRow | QDataVis::SelectionSlice);
    } else {
        m_chart->removeSeries(m_temperatureData);
        m_chart->removeSeries(m_temperatureData2);
        m_chart->addSeries(m_genericData);

        m_chart->setTitle(QStringLiteral("Generic data"));

        m_minval = m_fixedRangeAxis->min();
        m_maxval = m_fixedRangeAxis->max();
        m_chart->setValueAxis(m_currentAxis);
        m_chart->setRowAxis(m_genericRowAxis);
        m_chart->setColumnAxis(m_genericColumnAxis);

        m_chart->setSelectionMode(QDataVis::SelectionRow | QDataVis::SelectionSlice);
    }
}

void GraphModifier::selectBar()
{
    QPoint targetBar(5, 5);
    QPoint noSelection(-1, -1);
    if (m_selectedBar != targetBar)
        m_chart->setSelectedBar(targetBar);
    else
        m_chart->setSelectedBar(noSelection);
}

void GraphModifier::swapAxis()
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

void GraphModifier::releaseAxes()
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

void GraphModifier::releaseProxies()
{
    // Releases all series
    // Correct series will get readded back when graph is switched
    m_chart->removeSeries(m_temperatureData);
    m_chart->removeSeries(m_temperatureData2);
    m_chart->removeSeries(m_genericData);
}

void GraphModifier::flipViews()
{
    m_chart->scene()->setSecondarySubviewOnTop(!m_chart->scene()->isSecondarySubviewOnTop());
}

void GraphModifier::createMassiveArray()
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
        for (int j = 0; j < arrayDimension; j++) {
            if (!m_negativeValuesOn)
                (*dataRow)[j].setValue((qreal(i % 300 + 1) / 300.0) * qreal(rand() % int(m_maxval)));
            else
                (*dataRow)[j].setValue((qreal(i % 300 + 1) / 300.0) * qreal(rand() % int(m_maxval))
                                       + m_minval);
        }
        dataArray->append(dataRow);
    }

    m_chart->seriesList().at(0)->dataProxy()->resetArray(dataArray, genericRowLabels, genericColumnLabels);

    qDebug() << "Created Massive Array (" << arrayDimension << "), time:" << timer.elapsed();
}

void GraphModifier::resetTemperatureData()
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

    QBarDataArray *dataSet2 = new QBarDataArray;

    dataSet2->reserve(m_years.size());
    for (int year = m_years.size() - 1; year >= 0; year--) {
        dataRow = new QBarDataRow(m_months.size());
        // Create data items
        for (int month = m_months.size() - 1; month >= 0 ; month--) {
            // Add data to rows
            (*dataRow)[month].setValue(temp[year][month]);
        }
        // Add row to set
        dataSet2->append(dataRow);
    }

    // Add data to chart (chart assumes ownership)
    m_temperatureData->dataProxy()->resetArray(dataSet, m_years, m_months);
    m_temperatureData2->dataProxy()->resetArray(dataSet2, m_years, m_months);
}


static int addCounter = 0;
static int insertCounter = 0;
static int changeCounter = 0;

void GraphModifier::addRow()
{
    QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
    for (qreal i = 0; i < m_columnCount; i++) {
        if (!m_negativeValuesOn)
            (*dataRow)[i].setValue(((i + 1) / (qreal)m_columnCount) * (qreal)(rand() % int(m_maxval)));
        else
            (*dataRow)[i].setValue(((i + 1) / (qreal)m_columnCount) * (qreal)(rand() % int(m_maxval))
                                   - (qreal)(rand() % int(m_minval)));
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    QString label = QStringLiteral("Add %1").arg(addCounter++);
    m_chart->seriesList().at(0)->dataProxy()->addRow(dataRow, label);
}

void GraphModifier::addRows()
{
    QBarDataArray dataArray;
    QStringList labels;
    for (int i = 0; i < m_rowCount; i++) {
        QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
        for (int j = 0; j < m_columnCount; j++)
            (*dataRow)[j].setValue(qreal(j + i + m_chart->seriesList().at(0)->dataProxy()->rowCount()) + m_minval);
        dataArray.append(dataRow);
        labels.append(QStringLiteral("Add %1").arg(addCounter++));
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    m_chart->seriesList().at(0)->dataProxy()->addRows(dataArray, labels);
}

void GraphModifier::insertRow()
{
    QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
    for (qreal i = 0; i < m_columnCount; i++)
        (*dataRow)[i].setValue(((i + 1) / (qreal)m_columnCount) * (qreal)(rand() % int(m_maxval))
                               + m_minval);

    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = qMax(m_selectedBar.x(), 0);
    QString label = QStringLiteral("Insert %1").arg(insertCounter++);
    m_chart->seriesList().at(0)->dataProxy()->insertRow(row, dataRow, label);
}

void GraphModifier::insertRows()
{
    QTime timer;
    timer.start();
    QBarDataArray dataArray;
    QStringList labels;
    for (int i = 0; i < m_rowCount; i++) {
        QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
        for (int j = 0; j < m_columnCount; j++)
            (*dataRow)[j].setValue(qreal(j + i + m_chart->seriesList().at(0)->dataProxy()->rowCount()) + m_minval);
        dataArray.append(dataRow);
        labels.append(QStringLiteral("Insert %1").arg(insertCounter++));
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = qMax(m_selectedBar.x(), 0);
    m_chart->seriesList().at(0)->dataProxy()->insertRows(row, dataArray, labels);
    qDebug() << "Inserted" << m_rowCount << "rows, time:" << timer.elapsed();
}

void GraphModifier::changeItem()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    int column = m_selectedBar.y();
    if (row >= 0 && column >= 0) {
        QBarDataItem item(qreal(rand() % 100));
        m_chart->seriesList().at(0)->dataProxy()->setItem(row, column, item);
    }
}

void GraphModifier::changeRow()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    if (row >= 0) {
        QBarDataRow *newRow = new QBarDataRow(m_chart->seriesList().at(0)->dataProxy()->rowAt(row)->size());
        for (int i = 0; i < newRow->size(); i++)
            (*newRow)[i].setValue(qreal(rand() % int(m_maxval)) + m_minval);
        QString label = QStringLiteral("Change %1").arg(changeCounter++);
        m_chart->seriesList().at(0)->dataProxy()->setRow(row, newRow, label);
    }
}

void GraphModifier::changeRows()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    if (row >= 0) {
        int startRow = qMax(row - 2, 0);
        QBarDataArray newArray;
        QStringList labels;
        for (int i = startRow; i <= row; i++ ) {
            QBarDataRow *newRow = new QBarDataRow(m_chart->seriesList().at(0)->dataProxy()->rowAt(i)->size());
            for (int j = 0; j < newRow->size(); j++)
                (*newRow)[j].setValue(qreal(rand() % int(m_maxval)) + m_minval);
            newArray.append(newRow);
            labels.append(QStringLiteral("Change %1").arg(changeCounter++));
        }
        m_chart->seriesList().at(0)->dataProxy()->setRows(startRow, newArray, labels);
    }
}

void GraphModifier::removeRow()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    if (row >= 0)
        m_chart->seriesList().at(0)->dataProxy()->removeRows(row, 1);
}

void GraphModifier::removeRows()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    if (row >= 0) {
        int startRow = qMax(row - 2, 0);
        m_chart->seriesList().at(0)->dataProxy()->removeRows(startRow, 3);
    }
}

void GraphModifier::changeStyle()
{
    static int model = 0;
    switch (model) {
    case 0:
        m_chart->setBarType(QDataVis::MeshStyleCylinders, false);
        break;
    case 1:
        m_chart->setBarType(QDataVis::MeshStyleCylinders, true);
        break;
    case 2:
        m_chart->setBarType(QDataVis::MeshStyleCones, false);
        break;
    case 3:
        m_chart->setBarType(QDataVis::MeshStyleCones, true);
        break;
    case 4:
        m_chart->setBarType(QDataVis::MeshStyleBars, false);
        break;
    case 5:
        m_chart->setBarType(QDataVis::MeshStyleBars, true);
        break;
    case 6:
        m_chart->setBarType(QDataVis::MeshStylePyramids, false);
        break;
    case 7:
        m_chart->setBarType(QDataVis::MeshStylePyramids, true);
        break;
    case 8:
        m_chart->setBarType(QDataVis::MeshStyleBevelBars, false);
        break;
    case 9:
        m_chart->setBarType(QDataVis::MeshStyleBevelBars, true);
        break;
    }
    model++;
    if (model > 9)
        model = 0;
}

void GraphModifier::changePresetCamera()
{
    static int preset = QDataVis::CameraPresetFrontLow;

    m_chart->scene()->activeCamera()->setCameraPreset((QDataVis::CameraPreset)preset);

    if (++preset > QDataVis::CameraPresetDirectlyBelow)
        preset = QDataVis::CameraPresetFrontLow;
}

void GraphModifier::changeTheme()
{
    static int theme = QDataVis::ThemeQt;

    m_chart->setTheme((QDataVis::Theme)theme);

    if (++theme > QDataVis::ThemeIsabelle)
        theme = QDataVis::ThemeQt;
}

void GraphModifier::changeLabelStyle()
{
    static int style = QDataVis::LabelStyleOpaque;

    m_chart->setLabelStyle((QDataVis::LabelStyle)style);

    if (++style > QDataVis::LabelStyleTransparent)
        style = QDataVis::LabelStyleOpaque;
}

void GraphModifier::changeSelectionMode()
{
    static int selectionMode = m_chart->selectionMode();

    if (++selectionMode > (QDataVis::SelectionItemAndColumn | QDataVis::SelectionSlice))
        selectionMode = QDataVis::SelectionNone;

    m_chart->setSelectionMode((QDataVis::SelectionFlag)selectionMode);
}

void GraphModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSize(m_fontSize);
    m_chart->setFont(newFont);
}

void GraphModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_chart->font();
    font.setPointSize(m_fontSize);
    m_chart->setFont(font);
}

void GraphModifier::shadowQualityUpdatedByVisual(QDataVis::ShadowQuality sq)
{
    int quality = int(sq);
    // Updates the UI component to show correct shadow quality
    emit shadowQualityChanged(quality);
}

void GraphModifier::handleSelectionChange(const QPoint &position)
{
    m_selectedBar = position;
    qDebug() << "Selected bar position:" << position;
}

void GraphModifier::setUseNullInputHandler(bool useNull)
{
    qDebug() << "setUseNullInputHandler" << useNull;
    if (m_useNullInputHandler == useNull)
        return;

    m_useNullInputHandler = useNull;

    if (useNull)
        m_chart->setActiveInputHandler(0);
    else
        m_chart->setActiveInputHandler(m_defaultInputHandler);
}


void GraphModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowQuality(quality);
    m_chart->setShadowQuality(sq);
    emit shadowQualityChanged(quality);
}

void GraphModifier::setBackgroundEnabled(int enabled)
{
    m_chart->setBackgroundVisible((bool)enabled);
}

void GraphModifier::setGridEnabled(int enabled)
{
    m_chart->setGridVisible((bool)enabled);
}

void GraphModifier::rotateX(int rotation)
{
    m_xRotation = rotation;
    m_chart->scene()->activeCamera()->setCameraPosition(m_xRotation, m_yRotation);
}

void GraphModifier::rotateY(int rotation)
{
    m_yRotation = rotation;
    m_chart->scene()->activeCamera()->setCameraPosition(m_xRotation, m_yRotation);
}

void GraphModifier::setSpecsRatio(int barwidth)
{
    m_chart->setBarThickness((qreal)barwidth / 30.0);
}

void GraphModifier::setSpacingSpecsX(int spacing)
{
    m_barSpacingX = (qreal)spacing / 100.0;
    m_chart->setBarSpacing(QSizeF(m_barSpacingX, m_barSpacingZ));
}

void GraphModifier::setSpacingSpecsZ(int spacing)
{
    m_barSpacingZ = (qreal)spacing / 100.0;
    m_chart->setBarSpacing(QSizeF(m_barSpacingX, m_barSpacingZ));
}

void GraphModifier::setSampleCountX(int samples)
{
    m_columnCount = samples;
    m_genericColumnAxis->setRange(m_genericRowAxis->min(), m_genericRowAxis->min() + samples - 1);
}

void GraphModifier::setSampleCountZ(int samples)
{
    m_rowCount = samples;
    m_genericRowAxis->setRange(m_genericColumnAxis->min(), m_genericColumnAxis->min() + samples - 1);
}

void GraphModifier::setMinX(int min)
{
    m_genericRowAxis->setRange(min, min + m_rowCount - 1);
}

void GraphModifier::setMinZ(int min)
{
    m_genericColumnAxis->setRange(min, min + m_rowCount - 1);
}

void GraphModifier::setMinY(int min)
{
    m_fixedRangeAxis->setMin(min);
    m_negativeValuesOn = (min < 0) ? true : false;
    m_minval = min;
}

void GraphModifier::setMaxY(int max)
{
    m_fixedRangeAxis->setMax(max);
    m_maxval = max;
}


void GraphModifier::setGradient()
{
    QLinearGradient barGradient(0, 0, 1, 100);
    barGradient.setColorAt(1.0, Qt::lightGray);
    barGradient.setColorAt(0.75001, Qt::lightGray);
    barGradient.setColorAt(0.75, Qt::blue);
    barGradient.setColorAt(0.50001, Qt::blue);
    barGradient.setColorAt(0.50, Qt::red);
    barGradient.setColorAt(0.25001, Qt::red);
    barGradient.setColorAt(0.25, Qt::yellow);
    barGradient.setColorAt(0.0, Qt::yellow);

    QLinearGradient singleHighlightGradient(0, 0, 1, 100);
    singleHighlightGradient.setColorAt(1.0, Qt::white);
    singleHighlightGradient.setColorAt(0.75, Qt::lightGray);
    singleHighlightGradient.setColorAt(0.50, Qt::gray);
    singleHighlightGradient.setColorAt(0.25, Qt::darkGray);
    singleHighlightGradient.setColorAt(0.0, Qt::black);

    QLinearGradient multiHighlightGradient(0, 0, 1, 100);
    multiHighlightGradient.setColorAt(1.0, Qt::black);
    multiHighlightGradient.setColorAt(0.75, Qt::darkBlue);
    multiHighlightGradient.setColorAt(0.50, Qt::darkRed);
    multiHighlightGradient.setColorAt(0.25, Qt::darkYellow);
    multiHighlightGradient.setColorAt(0.0, Qt::darkGray);

    m_chart->setBarColor(Qt::green);
    m_chart->setSingleHighlightColor(Qt::white);
    m_chart->setMultiHighlightColor(Qt::cyan);

    m_chart->setBarGradient(barGradient);
    m_chart->setSingleHighlightGradient(singleHighlightGradient);
    m_chart->setMultiHighlightGradient(multiHighlightGradient);

    QDataVis::ColorStyle oldStyle = m_chart->colorStyle();
    if (oldStyle == QDataVis::ColorStyleUniform)
        m_chart->setColorStyle(QDataVis::ColorStyleObjectGradient);
    else if (oldStyle == QDataVis::ColorStyleObjectGradient)
        m_chart->setColorStyle(QDataVis::ColorStyleRangeGradient);
    if (oldStyle == QDataVis::ColorStyleRangeGradient)
        m_chart->setColorStyle(QDataVis::ColorStyleUniform);
}

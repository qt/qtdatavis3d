/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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
#include "custominputhandler.h"
#include <QtDataVisualization/qcategory3daxis.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/qlogvalue3daxisformatter.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtDataVisualization/q3dinputhandler.h>
#include <QtCore/QTime>
#include <QtCore/qmath.h>

using namespace QtDataVisualization;

const QString celsiusString = QString(QChar(0xB0)) + "C";

GraphModifier::GraphModifier(Q3DBars *barchart, QColorDialog *colorDialog)
    : m_graph(barchart),
      m_colorDialog(colorDialog),
      m_columnCount(21),
      m_rowCount(21),
      m_xRotation(0.0f),
      m_yRotation(0.0f),
      m_static(true),
      m_barSpacingX(0.1f),
      m_barSpacingZ(0.1f),
      m_fontSize(20),
      m_segments(10),
      m_subSegments(3),
      m_minval(-16.0f),
      m_maxval(20.0f),
      m_selectedBar(-1, -1),
      m_selectedSeries(0),
      m_autoAdjustingAxis(new QValue3DAxis),
      m_fixedRangeAxis(new QValue3DAxis),
      m_temperatureAxis(new QValue3DAxis),
      m_yearAxis(new QCategory3DAxis),
      m_monthAxis(new QCategory3DAxis),
      m_genericRowAxis(new QCategory3DAxis),
      m_genericColumnAxis(new QCategory3DAxis),
      m_temperatureData(new QBar3DSeries),
      m_temperatureData2(new QBar3DSeries),
      m_genericData(new QBar3DSeries),
      m_dummyData(new QBar3DSeries),
      m_dummyData2(new QBar3DSeries),
      m_dummyData3(new QBar3DSeries),
      m_dummyData4(new QBar3DSeries),
      m_dummyData5(new QBar3DSeries),
      m_currentAxis(m_fixedRangeAxis),
      m_negativeValuesOn(false),
      m_useNullInputHandler(false),
      m_defaultInputHandler(0),
      m_ownTheme(0),
      m_builtinTheme(new Q3DTheme(Q3DTheme::ThemeStoneMoss)),
      m_customInputHandler(new CustomInputHandler),
      m_extraSeries(0)
{
    m_temperatureData->setObjectName("m_temperatureData");
    m_temperatureData2->setObjectName("m_temperatureData2");
    m_genericData->setObjectName("m_genericData");
    m_dummyData->setObjectName("m_dummyData");
    m_dummyData2->setObjectName("m_dummyData2");
    m_dummyData3->setObjectName("m_dummyData3");
    m_dummyData4->setObjectName("m_dummyData4");
    m_dummyData5->setObjectName("m_dummyData5");

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

    m_graph->addAxis(m_autoAdjustingAxis);
    m_graph->addAxis(m_fixedRangeAxis);
    m_graph->addAxis(m_temperatureAxis);
    m_graph->addAxis(m_yearAxis);
    m_graph->addAxis(m_monthAxis);
    m_graph->addAxis(m_genericRowAxis);
    m_graph->addAxis(m_genericColumnAxis);

    m_graph->setActiveTheme(m_builtinTheme);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftMedium);

    m_temperatureData->setName("Oulu");
    m_temperatureData2->setName("Helsinki");
    m_genericData->setName("Generic series");
    m_dummyData->setName("Dummy 1");
    m_dummyData2->setName("Dummy 2");
    m_dummyData3->setName("Dummy 3");
    m_dummyData4->setName("Dummy 4");
    m_dummyData5->setName("Dummy 5");

    m_temperatureData->setItemLabelFormat(QStringLiteral("@seriesName: @valueTitle for @colLabel @rowLabel: @valueLabel ~ %.4f"));
    m_temperatureData2->setItemLabelFormat(QStringLiteral("@seriesName: @valueTitle for @colLabel @rowLabel: @valueLabel"));
    m_genericData->setItemLabelFormat(QStringLiteral("@seriesName: @valueTitle for (@rowIdx, @colIdx): @valueLabel"));

    m_dummyData->setItemLabelFormat(QStringLiteral("@seriesName: @valueLabel"));
    m_dummyData2->setItemLabelFormat(QStringLiteral("@seriesName: @valueLabel"));
    m_dummyData3->setItemLabelFormat(QStringLiteral("@seriesName: @valueLabel"));
    m_dummyData4->setItemLabelFormat(QStringLiteral("@seriesName: @valueLabel"));
    m_dummyData5->setItemLabelFormat(QStringLiteral("@seriesName: @valueLabel"));

    m_genericData->dataProxy()->setColumnLabels(genericColumnLabels);

    m_temperatureData->setBaseColor(Qt::red);
    m_temperatureData->setSingleHighlightColor(Qt::cyan);
    m_temperatureData->setMultiHighlightColor(Qt::magenta);
    m_temperatureData2->setBaseColor(Qt::yellow);
    m_genericData->setBaseColor(Qt::blue);

    QLinearGradient barGradient1(0, 0, 1, 100);
    barGradient1.setColorAt(1.0, Qt::red);
    barGradient1.setColorAt(0.75001, Qt::red);
    barGradient1.setColorAt(0.75, Qt::magenta);
    barGradient1.setColorAt(0.50001, Qt::magenta);
    barGradient1.setColorAt(0.50, Qt::blue);
    barGradient1.setColorAt(0.25001, Qt::blue);
    barGradient1.setColorAt(0.25, Qt::black);
    barGradient1.setColorAt(0.0, Qt::black);

    QLinearGradient barGradient2(0, 0, 1, 100);
    barGradient2.setColorAt(1.0, Qt::red);
    barGradient2.setColorAt(0.75, Qt::magenta);
    barGradient2.setColorAt(0.50, Qt::blue);
    barGradient2.setColorAt(0.25, Qt::black);
    barGradient2.setColorAt(0.0, Qt::black);

    QLinearGradient singleHighlightGradient(0, 0, 1, 100);
    singleHighlightGradient.setColorAt(1.0, Qt::white);
    singleHighlightGradient.setColorAt(0.75, Qt::lightGray);
    singleHighlightGradient.setColorAt(0.50, Qt::gray);
    singleHighlightGradient.setColorAt(0.25, Qt::darkGray);
    singleHighlightGradient.setColorAt(0.0, Qt::black);

    QLinearGradient multiHighlightGradient(0, 0, 1, 100);
    multiHighlightGradient.setColorAt(1.0, Qt::lightGray);
    multiHighlightGradient.setColorAt(0.75, Qt::gray);
    multiHighlightGradient.setColorAt(0.50, Qt::darkGray);
    multiHighlightGradient.setColorAt(0.25, Qt::black);
    multiHighlightGradient.setColorAt(0.0, Qt::black);

    m_temperatureData->setBaseGradient(barGradient1);
    m_temperatureData2->setBaseGradient(barGradient2);
    m_temperatureData->setSingleHighlightGradient(singleHighlightGradient);
    m_temperatureData->setMultiHighlightGradient(multiHighlightGradient);

    m_graph->activeTheme()->setFont(QFont("Times Roman", 20));

    // Release and store the default input handler.
    m_defaultInputHandler = m_graph->activeInputHandler();
    m_graph->releaseInputHandler(m_defaultInputHandler);
    m_graph->setActiveInputHandler(m_defaultInputHandler);

    QObject::connect(m_graph, &Q3DBars::shadowQualityChanged, this,
                     &GraphModifier::shadowQualityUpdatedByVisual);
    QObject::connect(m_temperatureData, &QBar3DSeries::selectedBarChanged, this,
                     &GraphModifier::handleSelectionChange);
    QObject::connect(m_temperatureData2, &QBar3DSeries::selectedBarChanged, this,
                     &GraphModifier::handleSelectionChange);
    QObject::connect(m_genericData, &QBar3DSeries::selectedBarChanged, this,
                     &GraphModifier::handleSelectionChange);

    QObject::connect(m_graph, &Q3DBars::rowAxisChanged, this,
                     &GraphModifier::handleRowAxisChanged);
    QObject::connect(m_graph, &Q3DBars::columnAxisChanged, this,
                     &GraphModifier::handleColumnAxisChanged);
    QObject::connect(m_graph, &Q3DBars::valueAxisChanged, this,
                     &GraphModifier::handleValueAxisChanged);
    QObject::connect(m_graph, &Q3DBars::primarySeriesChanged, this,
                     &GraphModifier::handlePrimarySeriesChanged);
    QObject::connect(m_temperatureAxis, &QAbstract3DAxis::labelsChanged, this,
                     &GraphModifier::handleValueAxisLabelsChanged);

    QObject::connect(&m_insertRemoveTimer, &QTimer::timeout, this,
                     &GraphModifier::insertRemoveTimerTimeout);

    m_graph->addSeries(m_temperatureData);
    m_graph->addSeries(m_temperatureData2);

    QObject::connect(&m_selectionTimer, &QTimer::timeout, this,
                     &GraphModifier::triggerSelection);
    QObject::connect(&m_rotationTimer, &QTimer::timeout, this,
                     &GraphModifier::triggerRotation);

    QObject::connect(m_graph, &QAbstract3DGraph::currentFpsChanged, this,
                     &GraphModifier::handleFpsChange);


    resetTemperatureData();
}

GraphModifier::~GraphModifier()
{
    delete m_graph;
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
        m_graph->removeSeries(m_genericData);

        m_graph->setTitle(QStringLiteral("Average temperatures in Oulu, Finland (2006-2012)"));

        m_graph->setValueAxis(m_temperatureAxis);
        m_graph->setRowAxis(m_yearAxis);
        m_graph->setColumnAxis(m_monthAxis);

    } else {
        m_graph->addSeries(m_genericData);

        m_graph->setTitle(QStringLiteral("Generic data"));

        m_minval = m_fixedRangeAxis->min();
        m_maxval = m_fixedRangeAxis->max();
        m_graph->setValueAxis(m_currentAxis);
        m_graph->setRowAxis(m_genericRowAxis);
        m_graph->setColumnAxis(m_genericColumnAxis);
    }
}

void GraphModifier::selectBar()
{
    QPoint targetBar(5, 5);
    QPoint noSelection(-1, -1);
    if (m_selectedBar != targetBar)
        m_graph->seriesList().at(0)->setSelectedBar(targetBar);
    else
        m_graph->seriesList().at(0)->setSelectedBar(noSelection);
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

    m_graph->setValueAxis(m_currentAxis);
}

void GraphModifier::releaseAxes()
{
    // Releases all axes - results in default axes for all dimensions.
    // Axes reset when the graph is switched as set*Axis calls are made, which
    // implicitly add axes.
    m_graph->releaseAxis(m_autoAdjustingAxis);
    m_graph->releaseAxis(m_fixedRangeAxis);
    m_graph->releaseAxis(m_temperatureAxis);
    m_graph->releaseAxis(m_yearAxis);
    m_graph->releaseAxis(m_monthAxis);
    m_graph->releaseAxis(m_genericRowAxis);
    m_graph->releaseAxis(m_genericColumnAxis);
}

void GraphModifier::releaseProxies()
{
    // Releases all series/add all series toggle
    if (m_graph->seriesList().size() > 0) {
        m_graph->removeSeries(m_temperatureData);
        m_graph->removeSeries(m_temperatureData2);
        m_graph->removeSeries(m_genericData);
        m_graph->removeSeries(m_dummyData);
        m_graph->removeSeries(m_dummyData2);
        m_graph->removeSeries(m_dummyData3);
        m_graph->removeSeries(m_dummyData4);
        m_graph->removeSeries(m_dummyData5);
    } else {
        m_graph->addSeries(m_temperatureData);
        m_graph->addSeries(m_temperatureData2);
        m_graph->addSeries(m_genericData);
    }
}

void GraphModifier::flipViews()
{
    m_graph->scene()->setSecondarySubviewOnTop(!m_graph->scene()->isSecondarySubviewOnTop());
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
                (*dataRow)[j].setValue((float(i % 300 + 1) / 300.0) * float(rand() % int(m_maxval)));
            else
                (*dataRow)[j].setValue((float(i % 300 + 1) / 300.0) * float(rand() % int(m_maxval))
                                       + m_minval);
        }
        dataArray->append(dataRow);
    }

    m_genericData->dataProxy()->resetArray(dataArray, genericRowLabels, genericColumnLabels);

    qDebug() << "Created Massive Array (" << arrayDimension << "), time:" << timer.elapsed();
}

void GraphModifier::resetTemperatureData()
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
    for (float i = 0; i < m_columnCount; i++) {
        if (!m_negativeValuesOn)
            (*dataRow)[i].setValue(((i + 1) / (float)m_columnCount) * (float)(rand() % int(m_maxval)));
        else
            (*dataRow)[i].setValue(((i + 1) / (float)m_columnCount) * (float)(rand() % int(m_maxval))
                                   - (float)(rand() % int(m_minval)));
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    QString label = QStringLiteral("Add %1").arg(addCounter++);
    m_genericData->dataProxy()->addRow(dataRow, label);
}

void GraphModifier::addRows()
{
    QBarDataArray dataArray;
    QStringList labels;
    for (int i = 0; i < m_rowCount; i++) {
        QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
        for (int j = 0; j < m_columnCount; j++)
            (*dataRow)[j].setValue(float(j + i + m_genericData->dataProxy()->rowCount()) + m_minval);
        dataArray.append(dataRow);
        labels.append(QStringLiteral("Add %1").arg(addCounter++));
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    m_genericData->dataProxy()->addRows(dataArray, labels);
}

void GraphModifier::insertRow()
{
    QBarDataRow *dataRow = new QBarDataRow(m_columnCount);
    for (float i = 0; i < m_columnCount; i++)
        (*dataRow)[i].setValue(((i + 1) / (float)m_columnCount) * (float)(rand() % int(m_maxval))
                               + m_minval);

    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = qMax(m_selectedBar.x(), 0);
    QString label = QStringLiteral("Insert %1").arg(insertCounter++);
    m_genericData->dataProxy()->insertRow(row, dataRow, label);
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
            (*dataRow)[j].setValue(float(j + i + m_genericData->dataProxy()->rowCount()) + m_minval);
        dataArray.append(dataRow);
        labels.append(QStringLiteral("Insert %1").arg(insertCounter++));
    }

    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = qMax(m_selectedBar.x(), 0);
    m_genericData->dataProxy()->insertRows(row, dataArray, labels);
    qDebug() << "Inserted" << m_rowCount << "rows, time:" << timer.elapsed();
}

void GraphModifier::changeItem()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    int column = m_selectedBar.y();
    if (row >= 0 && column >= 0) {
        QBarDataItem item(float(rand() % 100));
        m_genericData->dataProxy()->setItem(row, column, item);
    }
}

void GraphModifier::changeRow()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    if (row >= 0) {
        QBarDataRow *newRow = new QBarDataRow(m_genericData->dataProxy()->rowAt(row)->size());
        for (int i = 0; i < newRow->size(); i++)
            (*newRow)[i].setValue(float(rand() % int(m_maxval)) + m_minval);
        QString label = QStringLiteral("Change %1").arg(changeCounter++);
        m_genericData->dataProxy()->setRow(row, newRow, label);
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
            QBarDataRow *newRow = new QBarDataRow(m_genericData->dataProxy()->rowAt(i)->size());
            for (int j = 0; j < newRow->size(); j++)
                (*newRow)[j].setValue(float(rand() % int(m_maxval)) + m_minval);
            newArray.append(newRow);
            labels.append(QStringLiteral("Change %1").arg(changeCounter++));
        }
        m_genericData->dataProxy()->setRows(startRow, newArray, labels);
    }
}

void GraphModifier::removeRow()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    if (row >= 0)
        m_genericData->dataProxy()->removeRows(row, 1);
}

void GraphModifier::removeRows()
{
    // TODO Needs to be changed to account for data window offset once it is implemented.
    int row = m_selectedBar.x();
    if (row >= 0) {
        int startRow = qMax(row - 3, 0);
        m_genericData->dataProxy()->removeRows(startRow, 3);
    }
}

void GraphModifier::changeStyle()
{
    static int model = 0;
    switch (model) {
    case 0:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshCylinder);
        m_graph->seriesList().at(0)->setMeshSmooth(false);
        break;
    case 1:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshCylinder);
        m_graph->seriesList().at(0)->setMeshSmooth(true);
        break;
    case 2:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshCone);
        m_graph->seriesList().at(0)->setMeshSmooth(false);
        break;
    case 3:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshCone);
        m_graph->seriesList().at(0)->setMeshSmooth(true);
        break;
    case 4:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshBar);
        m_graph->seriesList().at(0)->setMeshSmooth(false);
        break;
    case 5:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshBar);
        m_graph->seriesList().at(0)->setMeshSmooth(true);
        break;
    case 6:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshPyramid);
        m_graph->seriesList().at(0)->setMeshSmooth(false);
        break;
    case 7:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshPyramid);
        m_graph->seriesList().at(0)->setMeshSmooth(true);
        break;
    case 8:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshBevelBar);
        m_graph->seriesList().at(0)->setMeshSmooth(false);
        break;
    case 9:
        m_graph->seriesList().at(0)->setMesh(QAbstract3DSeries::MeshBevelBar);
        m_graph->seriesList().at(0)->setMeshSmooth(true);
        break;
    }
    model++;
    if (model > 9)
        model = 0;
}

void GraphModifier::changePresetCamera()
{
    static int preset = Q3DCamera::CameraPresetFrontLow;

    m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);

    if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
        preset = Q3DCamera::CameraPresetFrontLow;
}

void GraphModifier::changeTheme()
{
    static int theme = Q3DTheme::ThemeQt;

    Q3DTheme *currentTheme = m_graph->activeTheme();
    m_builtinTheme->setType(Q3DTheme::Theme(theme));
    if (currentTheme == m_ownTheme)
        m_graph->setActiveTheme(m_builtinTheme);

    switch (theme) {
        case Q3DTheme::ThemeQt:
            qDebug() << __FUNCTION__ << "ThemeQt";
            break;
        case Q3DTheme::ThemePrimaryColors:
            qDebug() << __FUNCTION__ << "ThemePrimaryColors";
            break;
        case Q3DTheme::ThemeDigia:
            qDebug() << __FUNCTION__ << "ThemeDigia";
            break;
        case Q3DTheme::ThemeStoneMoss:
            qDebug() << __FUNCTION__ << "ThemeStoneMoss";
            break;
        case Q3DTheme::ThemeArmyBlue:
            qDebug() << __FUNCTION__ << "ThemeArmyBlue";
            break;
        case Q3DTheme::ThemeRetro:
            qDebug() << __FUNCTION__ << "ThemeRetro";
            break;
        case Q3DTheme::ThemeEbony:
            qDebug() << __FUNCTION__ << "ThemeEbony";
            break;
        case Q3DTheme::ThemeIsabelle:
            qDebug() << __FUNCTION__ << "ThemeIsabelle";
            break;
        default:
            qDebug() << __FUNCTION__ << "Unknown theme";
            break;
    }

    if (++theme > Q3DTheme::ThemeIsabelle)
        theme = Q3DTheme::ThemeQt;
}

void GraphModifier::changeLabelStyle()
{
    m_graph->activeTheme()->setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
}

void GraphModifier::changeSelectionMode()
{
    static int selectionMode = m_graph->selectionMode();

    if (++selectionMode > (QAbstract3DGraph::SelectionItemAndColumn | QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionMultiSeries))
        selectionMode = QAbstract3DGraph::SelectionNone;

    m_graph->setSelectionMode((QAbstract3DGraph::SelectionFlag)selectionMode);
}

void GraphModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSize(m_fontSize);
    m_graph->activeTheme()->setFont(newFont);
}

void GraphModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(m_fontSize);
    m_graph->activeTheme()->setFont(font);
}

void GraphModifier::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq)
{
    int quality = int(sq);
    // Updates the UI component to show correct shadow quality
    emit shadowQualityChanged(quality);
}

void GraphModifier::handleSelectionChange(const QPoint &position)
{
    m_selectedBar = position;
    int index = 0;
    foreach (QBar3DSeries *series, m_graph->seriesList()) {
        if (series == sender()) {
            if (series->selectedBar() != QBar3DSeries::invalidSelectionPosition())
                m_selectedSeries = series;
            break;
        }
        index++;
    }

    if (m_selectedSeries->selectedBar() == QBar3DSeries::invalidSelectionPosition())
        m_selectedSeries = 0;

    qDebug() << "Selected bar position:" << position << "series:" << index;
}

void GraphModifier::setUseNullInputHandler(bool useNull)
{
    qDebug() << "setUseNullInputHandler" << useNull;
    if (m_useNullInputHandler == useNull)
        return;

    m_useNullInputHandler = useNull;

    if (useNull)
        m_graph->setActiveInputHandler(0);
    else
        m_graph->setActiveInputHandler(m_defaultInputHandler);
}

void GraphModifier::handleRowAxisChanged(QCategory3DAxis *axis)
{
    qDebug() << __FUNCTION__ << axis << axis->orientation() << (axis == m_graph->rowAxis());
}

void GraphModifier::handleColumnAxisChanged(QCategory3DAxis *axis)
{
    qDebug() << __FUNCTION__ << axis << axis->orientation() << (axis == m_graph->columnAxis());
}

void GraphModifier::handleValueAxisChanged(QValue3DAxis *axis)
{
    qDebug() << __FUNCTION__ << axis << axis->orientation() << (axis == m_graph->valueAxis());
}

void GraphModifier::handlePrimarySeriesChanged(QBar3DSeries *series)
{
    qDebug() << __FUNCTION__ << series;
}

void GraphModifier::changeShadowQuality(int quality)
{
    QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
    emit shadowQualityChanged(quality);
}

void GraphModifier::showFiveSeries()
{
    releaseProxies();
    releaseAxes();
    m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemRowAndColumn | QAbstract3DGraph::SelectionMultiSeries);

    m_dummyData->dataProxy()->resetArray(makeDummyData(), QStringList(), QStringList());
    m_dummyData2->dataProxy()->resetArray(makeDummyData(), QStringList(), QStringList());
    m_dummyData3->dataProxy()->resetArray(makeDummyData(), QStringList(), QStringList());
    m_dummyData4->dataProxy()->resetArray(makeDummyData(), QStringList(), QStringList());
    m_dummyData5->dataProxy()->resetArray(makeDummyData(), QStringList(), QStringList());

    m_graph->addSeries(m_dummyData);
    m_graph->addSeries(m_dummyData2);
    m_graph->addSeries(m_dummyData3);
    m_graph->addSeries(m_dummyData4);

    // Toggle between four and five series
    static int count = 0;
    if (++count % 2)
        m_graph->addSeries(m_dummyData5);
}

QBarDataArray *GraphModifier::makeDummyData()
{
    // Set up data
    static const float temp[4][4] = {
        {10.0f, 5.0f, 10.0f, 5.0f},
        {5.0f, 10.0f, 5.0f, 10.0f},
        {10.0f, 5.0f, 10.0f, 5.0f},
        {5.0f, 10.0f, 5.0f, 10.0f}
    };

    // Create data rows
    QBarDataArray *dataSet = new QBarDataArray;
    QBarDataRow *dataRow;

    dataSet->reserve(4);
    for (int i = 0; i < 4; i++) {
        dataRow = new QBarDataRow(4);
        // Create data items
        for (int j = 0; j < 4; j++) {
            // Add data to rows
            (*dataRow)[j].setValue(temp[i][j]);
        }
        // Add row to set
        dataSet->append(dataRow);
    }
    return dataSet;
}

// Executes one step of the primary series test
void GraphModifier::primarySeriesTest()
{
    static int nextStep = 0;

    QStringList testLabels;
    QStringList testLabels2;
    QStringList testLabels3;
    QStringList testLabels5;
    testLabels << "1" << "2" << "3" << "4";
    testLabels2 << "11" << "22" << "33" << "44";
    testLabels3 << "111" << "222" << "333" << "444";
    testLabels5 << "11111" << "22222" << "33333" << "44444";

    switch (nextStep++) {
    case 0: {
        qDebug() << "Step 0 - Init:";
        m_graph->addSeries(m_dummyData); // Add one series to enforce release in releaseProxies()
        releaseProxies();
        releaseAxes();
        m_dummyData->dataProxy()->resetArray(makeDummyData(),
                                             testLabels,
                                             QStringList() << "A" << "B" << "C" << "D");
        m_dummyData2->dataProxy()->resetArray(makeDummyData(),
                                              testLabels2,
                                              QStringList() << "AA" << "BB" << "CC" << "DD");
        m_dummyData3->dataProxy()->resetArray(makeDummyData(),
                                              testLabels3,
                                              QStringList() << "AAA" << "BBB" << "CCC" << "DDD");
        m_dummyData4->dataProxy()->resetArray(makeDummyData(),
                                              QStringList() << "1111" << "2222" << "3333" << "4444",
                                              QStringList() << "AAAA" << "BBBB" << "CCCC" << "DDDD");
        m_dummyData5->dataProxy()->resetArray(makeDummyData(),
                                              testLabels5,
                                              QStringList() << "AAAAA" << "BBBBB" << "CCCCC" << "DDDDD");

        m_graph->addSeries(m_dummyData);
        m_graph->addSeries(m_dummyData2);
        m_graph->addSeries(m_dummyData3);

        m_dummyData->setBaseColor(Qt::black);
        m_dummyData2->setBaseColor(Qt::white);
        m_dummyData3->setBaseColor(Qt::red);
        m_dummyData4->setBaseColor(Qt::blue);
        m_dummyData5->setBaseColor(Qt::green);

        if (m_graph->primarySeries() == m_dummyData)
            if (m_graph->rowAxis()->labels() == testLabels)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData, actual: " << m_graph->primarySeries();
        break;
    }
    case 1: {
        qDebug() << "Step 1 - Set another series as primary:";
        m_graph->setPrimarySeries(m_dummyData3);
        if (m_graph->primarySeries() == m_dummyData3) {
            if (m_graph->rowAxis()->labels() == testLabels3)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        } else {
            qDebug() << "--> FAIL!!! Primary should be m_dummyData3, actual: " << m_graph->primarySeries();
        }
        break;
    }
    case 2: {
        qDebug() << "Step 2 - Add new series:";
        m_graph->addSeries(m_dummyData4);
        if (m_graph->primarySeries() == m_dummyData3)
            if (m_graph->rowAxis()->labels() == testLabels3)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData3, actual: " << m_graph->primarySeries();
        break;
    }
    case 3: {
        qDebug() << "Step 3 - Reset primary series:";
        m_graph->setPrimarySeries(0);
        if (m_graph->primarySeries() == m_dummyData)
            if (m_graph->rowAxis()->labels() == testLabels)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData, actual: " << m_graph->primarySeries();
        break;
    }
    case 4: {
        qDebug() << "Step 4 - Set new series at primary:";
        m_graph->setPrimarySeries(m_dummyData5);
        if (m_graph->primarySeries() == m_dummyData5)
            if (m_graph->rowAxis()->labels() == testLabels5)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData5, actual: " << m_graph->primarySeries();
        break;
    }
    case 5: {
        qDebug() << "Step 5 - Remove nonexistent series:";
        m_graph->removeSeries(0);
        if (m_graph->primarySeries() == m_dummyData5)
            if (m_graph->rowAxis()->labels() == testLabels5)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData5, actual: " << m_graph->primarySeries();
        break;
    }
    case 6: {
        qDebug() << "Step 6 - Remove non-primary series:";
        m_graph->removeSeries(m_dummyData);
        if (m_graph->primarySeries() == m_dummyData5)
            if (m_graph->rowAxis()->labels() == testLabels5)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData5, actual: " << m_graph->primarySeries();
        break;
    }
    case 7: {
        qDebug() << "Step 7 - Remove primary series:";
        m_graph->removeSeries(m_dummyData5);
        if (m_graph->primarySeries() == m_dummyData2) // first series removed, second should be first now
            if (m_graph->rowAxis()->labels() == testLabels2)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData3, actual: " << m_graph->primarySeries();
        break;
    }
    case 8: {
        qDebug() << "Step 8 - move a series (m_dummyData2) forward to a different position";
        m_graph->insertSeries(3, m_dummyData2);
        if (m_graph->primarySeries() == m_dummyData2)
            if (m_graph->seriesList().at(2) == m_dummyData2) // moving series forward, index decrements
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Moved to incorrect index, index 2 has:" << m_graph->seriesList().at(2);
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData3, actual: " << m_graph->primarySeries();
        break;
    }
    case 9: {
        qDebug() << "Step 9 - move a series (m_dummyData4) backward to a different position";
        m_graph->insertSeries(0, m_dummyData4);
        if (m_graph->primarySeries() == m_dummyData2)
            if (m_graph->seriesList().at(0) == m_dummyData4)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Moved to incorrect index, index 0 has:" << m_graph->seriesList().at(0);
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData3, actual: " << m_graph->primarySeries();
        break;
    }
    case 10: {
        qDebug() << "Step 10 - Insert a series (m_dummyData) series to position 2";
        m_graph->insertSeries(2, m_dummyData);
        if (m_graph->primarySeries() == m_dummyData2)
            if (m_graph->seriesList().at(2) == m_dummyData)
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Moved to incorrect index, index 2 has:" << m_graph->seriesList().at(2);
        else
            qDebug() << "--> FAIL!!! Primary should be m_dummyData3, actual: " << m_graph->primarySeries();
        break;
    }
    case 11: {
        qDebug() << "Step 11 - Remove everything";
        m_graph->removeSeries(m_dummyData);
        m_graph->removeSeries(m_dummyData2);
        m_graph->removeSeries(m_dummyData3);
        m_graph->removeSeries(m_dummyData4);
        m_graph->removeSeries(m_dummyData5);
        if (m_graph->primarySeries() == 0)
            if (m_graph->rowAxis()->labels() == QStringList())
                qDebug() << "--> SUCCESS";
            else
                qDebug() << "--> FAIL!!! Row labels incorrect: " << m_graph->rowAxis()->labels();
        else
            qDebug() << "--> FAIL!!! Primary should be null, actual: " << m_graph->primarySeries();
        break;
    }
    default:
        qDebug() << "-- Restarting test sequence --";
        nextStep = 0;
        break;
    }


}

void GraphModifier::insertRemoveTestToggle()
{
    if (m_insertRemoveTimer.isActive()) {
        m_insertRemoveTimer.stop();
        m_selectionTimer.stop();
        m_graph->removeSeries(m_dummyData);
        m_graph->removeSeries(m_dummyData2);
        releaseProxies();
        releaseAxes();
        m_graph->setActiveInputHandler(m_defaultInputHandler);
    } else {
        releaseProxies();
        releaseAxes();
        m_graph->rowAxis()->setRange(0, 32);
        m_graph->columnAxis()->setRange(0, 10);
        m_graph->setActiveInputHandler(m_customInputHandler);
        m_graph->addSeries(m_dummyData);
        m_graph->addSeries(m_dummyData2);
        m_insertRemoveStep = 0;
        m_insertRemoveTimer.start(100);
        m_selectionTimer.start(10);
    }
}

void GraphModifier::toggleRotation()
{
    if (m_rotationTimer.isActive())
        m_rotationTimer.stop();
    else
        m_rotationTimer.start(20);
}

void GraphModifier::useLogAxis()
{
    static int counter = -1;
    static QLogValue3DAxisFormatter *logFormatter = new QLogValue3DAxisFormatter;
    static float minRange = 1.0f;
    counter++;

    switch (counter) {
    case 0: {
        qDebug() << "Case" << counter << ": Default log axis";
        logFormatter = new QLogValue3DAxisFormatter;
        m_graph->valueAxis()->setFormatter(logFormatter);
        m_graph->valueAxis()->setRange(minRange, 1200.0f);
        m_graph->valueAxis()->setLabelFormat(QStringLiteral("%.3f"));
        break;
    }
    case 1: {
        qDebug() << "Case" << counter << ": Hide max label";
        logFormatter->setShowEdgeLabels(false);
        break;
    }
    case 2: {
        qDebug() << "Case" << counter << ": Try to hide subgrid unsuccessfully";
        m_graph->valueAxis()->setSubSegmentCount(1);
        break;
    }
    case 3: {
        qDebug() << "Case" << counter << ": Hide subgrid property";
        logFormatter->setAutoSubGrid(false);
        m_graph->valueAxis()->setSubSegmentCount(1);
        break;
    }
    case 4: {
        qDebug() << "Case" << counter << ": Different base: 2";
        logFormatter->setBase(2.0f);
        logFormatter->setAutoSubGrid(true);
        break;
    }
    case 5: {
        qDebug() << "Case" << counter << ": Different base: 16";
        logFormatter->setBase(16.0f);
        break;
    }
    case 6: {
        qDebug() << "Case" << counter << ": Invalid bases";
        logFormatter->setBase(-1.0f);
        logFormatter->setBase(1.0f);
        break;
    }
    case 7: {
        qDebug() << "Case" << counter << ": Zero base";
        logFormatter->setBase(0.0f);
        break;
    }
    case 8: {
        qDebug() << "Case" << counter << ": Explicit ranges and segments";
        int segmentCount = 6;
        int base = 4;
        m_graph->valueAxis()->setSegmentCount(segmentCount);
        m_graph->valueAxis()->setSubSegmentCount(base - 1);
        m_graph->valueAxis()->setRange(1.0f / float(base * base), qPow(base, segmentCount - 2));
        break;
    }
    case 9: {
        qDebug() << "Case" << counter << ": Negative range";
        m_graph->valueAxis()->setRange(-20.0f, 50.0f);
        break;
    }
    case 10: {
        qDebug() << "Case" << counter << ": Non-integer base";
        logFormatter->setBase(3.3f);
        logFormatter->setAutoSubGrid(true);
        break;
    }
    default:
        qDebug() << "Resetting logaxis test";
        minRange++;
        counter = -1;
        break;
    }


}

void GraphModifier::changeValueAxisFormat(const QString & text)
{
    m_graph->valueAxis()->setLabelFormat(text);
}

void GraphModifier::changeLogBase(const QString &text)
{
    QLogValue3DAxisFormatter *formatter =
        qobject_cast<QLogValue3DAxisFormatter *>(m_graph->valueAxis()->formatter());
    if (formatter)
        formatter->setBase(qreal(text.toDouble()));
}

void GraphModifier::addRemoveSeries()
{
    static int counter = 0;

    switch (counter) {
    case 0: {
        qDebug() << __FUNCTION__ << counter << "New series";
        m_extraSeries = new QBar3DSeries;
        m_graph->addSeries(m_extraSeries);
        QObject::connect(m_extraSeries, &QBar3DSeries::selectedBarChanged, this,
                         &GraphModifier::handleSelectionChange);
    }
        break;
    case 1: {
        qDebug() << __FUNCTION__ << counter << "Add data to series";
        QBarDataArray *array = new QBarDataArray;
        array->reserve(5);
        for (int i = 0; i < 5; i++) {
            array->append(new QBarDataRow(10));
            for (int j = 0; j < 10; j++)
                (*array->at(i))[j].setValue(i * j);
        }
        m_extraSeries->dataProxy()->resetArray(array);
    }
        break;
    case 2: {
        qDebug() << __FUNCTION__ << counter << "Hide series";
        m_extraSeries->setVisible(false);
    }
        break;
    case 3: {
        qDebug() << __FUNCTION__ << counter << "Modify data when hidden";
        QBarDataArray array;
        array.reserve(5);
        for (int i = 0; i < 5; i++) {
            array.append(new QBarDataRow(10));
            for (int j = 0; j < 10; j++)
                (*array.at(i))[j].setValue(2 * i * j);
        }
        m_extraSeries->dataProxy()->addRows(array);
    }
        break;
    case 4: {
        qDebug() << __FUNCTION__ << counter << "Show series again";
        m_extraSeries->setVisible(true);
    }
        break;
    case 5: {
        qDebug() << __FUNCTION__ << counter << "Remove series";
        m_graph->removeSeries(m_extraSeries);
        delete m_extraSeries;
        m_extraSeries = 0;
    }
        break;
    default:
        qDebug() << __FUNCTION__ << "Resetting test";
        counter = -1;
    }
    counter++;
}

void GraphModifier::changeValueAxisSegments(int value)
{
    qDebug() << __FUNCTION__ << value;
    m_segments = value;
    m_graph->valueAxis()->setSegmentCount(m_segments);
}

void GraphModifier::insertRemoveTimerTimeout()
{
    if (m_insertRemoveStep < 32) {
        for (int k = 0; k < 1; k++) {
            QBarDataRow *dataRow = new QBarDataRow(10);
            for (float i = 0; i < 10; i++)
                (*dataRow)[i].setValue(((i + 1) / 10.0f) * (float)(rand() % 100));

            QString label = QStringLiteral("Insert %1").arg(insertCounter++);
            m_dummyData->dataProxy()->insertRow(0, dataRow, label);
        }
    } else {
        for (int k = 0; k < 1; k++)
            m_dummyData->dataProxy()->removeRows(0, 1);
    }

    if (m_insertRemoveStep < 16 || (m_insertRemoveStep > 31 && m_insertRemoveStep < 48)) {
        for (int k = 0; k < 2; k++) {
            QBarDataRow *dataRow = new QBarDataRow(10);
            for (float i = 0; i < 10; i++)
                (*dataRow)[i].setValue(((i + 1) / 10.0f) * (float)(rand() % 100));

            QString label = QStringLiteral("Insert %1").arg(insertCounter++);
            m_dummyData2->dataProxy()->insertRow(0, dataRow, label);
        }
    } else {
        for (int k = 0; k < 2; k++)
            m_dummyData2->dataProxy()->removeRows(0, 1);
    }

    if (m_insertRemoveStep++ > 63)
        m_insertRemoveStep = 0;
}

void GraphModifier::triggerSelection()
{
    m_graph->scene()->setSelectionQueryPosition(m_customInputHandler->inputPosition());
}

void GraphModifier::triggerRotation()
{
    if (m_selectedSeries) {
        QPoint selectedBar = m_selectedSeries->selectedBar();
        if (selectedBar != QBar3DSeries::invalidSelectionPosition()) {
            QBarDataItem item(*(m_selectedSeries->dataProxy()->itemAt(selectedBar.x(), selectedBar.y())));
            item.setRotation(item.rotation() + 1.0f);
            m_selectedSeries->dataProxy()->setItem(selectedBar.x(), selectedBar.y(), item);
        }
    } else {
        // Rotate the first series instead
        static float seriesAngle = 0.0f;
        if (m_graph->seriesList().size())
            m_graph->seriesList().at(0)->setMeshAngle(seriesAngle++);
    }
}

void GraphModifier::handleValueAxisLabelsChanged()
{
    qDebug() << __FUNCTION__;
}

void GraphModifier::handleFpsChange(qreal fps)
{
    static const QString fpsPrefix(QStringLiteral("FPS: "));
    m_fpsLabel->setText(fpsPrefix + QString::number(qRound(fps)));
}

void GraphModifier::setBackgroundEnabled(int enabled)
{
    m_graph->activeTheme()->setBackgroundEnabled(bool(enabled));
}

void GraphModifier::setGridEnabled(int enabled)
{
    m_graph->activeTheme()->setGridEnabled(bool(enabled));
}

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

void GraphModifier::setFpsMeasurement(bool enable)
{
    m_graph->setMeasureFps(enable);
}

void GraphModifier::setSpecsRatio(int barwidth)
{
    m_graph->setBarThickness((float)barwidth / 30.0f);
}

void GraphModifier::setSpacingSpecsX(int spacing)
{
    m_barSpacingX = (float)spacing / 100.0f;
    m_graph->setBarSpacing(QSizeF(m_barSpacingX, m_barSpacingZ));
}

void GraphModifier::setSpacingSpecsZ(int spacing)
{
    m_barSpacingZ = (float)spacing / 100.0f;
    m_graph->setBarSpacing(QSizeF(m_barSpacingX, m_barSpacingZ));
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

void GraphModifier::changeColorStyle()
{
    int style = m_graph->activeTheme()->colorStyle();

    if (++style > Q3DTheme::ColorStyleRangeGradient)
        style = Q3DTheme::ColorStyleUniform;

    m_graph->activeTheme()->setColorStyle(Q3DTheme::ColorStyle(style));
}

void GraphModifier::useOwnTheme()
{
    // Own theme is persistent, any changes to it via UI will be remembered
    if (!m_ownTheme) {
        m_ownTheme = new Q3DTheme();
        m_ownTheme->setBackgroundEnabled(true);
        m_ownTheme->setGridEnabled(true);
        m_ownTheme->setAmbientLightStrength(0.3f);
        m_ownTheme->setBackgroundColor(QColor(QRgb(0x99ca53)));
        QList<QColor> colors;
        colors.append(QColor(QRgb(0x209fdf)));
        m_ownTheme->setBaseColors(colors);
        m_ownTheme->setColorStyle(Q3DTheme::ColorStyleUniform);
        m_ownTheme->setGridLineColor(QColor(QRgb(0x99ca53)));
        m_ownTheme->setHighlightLightStrength(7.0f);
        m_ownTheme->setLabelBackgroundEnabled(true);
        m_ownTheme->setLabelBorderEnabled(true);
        m_ownTheme->setLightColor(Qt::white);
        m_ownTheme->setLightStrength(6.0f);
        m_ownTheme->setMultiHighlightColor(QColor(QRgb(0x6d5fd5)));
        m_ownTheme->setSingleHighlightColor(QColor(QRgb(0xf6a625)));
        m_ownTheme->setLabelBackgroundColor(QColor(0xf6, 0xa6, 0x25, 0xa0));
        m_ownTheme->setLabelTextColor(QColor(QRgb(0x404044)));
        m_ownTheme->setWindowColor(QColor(QRgb(0xffffff)));
    }

    m_graph->setActiveTheme(m_ownTheme);

    m_colorDialog->open();
}

void GraphModifier::changeBaseColor(const QColor &color)
{
    qDebug() << "base color changed to" << color;
    QList<QColor> colors;
    colors.append(color);
    m_graph->activeTheme()->setBaseColors(colors);
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

    QList<QLinearGradient> barGradients;
    barGradients.append(barGradient);
    m_graph->activeTheme()->setBaseGradients(barGradients);
    m_graph->activeTheme()->setSingleHighlightGradient(singleHighlightGradient);
    m_graph->activeTheme()->setMultiHighlightGradient(multiHighlightGradient);

    m_graph->activeTheme()->setColorStyle(Q3DTheme::ColorStyleObjectGradient);
}

void GraphModifier::toggleMultiseriesScaling()
{
    m_graph->setMultiSeriesUniform(!m_graph->isMultiSeriesUniform());
}

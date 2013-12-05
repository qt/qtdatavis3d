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
#include <QtDataVisualization/q3dtheme.h>
#include <QTime>

QT_DATAVISUALIZATION_USE_NAMESPACE

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
      m_segments(4),
      m_subSegments(3),
      m_minval(-16.0f),
      m_maxval(20.0f),
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

    m_graph->addAxis(m_autoAdjustingAxis);
    m_graph->addAxis(m_fixedRangeAxis);
    m_graph->addAxis(m_temperatureAxis);
    m_graph->addAxis(m_yearAxis);
    m_graph->addAxis(m_monthAxis);
    m_graph->addAxis(m_genericRowAxis);
    m_graph->addAxis(m_genericColumnAxis);

    m_graph->setTheme(new Q3DTheme(Q3DTheme::ThemeStoneMoss));
    m_graph->setShadowQuality(QDataVis::ShadowQualitySoftMedium);

    m_temperatureData->setItemLabelFormat(QStringLiteral("1: @valueTitle for @colLabel @rowLabel: @valueLabel"));
    m_temperatureData2->setItemLabelFormat(QStringLiteral("2: @valueTitle for @colLabel @rowLabel: @valueLabel"));
    m_genericData->setItemLabelFormat(QStringLiteral("3: @valueTitle for (@rowIdx, @colIdx): @valueLabel"));
    m_genericData->dataProxy()->setColumnLabels(genericColumnLabels);

    m_graph->theme()->setFont(QFont("Times Roman", 20));

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

    m_graph->addSeries(m_temperatureData);
    m_graph->addSeries(m_temperatureData2);

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
        int startRow = qMax(row - 2, 0);
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

    m_graph->setTheme(new Q3DTheme((Q3DTheme::Theme)theme));

    if (++theme > Q3DTheme::ThemeIsabelle)
        theme = Q3DTheme::ThemeQt;
}

void GraphModifier::changeLabelStyle()
{
    m_graph->theme()->setLabelBackgroundEnabled(!m_graph->theme()->isLabelBackgroundEnabled());
}

void GraphModifier::changeSelectionMode()
{
    static int selectionMode = m_graph->selectionMode();

    if (++selectionMode > (QDataVis::SelectionItemAndColumn | QDataVis::SelectionSlice | QDataVis::SelectionMultiSeries))
        selectionMode = QDataVis::SelectionNone;

    m_graph->setSelectionMode((QDataVis::SelectionFlag)selectionMode);
}

void GraphModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSize(m_fontSize);
    m_graph->theme()->setFont(newFont);
}

void GraphModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_graph->theme()->font();
    font.setPointSize(m_fontSize);
    m_graph->theme()->setFont(font);
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
    int index = 0;
    foreach (QBar3DSeries *series, m_graph->seriesList()) {
        if (series == sender())
            break;
        index++;
    }

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


void GraphModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
    emit shadowQualityChanged(quality);
}

void GraphModifier::setBackgroundEnabled(int enabled)
{
    m_graph->theme()->setBackgroundEnabled(bool(enabled));
}

void GraphModifier::setGridEnabled(int enabled)
{
    m_graph->theme()->setGridEnabled(bool(enabled));
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
    int style = m_graph->theme()->colorStyle();

    if (++style > Q3DTheme::ColorStyleRangeGradient)
        style = Q3DTheme::ColorStyleUniform;

    m_graph->theme()->setColorStyle(Q3DTheme::ColorStyle(style));
}

void GraphModifier::useOwnTheme()
{
    Q3DTheme *theme = new Q3DTheme();
    theme->setBackgroundEnabled(true);
    theme->setGridEnabled(true);
    theme->setAmbientLightStrength(0.3f);
    theme->setBackgroundColor(QColor(QRgb(0x99ca53)));
    theme->setBaseColor(QColor(QRgb(0x209fdf)));
    theme->setColorStyle(Q3DTheme::ColorStyleUniform);
    theme->setGridLineColor(QColor(QRgb(0x99ca53)));
    theme->setHighlightLightStrength(7.0f);
    theme->setLabelBackgroundEnabled(true);
    theme->setLabelBorderEnabled(true);
    theme->setLightColor(Qt::white);
    theme->setLightStrength(6.0f);
    theme->setMultiHighlightColor(QColor(QRgb(0x6d5fd5)));
    theme->setSingleHighlightColor(QColor(QRgb(0xf6a625)));
    theme->setLabelBackgroundColor(QColor(0xf6, 0xa6, 0x25, 0xa0));
    theme->setLabelTextColor(QColor(QRgb(0x404044)));
    theme->setWindowColor(QColor(QRgb(0xffffff)));

    m_graph->setTheme(theme);

    m_colorDialog->open();
}

void GraphModifier::changeBaseColor(const QColor &color)
{
    qDebug() << "base color changed to" << color;
    m_graph->theme()->setBaseColor(color);
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

    m_graph->theme()->setBaseGradient(barGradient);
    m_graph->theme()->setSingleHighlightGradient(singleHighlightGradient);
    m_graph->theme()->setMultiHighlightGradient(multiHighlightGradient);

    m_graph->theme()->setColorStyle(Q3DTheme::ColorStyleObjectGradient);
}

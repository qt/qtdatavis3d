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

#include "scatterchart.h"
#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtDataVisualization/qscatter3dseries.h>
#include <QtDataVisualization/q3dvalueaxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/q3dtheme.h>
#include <qmath.h>
using namespace QtDataVisualization;

//#define RANDOM_SCATTER

const int numberOfItems = 10000;

ScatterDataModifier::ScatterDataModifier(Q3DScatter *scatter)
    : m_chart(scatter),
      m_fontSize(30.0f),
      m_loopCounter(0),
      m_selectedItem(-1),
      m_targetSeries(0)
{
    QFont font = m_chart->font();
    font.setPointSize(m_fontSize);
    m_chart->setFont(font);
    m_chart->setObjectType(QDataVis::MeshStyleSpheres, true);
    m_chart->setTheme(new Q3DTheme(QDataVis::ThemeStoneMoss));
    m_chart->setShadowQuality(QDataVis::ShadowQualityNone);
    m_chart->scene()->activeCamera()->setCameraPreset(QDataVis::CameraPresetFront);
    m_chart->setAxisX(new Q3DValueAxis);
    m_chart->setAxisY(new Q3DValueAxis);
    m_chart->setAxisZ(new Q3DValueAxis);

    createAndAddSeries();
    createAndAddSeries();

    m_chart->setSelectionMode(QDataVis::SelectionItem);

    QObject::connect(&m_timer, &QTimer::timeout, this, &ScatterDataModifier::timeout);
    QObject::connect(m_chart, &Q3DScatter::shadowQualityChanged, this,
                     &ScatterDataModifier::shadowQualityUpdatedByVisual);
}

ScatterDataModifier::~ScatterDataModifier()
{
    delete m_chart;
}

void ScatterDataModifier::start()
{
    addData();
}

void ScatterDataModifier::addData()
{
    // Add labels
    m_chart->axisX()->setTitle("X");
    m_chart->axisY()->setTitle("Y");
    m_chart->axisZ()->setTitle("Z");
    m_chart->axisX()->setRange(-50.0, 50.0);
    m_chart->axisY()->setRange(-1.0, 1.2);
    m_chart->axisZ()->setRange(-50.0, 50.0);

    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(numberOfItems);
    QScatterDataItem *ptrToDataArray = &dataArray->first();
    QScatterDataArray *dataArray2 = new QScatterDataArray;
    dataArray2->resize(numberOfItems);
    QScatterDataItem *ptrToDataArray2 = &dataArray2->first();

#ifdef RANDOM_SCATTER
    for (int i = 0; i < numberOfItems; i++) {
        ptrToDataArray->setPosition(randVector());
        ptrToDataArray++;
        ptrToDataArray2->setPosition(randVector());
        ptrToDataArray2++;
    }
#else
    float limit = qSqrt(numberOfItems) / 2.0f;
    for (float i = -limit; i < limit; i++) {
        for (float j = -limit; j < limit; j++) {
            ptrToDataArray->setPosition(QVector3D(i, qCos(qDegreesToRadians((i * j) / 7.5)), j));
            ptrToDataArray++;
            ptrToDataArray2->setPosition(QVector3D(i, qCos(qDegreesToRadians((i * j) / 7.5)) + 0.2, j));
            ptrToDataArray2++;
        }
    }
#endif

    m_chart->seriesList().at(0)->dataProxy()->resetArray(dataArray);
    m_chart->seriesList().at(1)->dataProxy()->resetArray(dataArray2);
}

void ScatterDataModifier::changeStyle()
{
    static int model = 0;
    switch (model) {
    case 0:
        m_chart->setObjectType(QDataVis::MeshStyleDots, false);
        break;
    case 1:
        m_chart->setObjectType(QDataVis::MeshStyleDots, true);
        break;
    case 2:
        m_chart->setObjectType(QDataVis::MeshStyleSpheres, false);
        break;
    case 3:
        m_chart->setObjectType(QDataVis::MeshStyleSpheres, true);
        break;
    case 4:
        m_chart->setObjectType(QDataVis::MeshStylePoints);
        break;
    }
    model++;
    if (model > 4)
        model = 0;
}

void ScatterDataModifier::changePresetCamera()
{
    static int preset = QDataVis::CameraPresetFrontLow;

    m_chart->scene()->activeCamera()->setCameraPreset((QDataVis::CameraPreset)preset);

    if (++preset > QDataVis::CameraPresetDirectlyAboveCCW45)
        preset = QDataVis::CameraPresetFrontLow;
}

void ScatterDataModifier::changeTheme()
{
    static int theme = QDataVis::ThemeQt;

    m_chart->setTheme(new Q3DTheme(QDataVis::Theme(theme)));

    if (++theme > QDataVis::ThemeIsabelle)
        theme = QDataVis::ThemeQt;
}

void ScatterDataModifier::changeLabelStyle()
{
    static int style = QDataVis::LabelStyleOpaque;

    m_chart->setLabelStyle((QDataVis::LabelStyle)style);

    if (++style > QDataVis::LabelStyleTransparent)
        style = QDataVis::LabelStyleOpaque;
}

void ScatterDataModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    m_chart->setFont(newFont);
}

void ScatterDataModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_chart->font();
    font.setPointSize(m_fontSize);
    m_chart->setFont(font);
}

void ScatterDataModifier::shadowQualityUpdatedByVisual(QDataVis::ShadowQuality sq)
{
    int quality = int(sq);
     // Updates the UI component to show correct shadow quality
    emit shadowQualityChanged(quality);
}

void ScatterDataModifier::clear()
{
    foreach (QScatter3DSeries *series, m_chart->seriesList()) {
        m_chart->removeSeries(series);
        delete series;
    }

    m_targetSeries = 0;

    qDebug() << m_loopCounter << "Cleared array";
}

void ScatterDataModifier::resetAxes()
{
    m_chart->releaseAxis(m_chart->axisX());
    m_chart->releaseAxis(m_chart->axisY());
    m_chart->releaseAxis(m_chart->axisZ());

    m_chart->setAxisX(new Q3DValueAxis);
    m_chart->setAxisY(new Q3DValueAxis);
    m_chart->setAxisZ(new Q3DValueAxis);
    m_chart->axisX()->setSegmentCount(5);
    m_chart->axisY()->setSegmentCount(5);
    m_chart->axisZ()->setSegmentCount(5);
    m_chart->axisX()->setTitle("X");
    m_chart->axisY()->setTitle("Y");
    m_chart->axisZ()->setTitle("Z");
}

void ScatterDataModifier::addOne()
{
    if (!m_targetSeries)
        createAndAddSeries();

    QScatterDataItem item(randVector());
    int addIndex = m_targetSeries->dataProxy()->addItem(item);
    qDebug() << m_loopCounter << "added one to index:" << addIndex << "array size:" << m_targetSeries->dataProxy()->array()->size();
}

void ScatterDataModifier::addBunch()
{
    if (!m_targetSeries)
        createAndAddSeries();

    QScatterDataArray items(100);
    for (int i = 0; i < items.size(); i++)
        items[i].setPosition(randVector());
    int addIndex = m_targetSeries->dataProxy()->addItems(items);
    qDebug() << m_loopCounter << "added bunch to index:" << addIndex << "array size:" << m_targetSeries->dataProxy()->array()->size();
}

void ScatterDataModifier::insertOne()
{
    if (!m_targetSeries)
        createAndAddSeries();

    QScatterDataItem item(randVector());
    m_targetSeries->dataProxy()->insertItem(0, item);
    qDebug() << m_loopCounter << "Inserted one, array size:" << m_targetSeries->dataProxy()->array()->size();
}

void ScatterDataModifier::insertBunch()
{
    if (!m_targetSeries)
        createAndAddSeries();

    QScatterDataArray items(100);
    for (int i = 0; i < items.size(); i++)
        items[i].setPosition(randVector());
    m_targetSeries->dataProxy()->insertItems(0, items);
    qDebug() << m_loopCounter << "Inserted bunch, array size:" << m_targetSeries->dataProxy()->array()->size();
}

void ScatterDataModifier::changeOne()
{
    if (!m_targetSeries)
        createAndAddSeries();

    if (m_selectedItem >= 0 && m_targetSeries->dataProxy()->array()->size()) {
        QScatterDataItem item(randVector());
        m_targetSeries->dataProxy()->setItem(m_selectedItem, item);
        qDebug() << m_loopCounter << "Changed one, array size:" << m_targetSeries->dataProxy()->array()->size();
    }
}

void ScatterDataModifier::changeBunch()
{
    if (!m_targetSeries)
        createAndAddSeries();

    if (m_targetSeries->dataProxy()->array()->size()) {
        int amount = qMin(m_targetSeries->dataProxy()->array()->size(), 100);
        QScatterDataArray items(amount);
        for (int i = 0; i < items.size(); i++)
            items[i].setPosition(randVector());
        m_targetSeries->dataProxy()->setItems(0, items);
        qDebug() << m_loopCounter << "Changed bunch, array size:" << m_targetSeries->dataProxy()->array()->size();
    }
}

void ScatterDataModifier::removeOne()
{
    if (!m_targetSeries)
        createAndAddSeries();

    if (m_selectedItem >= 0) {
        m_targetSeries->dataProxy()->removeItems(m_selectedItem, 1);
        qDebug() << m_loopCounter << "Removed one, array size:" << m_targetSeries->dataProxy()->array()->size();
    }
}

void ScatterDataModifier::removeBunch()
{
    if (!m_targetSeries)
        createAndAddSeries();

    m_targetSeries->dataProxy()->removeItems(0, 100);
    qDebug() << m_loopCounter << "Removed bunch, array size:" << m_targetSeries->dataProxy()->array()->size();
}

void ScatterDataModifier::timeout()
{
    int doWhat = rand() % 10;
    if (!(rand() % 100))
        doWhat = -1;

    switch (doWhat) {
    case 0:
        addOne();
        break;
    case 1:
        addBunch();
        break;
    case 2:
        insertOne();
        break;
    case 3:
        insertBunch();
        break;
    case 4:
        changeOne();
        break;
    case 5:
        changeBunch();
        break;
    case 6:
        removeOne();
        break;
    case 7:
        removeBunch();
        break;
    case 8:
        addSeries();
        break;
    case 9:
        if (m_chart->seriesList().size())
            m_targetSeries = m_chart->seriesList().at(rand() % m_chart->seriesList().size());
        else
            addSeries();
        break;
    default:
        clear();
        break;
    }

    m_loopCounter++;
}

void ScatterDataModifier::startStopTimer()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    } else {
        clear();
        m_loopCounter = 0;
        m_timer.start(0);
    }
}

void ScatterDataModifier::selectItem()
{
    if (!m_targetSeries)
        createAndAddSeries();

    int targetItem(3);
    int noSelection(-1);
    if (m_selectedItem != targetItem || m_targetSeries != m_chart->seriesList().at(0))
        m_chart->seriesList().at(0)->setSelectedItem(targetItem);
    else
        m_chart->seriesList().at(0)->setSelectedItem(noSelection);
}

void ScatterDataModifier::handleSelectionChange(int index)
{
    m_selectedItem = index;
    m_targetSeries = static_cast<QScatter3DSeries *>(sender());
    int seriesIndex = 0;
    foreach (QScatter3DSeries *series, m_chart->seriesList()) {
        if (series == sender())
            break;
        seriesIndex++;
    }

    qDebug() << "Selected item index:" << index << "series:" << seriesIndex;
}

void ScatterDataModifier::setGradient()
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
    singleHighlightGradient.setColorAt(1.0, Qt::lightGray);
    singleHighlightGradient.setColorAt(0.75, Qt::blue);
    singleHighlightGradient.setColorAt(0.50, Qt::red);
    singleHighlightGradient.setColorAt(0.25, Qt::yellow);
    singleHighlightGradient.setColorAt(0.0, Qt::white);

    m_chart->setItemColor(Qt::green);
    m_chart->setSingleHighlightColor(Qt::white);

    m_chart->setItemGradient(barGradient);
    m_chart->setSingleHighlightGradient(singleHighlightGradient);

    QDataVis::ColorStyle oldStyle = m_chart->colorStyle();
    if (oldStyle == QDataVis::ColorStyleUniform)
        m_chart->setColorStyle(QDataVis::ColorStyleObjectGradient);
    else if (oldStyle == QDataVis::ColorStyleObjectGradient)
        m_chart->setColorStyle(QDataVis::ColorStyleRangeGradient);
    if (oldStyle == QDataVis::ColorStyleRangeGradient)
        m_chart->setColorStyle(QDataVis::ColorStyleUniform);
}

void ScatterDataModifier::addSeries()
{
    QScatter3DSeries *series = createAndAddSeries();

    QScatter3DSeries *oldTargetSeries = m_targetSeries;
    m_targetSeries = series; // adding always adds to target series, so fake it for a bit
    addOne(); // add one random item to start the new series off
    m_targetSeries = oldTargetSeries;
}

void ScatterDataModifier::changeShadowQuality(int quality)
{
    QDataVis::ShadowQuality sq = QDataVis::ShadowQuality(quality);
    m_chart->setShadowQuality(sq);
    emit shadowQualityChanged(quality);
}

void ScatterDataModifier::setBackgroundEnabled(int enabled)
{
    m_chart->setBackgroundVisible((bool)enabled);
}

void ScatterDataModifier::setGridEnabled(int enabled)
{
    m_chart->setGridVisible((bool)enabled);
}

QVector3D ScatterDataModifier::randVector()
{
    return QVector3D(
        (float)(rand() % 100) / 2.0f - (float)(rand() % 100) / 2.0f,
        (float)(rand() % 100) / 100.0f - (float)(rand() % 100) / 100.0f,
        (float)(rand() % 100) / 2.0f - (float)(rand() % 100) / 2.0f);
}

QScatter3DSeries *ScatterDataModifier::createAndAddSeries()
{
    static int counter = 0;

    QScatter3DSeries *series = new QScatter3DSeries;

    if (!m_targetSeries)
        m_targetSeries = series;

    m_chart->addSeries(series);
    series->setItemLabelFormat(QString("%1: @xLabel - @yLabel - @zLabel").arg(counter++));

    QObject::connect(series, &QScatter3DSeries::selectedItemChanged, this,
                     &ScatterDataModifier::handleSelectionChange);

    return series;
}

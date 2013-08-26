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

#include "scatterchart.h"
#include <QtDataVis3D/qscatterdataproxy.h>
#include <QtDataVis3D/qvalueaxis.h>
#include <qmath.h>
using namespace QtDataVis3D;

//#define RANDOM_SCATTER

const int numberOfItems = 10000;

ScatterDataModifier::ScatterDataModifier(Q3DScatter *scatter)
    : m_chart(scatter),
      m_fontSize(30.0f)
{
    QFont font = m_chart->font();
    font.setPointSize(m_fontSize);
    m_chart->setFont(font);
    m_chart->setObjectType(QDataVis::Spheres, true);
    m_chart->setTheme(QDataVis::ThemeBrownSand);
    m_chart->setShadowQuality(QDataVis::ShadowHigh);
    m_chart->setCameraPreset(QDataVis::PresetFront);

    QScatterDataProxy *proxy = new QScatterDataProxy;
    proxy->setItemLabelFormat("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel");
    m_chart->setDataProxy(proxy);

    connect(&m_timer, &QTimer::timeout, this, &ScatterDataModifier::timeout);
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
    m_chart->valueAxisX()->setTitle("X");
    m_chart->valueAxisY()->setTitle("Y");
    m_chart->valueAxisZ()->setTitle("Z");
    m_chart->valueAxisX()->setRange(-50.0, 50.0);
    m_chart->valueAxisY()->setRange(-1.0, 1.0);
    m_chart->valueAxisZ()->setRange(-50.0, 50.0);

    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(numberOfItems);
    QScatterDataItem *ptrToDataArray = &dataArray->first();

#ifdef RANDOM_SCATTER
    for (int i = 0; i < numberOfItems; i++) {
        ptrToDataArray->setPosition(randVector());
        ptrToDataArray++;
    }
#else
    float limit = qSqrt(numberOfItems) / 2.0f;
    for (float i = -limit; i < limit; i++) {
        for (float j = -limit; j < limit; j++) {
            ptrToDataArray->setPosition(QVector3D(i, qCos(qDegreesToRadians((i * j) / 7.5)), j));
            ptrToDataArray++;
        }
    }
#endif

    static_cast<QScatterDataProxy *>(m_chart->dataProxy())->resetArray(dataArray);
}

void ScatterDataModifier::changeStyle()
{
    static int model = 0;
    switch (model) {
    case 0:
        m_chart->setObjectType(QDataVis::Dots, false);
        break;
    case 1:
        m_chart->setObjectType(QDataVis::Dots, true);
        break;
    case 2:
        m_chart->setObjectType(QDataVis::Spheres, false);
        break;
    case 3:
        m_chart->setObjectType(QDataVis::Spheres, true);
        break;
    }
    model++;
    if (model > 3)
        model = 0;
}

void ScatterDataModifier::changePresetCamera()
{
    static int preset = QDataVis::PresetFrontLow;

    m_chart->setCameraPreset((QDataVis::CameraPreset)preset);

    if (++preset > QDataVis::PresetDirectlyAboveCCW45)
        preset = QDataVis::PresetFrontLow;
}

void ScatterDataModifier::changeTheme()
{
    static int theme = QDataVis::ThemeSystem;

    m_chart->setTheme((QDataVis::ColorTheme)theme);

    if (++theme > QDataVis::ThemeLight)
        theme = QDataVis::ThemeSystem;
}

void ScatterDataModifier::changeTransparency()
{
    static int transparency = QDataVis::TransparencyNone;

    m_chart->setLabelTransparency((QDataVis::LabelTransparency)transparency);

    if (++transparency > QDataVis::TransparencyNoBackground)
        transparency = QDataVis::TransparencyNone;
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

void ScatterDataModifier::clear()
{
    m_chart->dataProxy()->resetArray(0);
    qDebug() << m_loopCounter << "Cleared array";
}

void ScatterDataModifier::addOne()
{
    QScatterDataItem item(randVector());
    int addIndex = m_chart->dataProxy()->addItem(item);
    qDebug() << m_loopCounter << "added one to index:" << addIndex << "array size:" << m_chart->dataProxy()->array()->size();
}

void ScatterDataModifier::addBunch()
{
    QScatterDataArray items(100);
    for (int i = 0; i < items.size(); i++)
        items[i].setPosition(randVector());
    int addIndex = m_chart->dataProxy()->addItems(items);
    qDebug() << m_loopCounter << "added bunch to index:" << addIndex << "array size:" << m_chart->dataProxy()->array()->size();
}

void ScatterDataModifier::insertOne()
{
    QScatterDataItem item(randVector());
    m_chart->dataProxy()->insertItem(0, item);
    qDebug() << m_loopCounter << "Inserted one, array size:" << m_chart->dataProxy()->array()->size();
}

void ScatterDataModifier::insertBunch()
{
    QScatterDataArray items(100);
    for (int i = 0; i < items.size(); i++)
        items[i].setPosition(randVector());
    m_chart->dataProxy()->insertItems(0, items);
    qDebug() << m_loopCounter << "Inserted bunch, array size:" << m_chart->dataProxy()->array()->size();
}

void ScatterDataModifier::changeOne()
{
    if (m_chart->dataProxy()->array()->size()) {
        QScatterDataItem item(randVector());
        m_chart->dataProxy()->setItem(0, item);
        qDebug() << m_loopCounter << "Changed one, array size:" << m_chart->dataProxy()->array()->size();
    }
}

void ScatterDataModifier::changeBunch()
{
    if (m_chart->dataProxy()->array()->size()) {
        int amount = qMin(m_chart->dataProxy()->array()->size(), 100);
        QScatterDataArray items(amount);
        for (int i = 0; i < items.size(); i++)
            items[i].setPosition(randVector());
        m_chart->dataProxy()->setItems(0, items);
        qDebug() << m_loopCounter << "Changed bunch, array size:" << m_chart->dataProxy()->array()->size();
    }
}

void ScatterDataModifier::removeOne()
{
    m_chart->dataProxy()->removeItems(0, 1);
    qDebug() << m_loopCounter << "Removed one, array size:" << m_chart->dataProxy()->array()->size();
}

void ScatterDataModifier::removeBunch()
{
    m_chart->dataProxy()->removeItems(0, 100);
    qDebug() << m_loopCounter << "Removed bunch, array size:" << m_chart->dataProxy()->array()->size();
}

void ScatterDataModifier::timeout()
{
    int doWhat = rand() % 8;
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

void ScatterDataModifier::changeShadowQuality(int quality)
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

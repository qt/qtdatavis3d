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
    m_chart->setFontSize(m_fontSize);
    m_chart->setBarType(Spheres, true);
    m_chart->setTheme(ThemeBrownSand);
    m_chart->setShadowQuality(ShadowHigh);

    QScatterDataProxy *proxy = new QScatterDataProxy;
    m_chart->setDataProxy(proxy);
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
    m_chart->valueAxisX()->setTitle("Somethings");
    m_chart->valueAxisY()->setTitle("Values");
    m_chart->valueAxisZ()->setTitle("Others");

    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(numberOfItems);
    QScatterDataItem *ptrToDataArray = &dataArray->first();

#if RANDOM_SCATTER
    for (int i = 0; i < numberOfItems; i++) {
        ptrToDataArray->setPosition(
                    QVector3D((qreal)(rand() % 100) / 100.0 - (qreal)(rand() % 100) / 100.0,
                              (qreal)(rand() % 100) / 100.0 - (qreal)(rand() % 100) / 100.0,
                              (qreal)(rand() % 100) / 100.0 - (qreal)(rand() % 100) / 100.0));
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
        m_chart->setBarType(Dots, false);
        break;
    case 1:
        m_chart->setBarType(Dots, true);
        break;
    case 2:
        m_chart->setBarType(Spheres, false);
        break;
    case 3:
        m_chart->setBarType(Spheres, true);
        break;
    }
    model++;
    if (model > 3)
        model = 0;
}

void ScatterDataModifier::changePresetCamera()
{
    static int preset = PresetFrontLow;

    m_chart->setCameraPreset((CameraPreset)preset);

    if (++preset > PresetDirectlyAboveCCW45)
        preset = PresetFrontLow;
}

void ScatterDataModifier::changeTheme()
{
    static int theme = ThemeSystem;

    m_chart->setTheme((ColorTheme)theme);

    if (++theme > ThemeLight)
        theme = ThemeSystem;
}

void ScatterDataModifier::changeTransparency()
{
    static int transparency = TransparencyNone;

    m_chart->setLabelTransparency((LabelTransparency)transparency);

    if (++transparency > TransparencyNoBackground)
        transparency = TransparencyNone;
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
    m_chart->setFontSize((GLfloat)m_fontSize);
}

void ScatterDataModifier::shadowQualityUpdatedByVisual(ShadowQuality sq)
{
    int quality = 0;
    switch (sq) {
    case ShadowLow:
        quality = 1;
        break;
    case ShadowMedium:
        quality = 2;
        break;
    case ShadowHigh:
        quality = 3;
        break;
    }

    // Updates the UI component to show correct shadow quality
    emit shadowQualityChanged(quality);
}

void ScatterDataModifier::changeShadowQuality(int quality)
{
    ShadowQuality sq = ShadowNone;
    switch (quality) {
    case 1:
        sq = ShadowLow;
        break;
    case 2:
        sq = ShadowMedium;
        break;
    case 3:
        sq = ShadowHigh;
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

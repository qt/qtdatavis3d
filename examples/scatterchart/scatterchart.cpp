/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "scatterchart.h"
#include "qscatterdataproxy.h"
#include "qcategoryaxis.h"

using namespace QtDataVis3D;

const int numberOfItems = 10000;

ScatterDataModifier::ScatterDataModifier(Q3DScatter *scatter)
    : m_chart(scatter),
      m_fontSize(30.0f)
{
    m_chart->setFontSize(m_fontSize);

    m_chart->setTheme(ThemeBlueIcy);

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
    QStringList rowLabels;
    rowLabels << "something -1.0" << "something -0.8" << "something -0.6" << "something -0.4"
              << "something -0.2" << "something 0.0" << "something 0.2" << "something 0.4"
              << "something 0.6" << "something 0.8" << "something 1.0";
    QStringList columnLabels;
    columnLabels << "other -1.0" << "other -0.8" << "other -0.6" << "other -0.4" << "other -0.2"
                 << "other 0.0" << "other 0.2" << "other 0.4" << "other 0.6" << "other 0.8"
                 << "other 1.0";

    // Set tick count and step
    m_chart->setTickCount(10, 0.1f, -1.0f);

    // Add labels
    m_chart->rowAxis()->setTitle("Somethings");
    m_chart->columnAxis()->setTitle("Others");
    m_chart->valueAxis()->setTitle("Values");
    m_chart->rowAxis()->setLabels(columnLabels);
    m_chart->columnAxis()->setLabels(rowLabels);

    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(numberOfItems);
    QScatterDataItem *ptrToDataArray = &dataArray->first();

    for (int i = 0; i < numberOfItems; i++) {
        //qDebug() << i << ptrToDataArray;
        ptrToDataArray->setPosition(
                    QVector3D((qreal)(rand() % 100) / 100.0 - (qreal)(rand() % 100) / 100.0,
                              (qreal)(rand() % 100) / 100.0 - (qreal)(rand() % 100) / 100.0,
                              (qreal)(rand() % 100) / 100.0 - (qreal)(rand() % 100) / 100.0));
        ptrToDataArray++;
    }

    static_cast<QScatterDataProxy *>(m_chart->dataProxy())->resetArray(dataArray);
}

void ScatterDataModifier::changeStyle()
{
    static int model = 3;
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
        transparency = TransparencyFromTheme;
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

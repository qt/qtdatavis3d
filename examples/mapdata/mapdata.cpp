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

#include "mapdata.h"
#include "qmapdataproxy.h"
#include <QImage>
#include <QFile>

using namespace QtDataVis3D;

MapsModifier::MapsModifier(Q3DMaps *maps)
    : m_chart(maps),
      m_imageRect(QRect()),
      m_fontSize(80.0f),
      m_barSpecs(QVector3D(30.0f, 30.0f, 30.0f))
{
    QImage image = QImage(QStringLiteral(":/images/finland"));
    m_imageRect = image.rect();
    m_chart->setAreaSpecs(m_imageRect, image);
    m_chart->setFontSize(m_fontSize);

#if 0
    m_chart->setBarSpecs(m_barSpecs, Q3DMaps::AdjustHeight);
    m_chart->setBarType(BevelBars, true);
    m_chart->setTheme(ThemeBlueIcy);
    m_chart->setShadowQuality(ShadowMedium);
#else
    m_chart->setBarSpecs(m_barSpecs, Q3DMaps::AdjustAll);
    m_chart->setMeshFileName(QStringLiteral(":/meshes/weirdthing"));
#endif
    QMapDataProxy *proxy = new QMapDataProxy;
    m_chart->setDataProxy(proxy);
}

MapsModifier::~MapsModifier()
{
    delete m_chart;
}

void MapsModifier::start()
{
    addData();
}

void MapsModifier::addData()
{
    QMapDataArray *dataArray = new QMapDataArray;
    QMapDataItem *item;
    item = new QMapDataItem();
    item->setValue(191050);
    item->setLabel("Oulu");
    item->setMapPosition(QPointF(963, 1604));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(22274);
    item->setLabel("Kemi");
    item->setMapPosition(QPointF(857, 1383));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(60887);
    item->setLabel("Rovaniemi");
    item->setMapPosition(QPointF(1061, 1119));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(16176);
    item->setLabel("Kuusamo");
    item->setMapPosition(QPointF(1459, 1284));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(3998);
    item->setLabel("Ivalo");
    item->setMapPosition(QPointF(1239, 474));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(37978);
    item->setLabel("Kajaani");
    item->setMapPosition(QPointF(1285, 1859));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(46809);
    item->setLabel("Kokkola");
    item->setMapPosition(QPointF(580, 1973));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(105236);
    item->setLabel("Kuopio");
    item->setMapPosition(QPointF(1292, 2283));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(133557);
    item->setLabel("Jyväskylä");
    item->setMapPosition(QPointF(991, 2496));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(65771);
    item->setLabel("Vaasa");
    item->setMapPosition(QPointF(441, 2184));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(217603);
    item->setLabel("Tampere");
    item->setMapPosition(QPointF(686, 2656));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(180350);
    item->setLabel("Turku");
    item->setMapPosition(QPointF(430, 3046));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(72400);
    item->setLabel("Lappeenranta");
    item->setMapPosition(QPoint(1365, 2852));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(14754);
    item->setLabel("Tammisaari");
    item->setMapPosition(QPointF(605, 3215));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(1879);
    item->setLabel("Enontekiö");
    item->setMapPosition(QPointF(752, 556));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(36624);
    item->setLabel("Savonlinna");
    item->setMapPosition(QPointF(1445, 2586));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(605022);
    item->setLabel("Helsinki");
    item->setMapPosition(QPointF(822, 3130));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(54887);
    item->setLabel("Kotka");
    item->setMapPosition(QPointF(1162, 3051));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(11372);
    item->setLabel("Maarianhamina");
    item->setMapPosition(QPointF(56, 3101));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(9266);
    item->setLabel("Hanko");
    item->setMapPosition(QPointF(527, 3228));
    dataArray->append(*item);
    delete item;

    item = new QMapDataItem();
    item->setValue(1287);
    item->setLabel("Utsjoki");
    item->setMapPosition(QPointF(1180, 72));
    dataArray->append(*item);
    delete item;

    static_cast<QMapDataProxy *>(m_chart->dataProxy())->resetArray(dataArray);
}

void MapsModifier::changeStyle()
{
    static int model = 0;
    switch (model) {
    case 0:
        m_chart->setBarType(Cylinders, false);
        break;
    case 1:
        m_chart->setBarType(Cylinders, true);
        break;
    case 2:
        m_chart->setBarType(Cones, false);
        break;
    case 3:
        m_chart->setBarType(Cones, true);
        break;
    case 4:
        m_chart->setBarType(Bars, false);
        break;
    case 5:
        m_chart->setBarType(Bars, true);
        break;
    case 6:
        m_chart->setBarType(Pyramids, false);
        break;
    case 7:
        m_chart->setBarType(Pyramids, true);
        break;
    case 8:
        m_chart->setBarType(BevelBars, false);
        break;
    case 9:
        m_chart->setBarType(BevelBars, true);
        break;
    case 10:
        m_chart->setBarType(Spheres, false);
        break;
    case 11:
        m_chart->setBarType(Spheres, true);
        break;
    }
    model++;
    if (model > 11)
        model = 0;
}

void MapsModifier::changePresetCamera()
{
    static int preset = PresetFrontLow;

    m_chart->setCameraPreset((CameraPreset)preset);

    if (++preset > PresetDirectlyAboveCCW45)
        preset = PresetFrontLow;
}

void MapsModifier::changeTheme()
{
    static int theme = ThemeSystem;

    m_chart->setTheme((ColorTheme)theme);

    if (++theme > ThemeLight)
        theme = ThemeSystem;
}

void MapsModifier::changeTransparency()
{
    static int transparency = TransparencyNone;

    m_chart->setLabelTransparency((LabelTransparency)transparency);

    if (++transparency > TransparencyNoBackground)
        transparency = TransparencyFromTheme;
}

void MapsModifier::changeValueDimension(int dimension)
{
    m_chart->setBarSpecs(m_barSpecs, (Q3DMaps::AdjustmentDirection)dimension);
}

void MapsModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    //qDebug() << newFont << newFont.style();
    m_chart->setFont(newFont);
}

void MapsModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    m_chart->setFontSize((GLfloat)m_fontSize);
}

void MapsModifier::changeShadowQuality(int quality)
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
    ShadowQuality realquality = m_chart->setShadowQuality(sq);
    // Check if it setting quality was successful
    if (realquality != sq) {
        switch (realquality) {
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
        emit shadowQuality(quality);
    }
}

//void MapsModifier::setGridEnabled(int enabled)
//{
//    m_chart->setGridEnabled((bool)enabled);
//}

//void MapsModifier::rotateX(int rotation)
//{
//    m_xRotation = rotation;
//    m_chart->setCameraPosition(m_xRotation, m_yRotation);
//}

//void MapsModifier::rotateY(int rotation)
//{
//    m_yRotation = rotation;
//    m_chart->setCameraPosition(m_xRotation, m_yRotation);
//}

//void MapsModifier::setSpecsX(int barwidth)
//{
//    m_barWidth = (float)barwidth / 100.0f;
//    m_chart->setBarSpecs(QSizeF(m_barWidth, m_barDepth), QSizeF(m_barSpacingX, m_barSpacingZ));
//}

//void MapsModifier::setSpecsZ(int bardepth)
//{
//    m_barDepth = (float)bardepth / 100.0f;
//    m_chart->setBarSpecs(QSizeF(m_barWidth, m_barDepth), QSizeF(m_barSpacingX, m_barSpacingZ));
//}

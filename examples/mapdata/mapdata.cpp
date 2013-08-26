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

#include "mapdata.h"
#include <QtDataVis3D/qmapdataproxy.h>
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
    QFont font = m_chart->font();
    font.setPointSize(m_fontSize);
    m_chart->setFont(font);

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
    case 10:
        m_chart->setBarType(QDataVis::Spheres, false);
        break;
    case 11:
        m_chart->setBarType(QDataVis::Spheres, true);
        break;
    }
    model++;
    if (model > 11)
        model = 0;
}

void MapsModifier::changePresetCamera()
{
    static int preset = QDataVis::PresetFrontLow;

    m_chart->setCameraPreset((QDataVis::CameraPreset)preset);

    if (++preset > QDataVis::PresetDirectlyAboveCCW45)
        preset = QDataVis::PresetFrontLow;
}

void MapsModifier::changeTheme()
{
    static int theme = QDataVis::ThemeSystem;

    m_chart->setTheme((QDataVis::ColorTheme)theme);

    if (++theme > QDataVis::ThemeLight)
        theme = QDataVis::ThemeSystem;
}

void MapsModifier::changeTransparency()
{
    static int transparency = QDataVis::TransparencyNone;

    m_chart->setLabelTransparency((QDataVis::LabelTransparency)transparency);

    if (++transparency > QDataVis::TransparencyNoBackground)
        transparency = QDataVis::TransparencyFromTheme;
}

void MapsModifier::changeValueDimension(int dimension)
{
    m_chart->setBarSpecs(m_barSpecs, (Q3DMaps::AdjustmentDirection)dimension);
}

void MapsModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPointSizeF(m_fontSize);
    m_chart->setFont(newFont);
}

void MapsModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_chart->font();
    font.setPointSize(m_fontSize);
    m_chart->setFont(font);
}

void MapsModifier::changeShadowQuality(int quality)
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
    QDataVis::ShadowQuality realquality = m_chart->setShadowQuality(sq);
    // Check if it setting quality was successful
    if (realquality != sq) {
        switch (realquality) {
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

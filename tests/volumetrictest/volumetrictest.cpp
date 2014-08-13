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

#include "volumetrictest.h"
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtCore/qmath.h>
#include <QtGui/QRgb>
#include <QtGui/QImage>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>

using namespace QtDataVisualization;

const int imageCount = 512;

VolumetricModifier::VolumetricModifier(Q3DScatter *scatter)
    : m_graph(scatter),
      m_volumeItem(0),
      m_sliceIndexX(0),
      m_sliceIndexY(0),
      m_sliceIndexZ(0)
{
    m_graph->activeTheme()->setType(Q3DTheme::ThemeQt);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
    m_graph->setOrthoProjection(true);
    //m_graph->scene()->activeCamera()->setTarget(QVector3D(0.5f, 0.5f, 0.5f));

    createVolume();
    createAnotherVolume();

    m_graph->addCustomItem(m_volumeItem);
    m_graph->addCustomItem(m_volumeItem2);
    m_graph->setMeasureFps(true);

    QObject::connect(m_graph->scene()->activeCamera(), &Q3DCamera::zoomLevelChanged, this,
                     &VolumetricModifier::handleZoomLevelChange);
    QObject::connect(m_graph, &QAbstract3DGraph::currentFpsChanged, this,
                     &VolumetricModifier::handleFpsChange);
}

VolumetricModifier::~VolumetricModifier()
{
    delete m_graph;
}

void VolumetricModifier::setFpsLabel(QLabel *fpsLabel)
{
    m_fpsLabel = fpsLabel;
}

void VolumetricModifier::sliceX(int enabled)
{
    m_volumeItem->setSliceIndexX(enabled ? m_sliceIndexX : -1);
    m_volumeItem2->setSliceIndexX(enabled ? m_sliceIndexX : -1);
}

void VolumetricModifier::sliceY(int enabled)
{
    m_volumeItem->setSliceIndexY(enabled ? m_sliceIndexY : -1);
    m_volumeItem2->setSliceIndexY(enabled ? m_sliceIndexY : -1);
}

void VolumetricModifier::sliceZ(int enabled)
{
    m_volumeItem->setSliceIndexZ(enabled ? m_sliceIndexZ : -1);
    m_volumeItem2->setSliceIndexZ(enabled ? m_sliceIndexZ : -1);
}

void VolumetricModifier::adjustSliceX(int value)
{
    m_sliceIndexX = int(float(value) / (1024.0f / m_volumeItem->textureWidth()));
    if (m_sliceIndexX == m_volumeItem->textureWidth())
        m_sliceIndexX--;
    qDebug() << "m_sliceIndexX:" << m_sliceIndexX;
    if (m_volumeItem->sliceIndexX() != -1) {
        m_volumeItem->setSliceIndexX(m_sliceIndexX);
        m_volumeItem2->setSliceIndexX(m_sliceIndexX);
    }
}

void VolumetricModifier::adjustSliceY(int value)
{
    m_sliceIndexY = int(float(value) / (1024.0f / m_volumeItem->textureHeight()));
    if (m_sliceIndexY == m_volumeItem->textureHeight())
        m_sliceIndexY--;
    qDebug() << "m_sliceIndexY:" << m_sliceIndexY;
    if (m_volumeItem->sliceIndexY() != -1) {
        m_volumeItem->setSliceIndexY(m_sliceIndexY);
        m_volumeItem2->setSliceIndexY(m_sliceIndexY);
    }
}

void VolumetricModifier::adjustSliceZ(int value)
{
    m_sliceIndexZ = int(float(value) / (1024.0f / m_volumeItem->textureDepth()));
    if (m_sliceIndexZ == m_volumeItem->textureDepth())
        m_sliceIndexZ--;
    qDebug() << "m_sliceIndexZ:" << m_sliceIndexZ;
    if (m_volumeItem->sliceIndexZ() != -1) {
        m_volumeItem->setSliceIndexZ(m_sliceIndexZ);
        m_volumeItem2->setSliceIndexZ(m_sliceIndexZ);
    }
}

void VolumetricModifier::handleZoomLevelChange()
{
    // Zooming inside volumetric object causes ugly clipping issues, so restrict zoom level a bit
    if (m_graph->scene()->activeCamera()->zoomLevel() > 220)
        m_graph->scene()->activeCamera()->setZoomLevel(220);
}

void VolumetricModifier::handleFpsChange(qreal fps)
{
    const QString fpsFormat = QStringLiteral("Fps: %1");
    int fps10 = int(fps * 10.0);
    m_fpsLabel->setText(fpsFormat.arg(QString::number(qreal(fps10) / 10.0)));
}

void VolumetricModifier::createVolume()
{
    m_volumeItem = new QCustom3DVolume;
    m_volumeItem->setTextureFormat(QImage::Format_ARGB32);
    m_volumeItem->setRotation(QQuaternion::fromAxisAndAngle(1.0f, 1.0f, 0.0f, 10.0f));
    m_volumeItem->setPosition(QVector3D(-0.5f, 0.0f, 0.0f));

    QImage logo;
    logo.load(QStringLiteral(":/logo.png"));
    qDebug() << "image dimensions:" << logo.width() << logo.height()
             << logo.byteCount() << (logo.width() * logo.height())
             << logo.bytesPerLine();

    QVector<QImage *> imageArray(imageCount);
    for (int i = 0; i < imageCount; i++) {
        QImage *newImage = new QImage(logo);
        imageArray[i] = newImage;
    }

    m_volumeItem->createTextureData(imageArray);

    m_sliceIndexX = m_volumeItem->textureWidth() / 2;
    m_sliceIndexY = m_volumeItem->textureWidth() / 2;
    m_sliceIndexZ = m_volumeItem->textureWidth() / 2;

    QVector<QRgb> colorTable = m_volumeItem->colorTable();
    colorTable.append(qRgba(255, 0, 0, 255));
    colorTable.append(qRgba(0, 255, 0, 255));
    colorTable.append(qRgba(0, 0, 255, 255));
    m_volumeItem->setColorTable(colorTable);
    int redIndex = colorTable.size() - 3;
    int greenIndex = colorTable.size() - 2;
    int blueIndex = colorTable.size() - 1;
    int width = m_volumeItem->textureDataWidth();
    int height = m_volumeItem->textureHeight();
    int depth = m_volumeItem->textureDepth();
    int frameSize = width * height;
    qDebug() << width << height << depth << m_volumeItem->textureData()->size();
    m_volumeItem->setScaling(QVector3D(float(width) / float(depth) * 2.0f,
                                       float(height) / float(depth) * 2.0f,
                                       2.0f));

    uchar *data = m_volumeItem->textureData()->data();
    uchar *p = data;

    // Change one picture using subtexture replacement
    QImage flipped = logo.mirrored();
    m_volumeItem->setSubTextureData(101, flipped);

    // Red first subtexture
//    for (int j = 0; j < height; j++) {
//        for (int i = 0; i < width; i++) {
//            *p = redIndex;
//            p++;
//        }
//    }

    // Red last subtexture
    p = data + frameSize * (imageCount - 1);
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            *p = redIndex;
            p++;
        }
    }

    // Blue x = 0
    p = data;
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            *p = blueIndex;
            p += width;
        }
    }

    // Blue x = max
    p = data + width - 1;
    for (int k = 0; k < depth; k++) {
        for (int j = 0; j < height; j++) {
            *p = blueIndex;
            p += width;
        }
    }

    // Green y = 0
//    p = data;
//    for (int k = 0; k < depth; k++) {
//        for (int i = 0; i < width; i++) {
//            *p = greenIndex;
//            p++;
//        }
//        p += (frameSize - width);
//    }

    // Green y = max
    p = data + frameSize - width;
    for (int k = 0; k < depth; k++) {
        for (int i = 0; i < width; i++) {
            *p = greenIndex;
            p++;
        }
        p += (frameSize - width);
    }
}

void VolumetricModifier::createAnotherVolume()
{
    m_volumeItem2 = new QCustom3DVolume;
    m_volumeItem2->setTextureFormat(QImage::Format_ARGB32);
    m_volumeItem2->setPosition(QVector3D(0.5f, 0.0f, 0.0f));

    QImage logo;
    logo.load(QStringLiteral(":/logo.png"));
    //logo = logo.convertToFormat(QImage::Format_ARGB8555_Premultiplied);
    qDebug() << "second image dimensions:" << logo.width() << logo.height()
             << logo.byteCount() << (logo.width() * logo.height())
             << logo.bytesPerLine();

    logo.save("d:/qt/goobar.png");

    QVector<QImage *> imageArray(imageCount);
    for (int i = 0; i < imageCount; i++) {
        QImage *newImage = new QImage(logo);
        imageArray[i] = newImage;
    }

    m_volumeItem2->createTextureData(imageArray);

    m_sliceIndexX = m_volumeItem2->textureWidth() / 2;
    m_sliceIndexY = m_volumeItem2->textureWidth() / 2;
    m_sliceIndexZ = m_volumeItem2->textureWidth() / 2;

    int width = m_volumeItem2->textureWidth();
    int height = m_volumeItem2->textureHeight();
    int depth = m_volumeItem2->textureDepth();
    qDebug() << width << height << depth << m_volumeItem2->textureData()->size();
    m_volumeItem2->setScaling(QVector3D(float(width) / float(depth) * 2.0f,
                                       float(height) / float(depth) * 2.0f,
                                       2.0f));

    // Change one picture using subtexture replacement
    QImage flipped = logo.mirrored();
    m_volumeItem2->setSubTextureData(101, flipped);
}


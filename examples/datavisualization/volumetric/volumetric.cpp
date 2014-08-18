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

#include "volumetric.h"
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtCore/qmath.h>
#include <QtGui/QRgb>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>

using namespace QtDataVisualization;

const int textureSize = 256;

VolumetricModifier::VolumetricModifier(Q3DScatter *scatter)
    : m_graph(scatter),
      m_volumeItem(0),
      m_sliceIndexX(textureSize / 2),
      m_sliceIndexY(textureSize / 4),
      m_sliceIndexZ(textureSize / 2)
{
    m_graph->activeTheme()->setType(Q3DTheme::ThemeQt);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
    m_graph->setOrthoProjection(true);

    createVolume();

    m_graph->addCustomItem(m_volumeItem);
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
}

void VolumetricModifier::sliceY(int enabled)
{
    m_volumeItem->setSliceIndexY(enabled ? m_sliceIndexY : -1);
}

void VolumetricModifier::sliceZ(int enabled)
{
    m_volumeItem->setSliceIndexZ(enabled ? m_sliceIndexZ : -1);
}

void VolumetricModifier::adjustSliceX(int value)
{
    m_sliceIndexX = value / (1024 / textureSize);
    if (m_volumeItem->sliceIndexX() != -1)
        m_volumeItem->setSliceIndexX(m_sliceIndexX);
}

void VolumetricModifier::adjustSliceY(int value)
{
    m_sliceIndexY = value / (1024 / textureSize * 2);
    if (m_volumeItem->sliceIndexY() != -1)
        m_volumeItem->setSliceIndexY(m_sliceIndexY);
}

void VolumetricModifier::adjustSliceZ(int value)
{
    m_sliceIndexZ = value / (1024 / textureSize);
    if (m_volumeItem->sliceIndexZ() != -1)
        m_volumeItem->setSliceIndexZ(m_sliceIndexZ);
}

void VolumetricModifier::handleZoomLevelChange()
{
    // Zooming inside volumetric object causes ugly clipping issues, so restrict zoom level a bit
    if (m_graph->scene()->activeCamera()->zoomLevel() > 200)
        m_graph->scene()->activeCamera()->setZoomLevel(200);
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
    m_volumeItem->setScaling(QVector3D(2.0f, 1.0f, 2.0f));
    m_volumeItem->setTextureWidth(textureSize);
    m_volumeItem->setTextureHeight(textureSize / 2);
    m_volumeItem->setTextureDepth(textureSize);
    m_volumeItem->setTextureFormat(QImage::Format_Indexed8);

    // Generate color table. First color is fully transparent used to fill outer
    // portions of the volume. The second, red layer, is somewhat transparent.
    // Rest of to colors are opaque.
    QVector<QRgb> colors;
    colors.resize(256);

    colors[0] = qRgba(0, 0, 0, 0);
    for (int i = 1; i < 256; i++) {
        if (i < 60) {
            colors[i] = qRgba((i * 2) + 120, 0, 0, 100);
        } else if (i < 120) {
            colors[i] = qRgba(0, ((i - 60) * 2) + 120, 0, 255);
        } else if (i < 180) {
            colors[i] = qRgba(0, 0, ((i - 120) * 2) + 120, 255);
        } else {
            colors[i] = qRgba(i, i, i, 255);
        }
    }
    m_volumeItem->setColorTable(colors);

    // Generate texture data for an half-ellipsoid.
    // This can take a while if the dimensions are large.
    // Note that in real world cases, the texture data is usually be supplied
    // as a stack of slice images.

    QVector<uchar> *textureData = new QVector<uchar>(textureSize * textureSize * textureSize / 2);

    QVector3D midPoint(float(textureSize) / 2.0f,
                       float(textureSize) / 2.0f,
                       float(textureSize) / 2.0f);

    int index = 0;
    for (int i = 0; i < textureSize; i++) {
        for (int j = 0; j < textureSize / 2; j++) {
            for (int k = 0; k < textureSize; k++) {
                int colorIndex = 0;
                // Take a slice out of the ellipsoid
                if (i >= textureSize / 2 || j >= textureSize / 4 || k >= textureSize / 2) {
                    QVector3D distVec = QVector3D(float(k), float(j * 2), float(i)) - midPoint;
                    float adjLen = qMin(255.0f, (distVec.length() * 512.0f / float(textureSize)));
                    if (adjLen < 230)
                        colorIndex = 255 - int(adjLen);
                    else
                        colorIndex = 0;
                }

                (*textureData)[index] = colorIndex;
                index++;
            }
        }
    }

    m_volumeItem->setTextureData(textureData);
}


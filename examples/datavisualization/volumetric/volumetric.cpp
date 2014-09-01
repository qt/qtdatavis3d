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
#include <QtDataVisualization/qcustom3dlabel.h>
#include <QtCore/qmath.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtCore/QDebug>

using namespace QtDataVisualization;

const int lowDetailSize(128);
const int mediumDetailSize(256);
const int highDetailSize(512);
const int colorTableSize(256);
const int cutOffColorIndex(15);

VolumetricModifier::VolumetricModifier(Q3DScatter *scatter)
    : m_graph(scatter),
      m_volumeItem(0),
      m_sliceIndexX(lowDetailSize / 2),
      m_sliceIndexY(lowDetailSize / 4),
      m_sliceIndexZ(lowDetailSize / 2),
      m_mediumDetailRB(0),
      m_highDetailRB(0),
      m_lowDetailData(0),
      m_mediumDetailData(0),
      m_highDetailData(0),
      m_mediumDetailIndex(0),
      m_highDetailIndex(0),
      m_sliceSliderX(0),
      m_sliceSliderY(0),
      m_sliceSliderZ(0),
      m_sliceLabelX(0),
      m_sliceLabelY(0),
      m_sliceLabelZ(0)
{
    m_graph->activeTheme()->setType(Q3DTheme::ThemeQt);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
    m_graph->setOrthoProjection(true);

#if !defined(QT_OPENGL_ES_2)
    m_lowDetailData = new QVector<uchar>(lowDetailSize * lowDetailSize * lowDetailSize / 2);
    m_mediumDetailData = new QVector<uchar>(mediumDetailSize * mediumDetailSize * mediumDetailSize / 2);
    m_highDetailData = new QVector<uchar>(highDetailSize * highDetailSize * highDetailSize / 2);

    createVolume(lowDetailSize, 0, lowDetailSize, m_lowDetailData);

    m_volumeItem = new QCustom3DVolume;
    m_volumeItem->setScaling(QVector3D(2.0f, 1.0f, 2.0f));
    m_volumeItem->setTextureWidth(lowDetailSize);
    m_volumeItem->setTextureHeight(lowDetailSize / 2);
    m_volumeItem->setTextureDepth(lowDetailSize);
    m_volumeItem->setTextureFormat(QImage::Format_Indexed8);
    m_volumeItem->setTextureData(new QVector<uchar>(*m_lowDetailData));

    // Generate color tables.
    // Both tables have a fully transparent colors to fill outer portions of the volume.

    // The primary color table.
    // The top two layers are transparent.
    m_colorTable1.resize(colorTableSize);
    m_colorTable2.resize(colorTableSize);

    for (int i = 1; i < colorTableSize; i++) {
        if (i < cutOffColorIndex)
            m_colorTable1[i] = qRgba(0, 0, 0, 0);
        else if (i < 60)
            m_colorTable1[i] = qRgba((i * 2) + 120, 0, 0, 15);
        else if (i < 120)
            m_colorTable1[i] = qRgba(0, ((i - 60) * 2) + 120, 0, 50);
        else if (i < 180)
            m_colorTable1[i] = qRgba(0, 0, ((i - 120) * 2) + 120, 255);
        else
            m_colorTable1[i] = qRgba(i, i, i, 255);
    }

    // The alternate color table.
    // The first visible layer is a thin opaque color, and rest of the volume uses a smooth
    // transparent gradient.
    for (int i = 1; i < colorTableSize; i++) {
        if (i < cutOffColorIndex)
            m_colorTable2[i] = qRgba(0, 0, 0, 0);
        else if (i < cutOffColorIndex + 4)
            m_colorTable2[i] = qRgba(75, 150, 0, 255);
        else
            m_colorTable2[i] = qRgba(i, 0, 255 - i, i);
    }

    m_volumeItem->setColorTable(m_colorTable1);

    m_graph->addCustomItem(m_volumeItem);

    m_timer.start(0);
#else
    // OpenGL ES2 doesn't support 3D textures, so show a warning label instead
    QCustom3DLabel *warningLabel = new QCustom3DLabel(
                "QCustom3DVolume is not supported with OpenGL ES2",
                QFont(),
                QVector3D(0.0f, 0.5f, 0.0f),
                QVector3D(1.5f, 1.5f, 0.0f),
                QQuaternion());
    warningLabel->setPositionAbsolute(true);
    warningLabel->setFacingCamera(true);
    m_graph->addCustomItem(warningLabel);
    m_graph->activeTheme()->setLightStrength(1.0f);
#endif

    QObject::connect(m_graph->scene()->activeCamera(), &Q3DCamera::zoomLevelChanged, this,
                     &VolumetricModifier::handleZoomLevelChange);
    QObject::connect(m_graph, &QAbstract3DGraph::currentFpsChanged, this,
                     &VolumetricModifier::handleFpsChange);
    QObject::connect(&m_timer, &QTimer::timeout, this,
                     &VolumetricModifier::handleTimeout);

}

VolumetricModifier::~VolumetricModifier()
{
    delete m_graph;
}

void VolumetricModifier::setFpsLabel(QLabel *fpsLabel)
{
    m_fpsLabel = fpsLabel;
}

void VolumetricModifier::setMediumDetailRB(QRadioButton *button)
{
    m_mediumDetailRB = button;
}

void VolumetricModifier::setHighDetailRB(QRadioButton *button)
{
    m_highDetailRB = button;
}

void VolumetricModifier::setSliceLabels(QLabel *xLabel, QLabel *yLabel, QLabel *zLabel)
{
    m_sliceLabelX = xLabel;
    m_sliceLabelY = yLabel;
    m_sliceLabelZ = zLabel;

    adjustSliceX(m_sliceSliderX->value());
    adjustSliceY(m_sliceSliderY->value());
    adjustSliceZ(m_sliceSliderZ->value());
}

void VolumetricModifier::setAlphaMultiplierLabel(QLabel *label)
{
    m_alphaMultiplierLabel = label;
}

void VolumetricModifier::sliceX(int enabled)
{
    if (m_volumeItem)
        m_volumeItem->setSliceIndexX(enabled ? m_sliceIndexX : -1);
}

void VolumetricModifier::sliceY(int enabled)
{
    if (m_volumeItem)
        m_volumeItem->setSliceIndexY(enabled ? m_sliceIndexY : -1);
}

void VolumetricModifier::sliceZ(int enabled)
{
    if (m_volumeItem)
        m_volumeItem->setSliceIndexZ(enabled ? m_sliceIndexZ : -1);
}

void VolumetricModifier::adjustSliceX(int value)
{
    m_sliceIndexX = value / (1024 / m_volumeItem->textureWidth());
    if (m_sliceIndexX == m_volumeItem->textureWidth())
        m_sliceIndexX--;
    if (m_volumeItem) {
        if (m_volumeItem->sliceIndexX() != -1)
            m_volumeItem->setSliceIndexX(m_sliceIndexX);
        m_sliceLabelX->setPixmap(QPixmap::fromImage(
                                     m_volumeItem->renderSlice(Qt::XAxis, m_sliceIndexX)));
    }
}

void VolumetricModifier::adjustSliceY(int value)
{
    m_sliceIndexY = value / (1024 / m_volumeItem->textureHeight());
    if (m_sliceIndexY == m_volumeItem->textureHeight())
        m_sliceIndexY--;
    if (m_volumeItem) {
        if (m_volumeItem->sliceIndexY() != -1)
            m_volumeItem->setSliceIndexY(m_sliceIndexY);
        m_sliceLabelY->setPixmap(QPixmap::fromImage(
                                     m_volumeItem->renderSlice(Qt::YAxis, m_sliceIndexY)));
    }
}

void VolumetricModifier::adjustSliceZ(int value)
{
    m_sliceIndexZ = value / (1024 / m_volumeItem->textureDepth());
    if (m_sliceIndexZ == m_volumeItem->textureDepth())
        m_sliceIndexZ--;
    if (m_volumeItem)  {
        if (m_volumeItem->sliceIndexZ() != -1)
            m_volumeItem->setSliceIndexZ(m_sliceIndexZ);
        m_sliceLabelZ->setPixmap(QPixmap::fromImage(
                                     m_volumeItem->renderSlice(Qt::ZAxis, m_sliceIndexZ)));
    }
}

void VolumetricModifier::handleZoomLevelChange()
{
    // Zooming inside volumetric object causes ugly clipping issues, so restrict zoom level a bit
    if (m_graph->scene()->activeCamera()->zoomLevel() > 200)
        m_graph->scene()->activeCamera()->setZoomLevel(200);
}

void VolumetricModifier::handleFpsChange(qreal fps)
{
    const QString fpsFormat = QStringLiteral("FPS: %1");
    int fps10 = int(fps * 10.0);
    m_fpsLabel->setText(fpsFormat.arg(qreal(fps10) / 10.0));
}

void VolumetricModifier::handleTimeout()
{
    if (m_mediumDetailIndex < mediumDetailSize) {
        m_mediumDetailIndex = createVolume(mediumDetailSize, m_mediumDetailIndex, 4,
                                           m_mediumDetailData);
        if (m_mediumDetailIndex == mediumDetailSize) {
            m_mediumDetailRB->setEnabled(true);
            QString label = QStringLiteral("Medium (%1x%2x%1)");
            m_mediumDetailRB->setText(label.arg(mediumDetailSize).arg(mediumDetailSize / 2));
        }
    } else if (m_highDetailIndex < highDetailSize) {
        m_highDetailIndex = createVolume(highDetailSize, m_highDetailIndex, 1,
                                         m_highDetailData);
        if (m_highDetailIndex == highDetailSize) {
            m_highDetailRB->setEnabled(true);
            QString label = QStringLiteral("High (%1x%2x%1)");
            m_highDetailRB->setText(label.arg(highDetailSize).arg(highDetailSize / 2));
            m_timer.stop();
        }
    }
}

void VolumetricModifier::toggleLowDetail(bool enabled)
{
    if (enabled) {
        m_volumeItem->setTextureData(new QVector<uchar>(*m_lowDetailData));
        m_volumeItem->setTextureDimensions(lowDetailSize, lowDetailSize / 2, lowDetailSize);
        adjustSliceX(m_sliceSliderX->value());
        adjustSliceY(m_sliceSliderY->value());
        adjustSliceZ(m_sliceSliderZ->value());
    }
}

void VolumetricModifier::toggleMediumDetail(bool enabled)
{
    if (enabled) {
        m_volumeItem->setTextureData(new QVector<uchar>(*m_mediumDetailData));
        m_volumeItem->setTextureDimensions(mediumDetailSize, mediumDetailSize / 2, mediumDetailSize);
        adjustSliceX(m_sliceSliderX->value());
        adjustSliceY(m_sliceSliderY->value());
        adjustSliceZ(m_sliceSliderZ->value());
    }
}

void VolumetricModifier::toggleHighDetail(bool enabled)
{
    if (enabled) {
        m_volumeItem->setTextureData(new QVector<uchar>(*m_highDetailData));
        m_volumeItem->setTextureDimensions(highDetailSize, highDetailSize / 2, highDetailSize);
        adjustSliceX(m_sliceSliderX->value());
        adjustSliceY(m_sliceSliderY->value());
        adjustSliceZ(m_sliceSliderZ->value());
    }
}

void VolumetricModifier::setFpsMeasurement(bool enabled)
{
    m_graph->setMeasureFps(enabled);
    if (enabled)
        m_fpsLabel->setText(QStringLiteral("Measuring..."));
    else
        m_fpsLabel->setText(QString());
}

void VolumetricModifier::setSliceSliders(QSlider *sliderX, QSlider *sliderY, QSlider *sliderZ)
{
    m_sliceSliderX = sliderX;
    m_sliceSliderY = sliderY;
    m_sliceSliderZ = sliderZ;
}

void VolumetricModifier::changeColorTable(int enabled)
{
    if (enabled)
        m_volumeItem->setColorTable(m_colorTable2);
    else
        m_volumeItem->setColorTable(m_colorTable1);

    // Rerender image labels
    adjustSliceX(m_sliceSliderX->value());
    adjustSliceY(m_sliceSliderY->value());
    adjustSliceZ(m_sliceSliderZ->value());
}

void VolumetricModifier::setPreserveOpacity(bool enabled)
{
    m_volumeItem->setPreserveOpacity(enabled);

    // Rerender image labels
    adjustSliceX(m_sliceSliderX->value());
    adjustSliceY(m_sliceSliderY->value());
    adjustSliceZ(m_sliceSliderZ->value());
}

void VolumetricModifier::setUseHighDefShader(bool enabled)
{
    m_volumeItem->setUseHighDefShader(enabled);
}

void VolumetricModifier::adjustAlphaMultiplier(int value)
{
    float mult;
    if (value > 100)
        mult = float(value - 99) / 2.0f;
    else
        mult = float(value) / float(500 - value * 4);
    m_volumeItem->setAlphaMultiplier(mult);
    QString labelFormat = QStringLiteral("Alpha multiplier: %1");
    m_alphaMultiplierLabel->setText(labelFormat.arg(
                                        QString::number(m_volumeItem->alphaMultiplier(), 'f', 3)));

    // Rerender image labels
    adjustSliceX(m_sliceSliderX->value());
    adjustSliceY(m_sliceSliderY->value());
    adjustSliceZ(m_sliceSliderZ->value());
}

int VolumetricModifier::createVolume(int textureSize, int startIndex, int count,
                                     QVector<uchar> *textureData)
{
    // Generate example texture data for an half-ellipsoid with a section missing.
    // This can take a while if the dimensions are large, so we support incremental data generation.

    QVector3D midPoint(float(textureSize) / 2.0f,
                       float(textureSize) / 2.0f,
                       float(textureSize) / 2.0f);

    int index = startIndex * textureSize * textureSize / 2.0f;
    int endIndex = startIndex + count;
    if (endIndex > textureSize)
        endIndex = textureSize;
    for (int i = startIndex; i < endIndex; i++) {
        for (int j = 0; j < textureSize / 2; j++) {
            for (int k = 0; k < textureSize; k++) {
                int colorIndex = 0;
                // Take a section out of the ellipsoid
                if (i >= textureSize / 2 || j >= textureSize / 4 || k >= textureSize / 2) {
                    QVector3D distVec = QVector3D(float(k), float(j * 2), float(i)) - midPoint;
                    float adjLen = qMin(255.0f, (distVec.length() * 512.0f / float(textureSize)));
                    colorIndex = 255 - int(adjLen);
                }

                (*textureData)[index] = colorIndex;
                index++;
            }
        }
    }

    return endIndex;
}


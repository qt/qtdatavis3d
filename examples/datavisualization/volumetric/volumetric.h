// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef VOLUMETRICMODIFIER_H
#define VOLUMETRICMODIFIER_H

#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qcustom3dvolume.h>
#include <QtCore/QTimer>
#include <QtGui/QRgb>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QRadioButton>

class VolumetricModifier : public QObject
{
    Q_OBJECT
public:
    explicit VolumetricModifier(Q3DScatter *scatter);
    ~VolumetricModifier();

    void setFpsLabel(QLabel *fpsLabel);
    void setMediumDetailRB(QRadioButton *button);
    void setHighDetailRB(QRadioButton *button);
    void setSliceLabels(QLabel *xLabel, QLabel *yLabel, QLabel *zLabel);
    void setAlphaMultiplierLabel(QLabel *label);

public Q_SLOTS:
    void sliceX(int enabled);
    void sliceY(int enabled);
    void sliceZ(int enabled);
    void adjustSliceX(int value);
    void adjustSliceY(int value);
    void adjustSliceZ(int value);
    void handleFpsChange(qreal fps);
    void handleTimeout();
    void toggleLowDetail(bool enabled);
    void toggleMediumDetail(bool enabled);
    void toggleHighDetail(bool enabled);
    void setFpsMeasurement(bool enabled);
    void setSliceSliders(QSlider *sliderX, QSlider *sliderY, QSlider *sliderZ);
    void changeColorTable(int enabled);
    void setPreserveOpacity(bool enabled);
    void setTransparentGround(bool enabled);
    void setUseHighDefShader(bool enabled);
    void adjustAlphaMultiplier(int value);
    void toggleAreaAll(bool enabled);
    void toggleAreaMine(bool enabled);
    void toggleAreaMountain(bool enabled);
    void setDrawSliceFrames(int enabled);

private:
    void initHeightMap(QString fileName, QList<uchar> &layerData);
    void initMineShaftArray();
    int createVolume(int textureSize, int startIndex, int count, QList<uchar> *textureData);
    int excavateMineShaft(int textureSize, int startIndex, int count, QList<uchar> *textureData);
    void excavateMineBlock(int textureSize, int dataIndex, int size, QList<uchar> *textureData);
    void handleSlicingChanges();

    Q3DScatter *m_graph;
    QCustom3DVolume *m_volumeItem;
    int m_sliceIndexX;
    int m_sliceIndexY;
    int m_sliceIndexZ;
    bool m_slicingX;
    bool m_slicingY;
    bool m_slicingZ;
    QLabel *m_fpsLabel;
    QRadioButton *m_mediumDetailRB;
    QRadioButton *m_highDetailRB;
    QList<uchar> *m_lowDetailData;
    QList<uchar> *m_mediumDetailData;
    QList<uchar> *m_highDetailData;
    QTimer m_timer;
    int m_mediumDetailIndex;
    int m_highDetailIndex;
    int m_mediumDetailShaftIndex;
    int m_highDetailShaftIndex;
    QSlider *m_sliceSliderX;
    QSlider *m_sliceSliderY;
    QSlider *m_sliceSliderZ;
    QList<QRgb> m_colorTable1;
    QList<QRgb> m_colorTable2;
    bool m_usingPrimaryTable;
    QLabel *m_sliceLabelX;
    QLabel *m_sliceLabelY;
    QLabel *m_sliceLabelZ;
    QLabel *m_alphaMultiplierLabel;
    QList<uchar> m_magmaLayer;
    QList<uchar> m_waterLayer;
    QList<uchar> m_groundLayer;
    QList<QPair<QVector3D, QVector3D>> m_mineShaftArray;
};

#endif

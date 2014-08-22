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

#ifndef VOLUMETRICMODIFIER_H
#define VOLUMETRICMODIFIER_H

#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qcustom3dvolume.h>
#include <QtCore/QTimer>
#include <QtGui/QRgb>

class QLabel;
class QRadioButton;
class QSlider;

using namespace QtDataVisualization;

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

public slots:
    void sliceX(int enabled);
    void sliceY(int enabled);
    void sliceZ(int enabled);
    void adjustSliceX(int value);
    void adjustSliceY(int value);
    void adjustSliceZ(int value);
    void handleZoomLevelChange();
    void handleFpsChange(qreal fps);
    void handleTimeout();
    void toggleLowDetail(bool enabled);
    void toggleMediumDetail(bool enabled);
    void toggleHighDetail(bool enabled);
    void setFpsMeasurement(bool enabled);
    void setSliceSliders(QSlider *sliderX, QSlider *sliderY, QSlider *sliderZ);
    void changeColorTable(int enabled);

private:

    int createVolume(int textureSize, int startIndex, int count,
                      QVector<uchar> *textureData);

    Q3DScatter *m_graph;
    QCustom3DVolume *m_volumeItem;
    int m_sliceIndexX;
    int m_sliceIndexY;
    int m_sliceIndexZ;
    QLabel *m_fpsLabel;
    QRadioButton *m_mediumDetailRB;
    QRadioButton *m_highDetailRB;
    QVector<uchar> *m_lowDetailData;
    QVector<uchar> *m_mediumDetailData;
    QVector<uchar> *m_highDetailData;
    QTimer m_timer;
    int m_mediumDetailIndex;
    int m_highDetailIndex;
    QSlider *m_sliceSliderX;
    QSlider *m_sliceSliderY;
    QSlider *m_sliceSliderZ;
    QVector<QRgb> m_colorTable1;
    QVector<QRgb> m_colorTable2;
    QLabel *m_sliceLabelX;
    QLabel *m_sliceLabelY;
    QLabel *m_sliceLabelZ;
};

#endif

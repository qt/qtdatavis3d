/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef VOLUMETRICMODIFIER_H
#define VOLUMETRICMODIFIER_H

#include <QtDataVisualization/qcustom3dvolume.h>
#include <QtDataVisualization/qcustom3ditem.h>
#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/q3dsurface.h>
#include <QtDataVisualization/q3dbars.h>

class QLabel;

using namespace QtDataVisualization;

class VolumetricModifier : public QObject
{
    Q_OBJECT
public:
    explicit VolumetricModifier(QAbstract3DGraph *scatter);
    ~VolumetricModifier();

    void setFpsLabel(QLabel *fpsLabel);
    void setSliceLabels(QLabel *xLabel, QLabel *yLabel, QLabel *zLabel);

public slots:
    void sliceX(int enabled);
    void sliceY(int enabled);
    void sliceZ(int enabled);
    void adjustSliceX(int value);
    void adjustSliceY(int value);
    void adjustSliceZ(int value);
    void handleFpsChange();
    void testSubtextureSetting();
    void adjustRangeX(int value);
    void adjustRangeY(int value);
    void adjustRangeZ(int value);
    void testBoundsSetting();

private:
    void createVolume();
    void createAnotherVolume();
    void createYetAnotherVolume();
    void checkRenderCase(int id, Qt::Axis axis, int index, const QVector<uchar> &dataBefore,
                         QCustom3DVolume *volumeItem);

    QAbstract3DGraph *m_graph;
    Q3DScatter *m_scatterGraph;
    Q3DSurface *m_surfaceGraph;
    Q3DBars *m_barGraph;
    QCustom3DVolume *m_volumeItem;
    QCustom3DVolume *m_volumeItem2;
    QCustom3DVolume *m_volumeItem3;
    QCustom3DItem *m_plainItem;
    int m_sliceIndexX;
    int m_sliceIndexY;
    int m_sliceIndexZ;
    QLabel *m_fpsLabel;
    QLabel *m_sliceLabelX;
    QLabel *m_sliceLabelY;
    QLabel *m_sliceLabelZ;
};

#endif

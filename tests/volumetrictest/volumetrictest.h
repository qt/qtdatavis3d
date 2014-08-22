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
#include <QtDataVisualization/qcustom3ditem.h>

class QLabel;

using namespace QtDataVisualization;

class VolumetricModifier : public QObject
{
    Q_OBJECT
public:
    explicit VolumetricModifier(Q3DScatter *scatter);
    ~VolumetricModifier();

    void setFpsLabel(QLabel *fpsLabel);

public slots:
    void sliceX(int enabled);
    void sliceY(int enabled);
    void sliceZ(int enabled);
    void adjustSliceX(int value);
    void adjustSliceY(int value);
    void adjustSliceZ(int value);
    void handleFpsChange();

private:
    void createVolume();
    void createAnotherVolume();
    void createYetAnotherVolume();

    Q3DScatter *m_graph;
    QCustom3DVolume *m_volumeItem;
    QCustom3DVolume *m_volumeItem2;
    QCustom3DVolume *m_volumeItem3;
    QCustom3DItem *m_plainItem;
    int m_sliceIndexX;
    int m_sliceIndexY;
    int m_sliceIndexZ;
    QLabel *m_fpsLabel;
};

#endif

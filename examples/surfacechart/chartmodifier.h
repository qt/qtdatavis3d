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

#ifndef CHARTMODIFIER_H
#define CHARTMODIFIER_H

#include <QtDataVis3D/Q3DSurface>
#include <QSlider>

using namespace QtDataVis3D;

class ChartModifier : public QObject
{
    Q_OBJECT
public:
    enum Samples {
        SqrtSin = 1,
        Plane,
        Map
    };

    explicit ChartModifier(Q3DSurface *chart);
    ~ChartModifier();

    void toggleSmooth(bool enabled);
    void toggleSurfaceGrid(bool enable);
    void toggleSqrtSin(bool enable);
    void togglePlane(bool enable);
    void setHeightMapData(bool enable);
    void toggleGridSliderLock(bool enable);
    void setGridSliderX(QSlider *slider) { m_gridSliderX = slider; }
    void setGridSliderZ(QSlider *slider) { m_gridSliderZ = slider; }
    void adjustXCount(int count);
    void adjustZCount(int count);
    void updateSamples();

private:
    Q3DSurface *m_chart;
    QSlider *m_gridSliderX;
    QSlider *m_gridSliderZ;
    bool m_gridSlidersLocked;
    int m_xCount;
    int m_zCount;
    int m_activeSample;
};

#endif // CHARTMODIFIER_H

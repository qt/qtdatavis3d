/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

#ifndef CHARTMODIFIER_H
#define CHARTMODIFIER_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QSlider>

using namespace QtDataVisualization;

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
    void setAxisRangeSliderX(QSlider *slider) { m_axisRangeSliderX = slider; }
    void setAxisRangeSliderZ(QSlider *slider) { m_axisRangeSliderZ = slider; }
    void setAxisMinSliderX(QSlider *slider) { m_axisMinSliderX = slider; }
    void setAxisMinSliderZ(QSlider *slider) { m_axisMinSliderZ = slider; }
    void adjustXCount(int count);
    void adjustZCount(int count);
    void adjustXRange(int range);
    void adjustZRange(int range);
    void adjustXMin(int min);
    void adjustZMin(int min);
    void updateSamples();
    void gradientPressed();
    void changeFont(const QFont &font);
    void changeTransparency();

public slots:
    void changeShadowQuality(int quality);
    void changeTheme(int theme);
    void changeSelectionMode(int mode);

private:
    void resetArrayAndSliders(QSurfaceDataArray *array, qreal minZ, qreal maxZ, qreal minX,
                              qreal maxX);

    Q3DSurface *m_chart;
    QSlider *m_gridSliderX;
    QSlider *m_gridSliderZ;
    QSlider *m_axisRangeSliderX;
    QSlider *m_axisRangeSliderZ;
    QSlider *m_axisMinSliderX;
    QSlider *m_axisMinSliderZ;
    bool m_gridSlidersLocked;
    int m_xCount;
    int m_zCount;
    int m_activeSample;
    int m_fontSize;
    qreal m_rangeX;
    qreal m_rangeZ;
    qreal m_minX;
    qreal m_minZ;
};

#endif // CHARTMODIFIER_H

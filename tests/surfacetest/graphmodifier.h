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

#ifndef GRAPHMODIFIER_H
#define GRAPHMODIFIER_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QSlider>
#include <QTimer>
#include <QLabel>

using namespace QtDataVisualization;

class GraphModifier : public QObject
{
    Q_OBJECT
public:
    enum Samples {
        SqrtSin = 1,
        Plane,
        Map
    };

    explicit GraphModifier(Q3DSurface *graph);
    ~GraphModifier();

    void toggleSmooth(bool enabled);
    void toggleSurfaceGrid(bool enable);
    void toggleSurface(bool enable);
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
    void changeStyle();
    void selectButtonClicked();
    void setSelectionInfoLabel(QLabel *label) {m_selectionInfoLabel = label; }
    void selectedPointChanged(const QPoint &point);
    void changeRow();
    void changeRows();

public slots:
    void changeShadowQuality(int quality);
    void changeTheme(int theme);
    void flipViews();
    void changeSelectionMode(int mode);
    void timeout();

private:
    void resetArrayAndSliders(QSurfaceDataArray *array, float minZ, float maxZ, float minX,
                              float maxX);

    Q3DSurface *m_graph;
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
    float m_rangeX;
    float m_rangeZ;
    float m_minX;
    float m_minZ;
    QTimer m_timer;
    QSurfaceDataArray *m_planeArray;
    QLabel *m_selectionInfoLabel;
    QSurface3DSeries *m_theSeries;
};

#endif

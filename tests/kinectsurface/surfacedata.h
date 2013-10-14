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

#ifndef SURFACEDATA_H
#define SURFACEDATA_H

#include "QKinectWrapper.h"
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/Q3DBars>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QScatterDataProxy>
#include <QtDataVisualization/QBarDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QTextEdit>

using namespace QtDataVisualization;

class SurfaceData : public QObject
{
    Q_OBJECT

public:
    explicit SurfaceData(Q3DSurface *surface, Q3DScatter *scatter, Q3DBars *bars,
                         QTextEdit *statusLabel);
    ~SurfaceData();

    void start();
    void stop();

    void updateData();
    void updateStatus(QKinect::KinectStatus status);

    void setDistance(int distance);
    void scrollDown();
    void setData(const QImage &image);
    void useGradientOne();
    void useGradientTwo();

public:
    enum VisualizationMode {
        Surface = 0,
        Scatter,
        MeshStyleBars
    };

public slots:
    void setResolution(int selection);
    void changeMode(int mode);

private:
    Q3DSurface *m_surface;
    Q3DScatter *m_scatter;
    Q3DBars *m_bars;
    QTextEdit *m_statusArea;
    bool m_resize;
    QSize m_resolution;
    int m_resolutionLevel;
    VisualizationMode m_mode;
    QScatterDataArray *m_scatterDataArray;
    QBarDataArray *m_barDataArray;
    QKinect::QKinectWrapper m_kinect;
};

class ContainerChanger : public QObject
{
    Q_OBJECT

public:
    explicit ContainerChanger(QWidget *surface, QWidget *scatter, QWidget *bars,
                              QWidget *buttonOne, QWidget *buttonTwo);
    ~ContainerChanger();

public slots:
    void changeContainer(int container);

private:
    QWidget *m_surface;
    QWidget *m_scatter;
    QWidget *m_bars;
    QWidget *m_button1;
    QWidget *m_button2;
};

#endif

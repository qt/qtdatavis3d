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

#ifndef GRAPHMODIFIER_H
#define GRAPHMODIFIER_H

#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/qabstract3dseries.h>

#include <QtGui/QFont>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QPropertyAnimation>

using namespace QtDataVisualization;

class GraphModifier : public QObject
{
    Q_OBJECT
public:
    explicit GraphModifier(Q3DBars *bargraph);
    ~GraphModifier();

    void resetTemperatureData();
    void changePresetCamera();
    void changeLabelBackground();
    void changeFont(const QFont &font);
    void changeFontSize(int fontsize);
    void rotateX(int rotation);
    void rotateY(int rotation);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void setSmoothBars(int smooth);
    void setSeriesVisibility(int enabled);
    void setReverseValueAxis(int enabled);
    void setReflection(bool enabled);

public slots:
    void changeRange(int range);
    void changeStyle(int style);
    void changeSelectionMode(int selectionMode);
    void changeTheme(int theme);
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);
    void changeLabelRotation(int rotation);
    void setAxisTitleVisibility(bool enabled);
    void setAxisTitleFixed(bool enabled);
    void zoomToSelectedBar();

signals:
    void shadowQualityChanged(int quality);
    void backgroundEnabledChanged(bool enabled);
    void gridEnabledChanged(bool enabled);
    void fontChanged(QFont font);
    void fontSizeChanged(int size);

private:
    Q3DBars *m_graph;
    float m_xRotation;
    float m_yRotation;
    int m_fontSize;
    int m_segments;
    int m_subSegments;
    float m_minval;
    float m_maxval;
    QStringList m_months;
    QStringList m_years;
    QValue3DAxis *m_temperatureAxis;
    QCategory3DAxis *m_yearAxis;
    QCategory3DAxis *m_monthAxis;
    QBar3DSeries *m_primarySeries;
    QBar3DSeries *m_secondarySeries;
    QAbstract3DSeries::Mesh m_barMesh;
    bool m_smooth;
    QPropertyAnimation m_animationCameraX;
    QPropertyAnimation m_animationCameraY;
    QPropertyAnimation m_animationCameraZoom;
    QPropertyAnimation m_animationCameraTarget;
    float m_defaultAngleX;
    float m_defaultAngleY;
    float m_defaultZoom;
    QVector3D m_defaultTarget;
};

#endif

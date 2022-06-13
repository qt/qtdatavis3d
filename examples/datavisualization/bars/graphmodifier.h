// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

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

public Q_SLOTS:
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

Q_SIGNALS:
    void shadowQualityChanged(int quality);
    void backgroundEnabledChanged(bool enabled);
    void gridEnabledChanged(bool enabled);
    void fontChanged(const QFont &font);
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

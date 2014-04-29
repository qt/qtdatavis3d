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

#ifndef CHARTMODIFIER_H
#define CHARTMODIFIER_H

#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/qabstract3dinputhandler.h>
#include <QtDataVisualization/qbar3dseries.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QFont>
#include <QDebug>
#include <QStringList>
#include <QPointer>
#include <QColorDialog>
#include <QTimer>
#include <QLabel>

using namespace QtDataVisualization;

class GraphModifier : public QObject
{
    Q_OBJECT
public:
    explicit GraphModifier(Q3DBars *barchart, QColorDialog *colorDialog);
    ~GraphModifier();

    void resetTemperatureData();
    void addRow();
    void addRows();
    void insertRow();
    void insertRows();
    void changeItem();
    void changeRow();
    void changeRows();
    void removeRow();
    void removeRows();
    void changeStyle();
    void changePresetCamera();
    void changeTheme();
    void changeLabelStyle();
    void changeSelectionMode();
    void changeFont(const QFont &font);
    void changeFontSize(int fontsize);
    void rotateX(int rotation);
    void rotateY(int rotation);
    void setFpsMeasurement(bool enable);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void setSpecsRatio(int barwidth);
    void setSpecsZ(int bardepth);
    void setSpacingSpecsX(int spacing);
    void setSpacingSpecsZ(int spacing);
    void setSampleCountX(int samples);
    void setSampleCountZ(int samples);
    void setMinX(int min);
    void setMinZ(int min);
    void setMinY(int min);
    void setMaxY(int max);
    void start();
    void restart(bool dynamicData);
    void selectBar();
    void swapAxis();
    void releaseAxes();
    void releaseSeries();
    void createMassiveArray();
    void useOwnTheme();
    void changeBaseColor(const QColor &color);
    void changeColorStyle();
    void showFiveSeries();
    QBarDataArray *makeDummyData();
    void primarySeriesTest();
    void insertRemoveTestToggle();
    void toggleRotation();
    void useLogAxis();
    void changeValueAxisFormat(const QString & text);
    void changeLogBase(const QString & text);
    void setFpsLabel(QLabel *fpsLabel) { m_fpsLabel = fpsLabel; }
    void addRemoveSeries();
    void testItemAndRowChanges();

public slots:
    void flipViews();
    void setGradient();
    void toggleMultiseriesScaling();
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);
    void handleSelectionChange(const QPoint &position);
    void setUseNullInputHandler(bool useNull);
    void changeValueAxisSegments(int value);

    void handleRowAxisChanged(QCategory3DAxis *axis);
    void handleColumnAxisChanged(QCategory3DAxis *axis);
    void handleValueAxisChanged(QValue3DAxis *axis);
    void handlePrimarySeriesChanged(QBar3DSeries *series);

    void insertRemoveTimerTimeout();
    void triggerSelection();
    void triggerRotation();
    void handleValueAxisLabelsChanged();
    void handleFpsChange(qreal fps);

signals:
    void shadowQualityChanged(int quality);

private:
    void populateFlatSeries(QBar3DSeries *series, int rows, int columns, float value);
    QBarDataRow *createFlatRow(int columns, float value);

    Q3DBars *m_graph;
    QColorDialog *m_colorDialog;
    int m_columnCount;
    int m_rowCount;
    float m_xRotation;
    float m_yRotation;
    bool m_static;
    float m_barSpacingX;
    float m_barSpacingZ;
    int m_fontSize;
    int m_segments;
    int m_subSegments;
    float m_minval;
    float m_maxval;
    QStringList m_months;
    QStringList m_years;
    QPoint m_selectedBar;
    QBar3DSeries *m_selectedSeries;
    QValue3DAxis *m_autoAdjustingAxis;
    QValue3DAxis *m_fixedRangeAxis;
    QValue3DAxis *m_temperatureAxis;
    QCategory3DAxis *m_yearAxis;
    QCategory3DAxis *m_monthAxis;
    QCategory3DAxis *m_genericRowAxis;
    QCategory3DAxis *m_genericColumnAxis;
    QBar3DSeries *m_temperatureData;
    QBar3DSeries *m_temperatureData2;
    QBar3DSeries *m_genericData;
    QBar3DSeries *m_dummyData;
    QBar3DSeries *m_dummyData2;
    QBar3DSeries *m_dummyData3;
    QBar3DSeries *m_dummyData4;
    QBar3DSeries *m_dummyData5;
    QValue3DAxis *m_currentAxis;
    bool m_negativeValuesOn;
    bool m_useNullInputHandler;
    QAbstract3DInputHandler *m_defaultInputHandler;
    Q3DTheme *m_ownTheme;
    Q3DTheme *m_builtinTheme;
    QTimer m_insertRemoveTimer;
    int m_insertRemoveStep;
    QAbstract3DInputHandler *m_customInputHandler;
    QTimer m_selectionTimer;
    QTimer m_rotationTimer;
    QLabel *m_fpsLabel;
    QBar3DSeries *m_extraSeries;
};

#endif

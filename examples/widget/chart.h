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

#include <QtDataVis3D/q3dbars.h>

#include <QFont>
#include <QDebug>
#include <QStringList>

using namespace QtDataVis3D;

class ChartModifier : public QObject
{
    Q_OBJECT
public:
    explicit ChartModifier(Q3DBars *barchart);
    ~ChartModifier();

    void addDataSet();
    void addRow();
    void addRows();
    void changeItem();
    void changeRow();
    void changeRows();
    void removeRow();
    void removeRows();
    void changeStyle();
    void changePresetCamera();
    void changeTheme();
    void changeTransparency();
    void changeSelectionMode();
    void changeFont(const QFont &font);
    void changeFontSize(int fontsize);
    void rotateX(int rotation);
    void rotateY(int rotation);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void setSpecsRatio(int barwidth);
    void setSpecsZ(int bardepth);
    void setSpacingSpecsX(int spacing);
    void setSpacingSpecsZ(int spacing);
    void setSampleCountX(int samples);
    void setSampleCountZ(int samples);
    void start();
    void restart(bool dynamicData);
    void selectBar();

public slots:
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QDataVis::ShadowQuality shadowQuality);
    void handleSelectionChange(const QPoint &position);

signals:
    void shadowQualityChanged(int quality);

private:
    Q3DBars *m_chart;
    int m_columnCount;
    int m_rowCount;
    float m_xRotation;
    float m_yRotation;
    bool m_static;
    qreal m_barThicknessRatio;
    float m_barSpacingX;
    float m_barSpacingZ;
    int m_fontSize;
    int m_segments;
    int m_subSegments;
    qreal m_minval;
    qreal m_maxval;
    QStringList m_genericRowLabels;
    QStringList m_genericColumnLabels;
    QPoint m_selectedBarPos;
};

#endif

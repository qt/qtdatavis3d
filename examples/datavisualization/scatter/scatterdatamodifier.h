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

#ifndef SCATTERDATAMODIFIER_H
#define SCATTERDATAMODIFIER_H

#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qabstract3dseries.h>
#include <QtGui/QFont>

using namespace QtDataVisualization;

class ScatterDataModifier : public QObject
{
    Q_OBJECT
public:
    explicit ScatterDataModifier(Q3DScatter *scatter);
    ~ScatterDataModifier();

    void addData();
    void changeStyle();
    void changePresetCamera();
    void changeLabelStyle();
    void changeFont(const QFont &font);
    void changeFontSize(int fontsize);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void setSmoothDots(int smooth);
    void toggleItemCount();
    void start();

public slots:
    void changeStyle(int style);
    void changeTheme(int theme);
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);

signals:
    void backgroundEnabledChanged(bool enabled);
    void gridEnabledChanged(bool enabled);
    void shadowQualityChanged(int quality);
    void fontChanged(QFont font);

private:
    QVector3D randVector();
    Q3DScatter *m_graph;
    int m_fontSize;
    QAbstract3DSeries::Mesh m_style;
    bool m_smooth;
    int m_itemCount;
    float m_curveDivider;
};

#endif

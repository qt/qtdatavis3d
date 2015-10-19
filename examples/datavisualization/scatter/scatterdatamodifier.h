/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

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

public Q_SLOTS:
    void changeStyle(int style);
    void changeTheme(int theme);
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);

Q_SIGNALS:
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

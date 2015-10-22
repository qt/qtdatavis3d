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
#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtCore/QTimer>

using namespace QtDataVisualization;

class ScatterDataModifier : public QObject
{
    Q_OBJECT
public:
    explicit ScatterDataModifier(Q3DScatter *scatter);
    ~ScatterDataModifier();

    void generateData();

public Q_SLOTS:
    void setFieldLines(int lines);
    void setArrowsPerLine(int arrows);
    void toggleRotation();
    void triggerRotation();
    void toggleSun();

private:
    Q3DScatter *m_graph;
    QTimer m_rotationTimer;
    int m_fieldLines;
    int m_arrowsPerLine;
    QScatter3DSeries *m_magneticField;
    QCustom3DItem *m_sun;
    QScatterDataArray *m_magneticFieldArray;
    float m_angleOffset;
    float m_angleStep;
};

#endif

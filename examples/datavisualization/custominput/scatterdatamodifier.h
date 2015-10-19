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

#include "custominputhandler.h"

#include <QtDataVisualization/q3dscatter.h>
#include <QtGui/QFont>
#include <QtCore/QTimer>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>
#include <QtGui/QVector3D>

using namespace QtDataVisualization;

class ScatterDataModifier : public QObject
{
    Q_OBJECT
public:
    explicit ScatterDataModifier(Q3DScatter *scatter);
    ~ScatterDataModifier();

    void addData();
    void toggleCameraAnimation();
    void start();

public Q_SLOTS:
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);
    void triggerSelection();

Q_SIGNALS:
    void shadowQualityChanged(int quality);

private:
    Q3DScatter *m_graph;
    QPropertyAnimation *m_animationCameraX;
    QSequentialAnimationGroup *m_animationCameraY;
    CustomInputHandler *m_inputHandler;
    QTimer *m_selectionTimer;
};

#endif

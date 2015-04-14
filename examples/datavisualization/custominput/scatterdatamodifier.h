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

public slots:
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality shadowQuality);
    void triggerSelection();

signals:
    void shadowQualityChanged(int quality);

private:
    Q3DScatter *m_graph;
    QPropertyAnimation *m_animationCameraX;
    QSequentialAnimationGroup *m_animationCameraY;
    CustomInputHandler *m_inputHandler;
    QTimer *m_selectionTimer;
};

#endif

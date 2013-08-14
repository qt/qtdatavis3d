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

#ifndef SCATTERDATAMODIFIER_H
#define SCATTERDATAMODIFIER_H

#include <QtDataVis3D/q3dscatter.h>

#include <QFont>
#include <QDebug>
#include <QTimer>

using namespace QtDataVis3D;

class ScatterDataModifier : public QObject
{
    Q_OBJECT
public:
    explicit ScatterDataModifier(Q3DScatter *scatter);
    ~ScatterDataModifier();

    void addData();
    void changeStyle();
    void changePresetCamera();
    void changeTheme();
    void changeTransparency();
    void changeFont(const QFont &font);
    void changeFontSize(int fontsize);
    void setBackgroundEnabled(int enabled);
    void setGridEnabled(int enabled);
    void start();

public slots:
    void changeShadowQuality(int quality);
    void shadowQualityUpdatedByVisual(ShadowQuality shadowQuality);
    void clear();
    void addOne();
    void addBunch();
    void insertOne();
    void insertBunch();
    void changeOne();
    void changeBunch();
    void removeOne();
    void removeBunch();
    void timeout();
    void startStopTimer();

signals:
    void shadowQualityChanged(int quality);

private:
    QVector3D randVector();
    Q3DScatter *m_chart;
    int m_fontSize;
    QTimer m_timer;
    int m_loopCounter;
};

#endif

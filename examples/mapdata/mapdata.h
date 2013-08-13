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

#include <QtDataVis3D/q3dmaps.h>

#include <QFont>
#include <QDebug>

using namespace QtDataVis3D;

class MapsModifier : public QObject
{
    Q_OBJECT
public:
    explicit MapsModifier(Q3DMaps *maps);
    ~MapsModifier();

    void addData();
    void changeStyle();
    void changePresetCamera();
    void changeTheme();
//    void changeSelectionMode();
    void changeTransparency();
    void changeFont(const QFont &font);
    void changeFontSize(int fontsize);
//    void rotateX(int rotation);
//    void rotateY(int rotation);
//    void setGridEnabled(int enabled);
//    void setSpecsX(int barwidth);
//    void setSpecsZ(int bardepth);
    void start();

public slots:
    void changeValueDimension(int dimension);
    void changeShadowQuality(int quality);

signals:
    void shadowQuality(int quality);

private:
    Q3DMaps *m_chart;
    QRect m_imageRect;
    int m_fontSize;
    QVector3D m_barSpecs;
};

#endif

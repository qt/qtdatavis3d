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

#ifndef TOPOGRAPHICSERIES_H
#define TOPOGRAPHICSERIES_H

#include <QtDataVisualization/QSurface3DSeries>

using namespace QtDataVisualization;

class TopographicSeries : public QSurface3DSeries
{
    Q_OBJECT
public:
    explicit TopographicSeries();
    ~TopographicSeries();

    void setTopographyFile(const QString file, float width, float height);

    float sampleCountX() { return m_sampleCountX; }
    float sampleCountZ() { return m_sampleCountZ; }

public slots:

private:
    float m_sampleCountX;
    float m_sampleCountZ;
};

#endif // TOPOGRAPHICSERIES_H

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

public Q_SLOTS:

private:
    float m_sampleCountX;
    float m_sampleCountZ;
};

#endif // TOPOGRAPHICSERIES_H

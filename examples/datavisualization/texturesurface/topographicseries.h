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

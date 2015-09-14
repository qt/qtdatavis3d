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

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QtDataVisualization/QSurface3DSeries>

using namespace QtDataVisualization;

class DataSource : public QObject
{
    Q_OBJECT
public:
    explicit DataSource(QObject *parent = 0);
    virtual ~DataSource();

    //! [0]
public Q_SLOTS:
    void generateData(int cacheCount, int rowCount, int columnCount,
                      float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);

    void update(QSurface3DSeries *series);
    //! [0]
private:
    void clearData();

    QVector<QSurfaceDataArray> m_data;
    int m_index;
    QSurfaceDataArray *m_resetArray;
};

#endif

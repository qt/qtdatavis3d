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

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QValue3DAxis>

class QQuickView;

using namespace QtDataVisualization;

class DataSource : public QObject
{
    Q_OBJECT
public:
    explicit DataSource(QObject *parent = 0);
    virtual ~DataSource();

    //! [0]
public slots:
    void generateData(int cacheCount, int rowCount, int columnCount,
                      float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);

    void update(QSurface3DSeries *series);

    QString selectionLabel(QSurface3DSeries *series, QValue3DAxis *axisX,
                           QValue3DAxis *axisY, QValue3DAxis *axisZ);
    //! [0]
private:
    void clearData();

    QVector<QSurfaceDataArray> m_data;
    int m_index;
    QSurfaceDataArray *m_resetArray;
};

#endif

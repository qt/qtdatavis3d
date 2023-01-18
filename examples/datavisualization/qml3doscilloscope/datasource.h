// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QtDataVisualization/QSurface3DSeries>
#include <QtQml/qqmlregistration.h>

//! [1]
class DataSource : public QObject
{
    Q_OBJECT
    QML_ELEMENT
//! [1]
public:
    explicit DataSource(QObject *parent = 0);
    virtual ~DataSource();

    //! [0]
    Q_INVOKABLE void generateData(int cacheCount, int rowCount, int columnCount,
                                  float xMin, float xMax, float yMin, float yMax,
                                  float zMin, float zMax);

    Q_INVOKABLE void update(QSurface3DSeries *series);
    //! [0]
private:
    void clearData();

    QList<QSurfaceDataArray> m_data;
    int m_index;
    QSurfaceDataArray *m_resetArray;
};

#endif

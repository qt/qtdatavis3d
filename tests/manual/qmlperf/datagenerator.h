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

#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QtDataVisualization/QScatter3DSeries>
#include <QtCore/QFile>

using namespace QtDataVisualization;

class DataGenerator  : public QObject
{
    Q_OBJECT
public:
    DataGenerator(QObject *parent = 0);
    virtual ~DataGenerator();

public Q_SLOTS:
    void generateData(QScatter3DSeries *series, uint count);
    void add(QScatter3DSeries *series, uint count);
    void writeLine(int itemCount, float fps);

private:
    QScatter3DSeries m_series;
    QFile *m_file;
};

#endif // DATAGENERATOR_H

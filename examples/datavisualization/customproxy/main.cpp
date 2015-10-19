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

#include "rainfallgraph.h"
#include <QtGui/QGuiApplication>

using namespace QtDataVisualization;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    Q3DBars *rainfall = new Q3DBars;
    rainfall->setFlags(rainfall->flags() ^ Qt::FramelessWindowHint);
    rainfall->resize(1000, 800);
    rainfall->setPosition(QPoint(10, 30));
    rainfall->show();

    RainfallGraph rainfallgraph(rainfall);
    rainfallgraph.start();

    return app.exec();
}

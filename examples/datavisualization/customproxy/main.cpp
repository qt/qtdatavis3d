/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

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

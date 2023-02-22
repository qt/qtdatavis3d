// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "rainfallgraph.h"
#include <QtGui/QGuiApplication>

int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", "opengl");
    QGuiApplication app(argc, argv);

    Q3DBarsNG *rainfall = new Q3DBarsNG;

    if (!rainfall->hasContext()) {
        qWarning() << QStringLiteral("Couldn't initialize the OpenGL context.") ;
        return -1;
    }

    rainfall->setFlags(rainfall->flags() ^ Qt::FramelessWindowHint);
    rainfall->resize(1000, 800);
    rainfall->setPosition(QPoint(10, 30));
    rainfall->show();

    RainfallGraph rainfallgraph(rainfall);
    rainfallgraph.start();

    return app.exec();
}

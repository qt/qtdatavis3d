// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "bargraph.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtabwidget.h>

int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", "opengl");

    QApplication app(argc, argv);

    // Create bar graph
    BarGraph *bars = new BarGraph();
    if (!bars->initialize())
        return -1;

    // Create a tab widget for creating own tabs for Bars3D, Scatter3D, and Surface3D
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->setWindowTitle(QLatin1String("Graph Gallery"));

    // Add bars widget
    tabWidget->addTab(bars->barsWidget(), QLatin1String("Bar Graph"));

    tabWidget->show();
    return app.exec();
}

// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "bargraph.h"
#include "scattergraph.h"
#include "surfacegraph.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtabwidget.h>

using namespace Qt::StringLiterals;

int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", "opengl");

    QApplication app(argc, argv);

    // Create bar graph
    BarGraph bars;
    if (!bars.initialize())
        return -1;

    // Create scatter graph
    ScatterGraph scatter;
    if (!scatter.initialize())
        return -1;

    // Create surface graph
    SurfaceGraph surface;
    if (!surface.initialize())
        return -1;

    // Create a tab widget for creating own tabs for Q3DBars, Q3DScatter, and Q3DSurface
    QTabWidget tabWidget;
    tabWidget.setWindowTitle(u"Graph Gallery"_s);

    // Add bars widget
    tabWidget.addTab(bars.barsWidget(), u"Bar Graph"_s);
    // Add scatter widget
    tabWidget.addTab(scatter.scatterWidget(), u"Scatter Graph"_s);
    // Add surface widget
    tabWidget.addTab(surface.surfaceWidget(), u"Surface Graph"_s);

    tabWidget.show();
    return app.exec();
}

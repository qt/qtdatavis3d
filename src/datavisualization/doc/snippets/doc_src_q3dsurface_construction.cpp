/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

//! [5]
#include <QtDataVisualization>

using namespace QtDataVisualization;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    //! [0]
    Q3DSurface surface;
    //! [0]
    //! [1]
    QSurfaceDataArray *data = new QSurfaceDataArray;
    QSurfaceDataRow *dataRow = new QSurfaceDataRow;
    //! [1]

    //! [2]
    *dataRow << 0.1 << 1.8 << 0.4;
    *data << dataRow;
    //! [2]

    //! [3]
    surface.activeDataProxy()->resetArray(data);
    //! [3]
    //! [4]
    surface.show();
    //! [4]

    return app.exec();
}
//! [5]

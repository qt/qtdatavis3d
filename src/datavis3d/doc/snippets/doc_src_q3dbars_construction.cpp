/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

//! [3]
#include <QtDataVis3D>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    //! [0]
    Q3DBars bars;
    bars.setupSampleSpace(5, 5);
    //! [0]
    //! [1]
    QVector<float> data;
    data << 1.0f << 3.0f << 7.5f << 5.0f << 2.2f;
    bars.addDataRow(data);
    //! [1]
    //! [2]
    bars.show();
    //! [2]

    return app.exec();
}
//! [3]

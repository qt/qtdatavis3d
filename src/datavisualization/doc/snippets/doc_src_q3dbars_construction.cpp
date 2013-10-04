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

//! [3]
#include <QtDataVisualization>

using namespace QtDataVisualization;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    //! [4]
    Q3DBars *bars = new Q3DBars();
    //! [4]
    //! [0]
    bars->rowAxis()->setRange(0, 4);
    bars->columnAxis()->setRange(0, 4);
    //! [0]
    //! [1]
    QBarDataRow data;
    data << 1.0 << 3.0 << 7.5 << 5.0 << 2.2;
    bars->activeDataProxy()->addRow(&data);
    //! [1]
    //! [2]
    bars->show();
    //! [2]

    return app.exec();
}
//! [3]

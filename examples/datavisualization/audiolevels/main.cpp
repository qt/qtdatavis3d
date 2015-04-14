/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include "audiolevels.h"

#include <QtGui/QGuiApplication>
#include <QtMultimedia/QAudio>

using namespace QtDataVisualization;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Q3DBars window;
    window.setFlags(window.flags() ^ Qt::FramelessWindowHint);
    window.resize(800, 500);
    window.setTitle("Qt Data Visualization - Microphone audio levels visualizer");
    window.show();

    AudioLevels audioLevels(&window);

    return app.exec();
}

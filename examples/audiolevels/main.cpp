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

#include "audiolevels.h"

#include <QGuiApplication>
#include <QAudio>

using namespace QtDataVisualization;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Q3DBars window;
    window.resize(1024, 768);
    window.setTitle("Qt Data Visualization - Microphone audio levels visualizer");
    window.show();

    AudioLevels audioLevels(&window);

    return app.exec();
}

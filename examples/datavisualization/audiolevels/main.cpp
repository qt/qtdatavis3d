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

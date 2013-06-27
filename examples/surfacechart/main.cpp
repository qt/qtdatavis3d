/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <Q3DSurface>

#include <QGuiApplication>

using namespace QtDataVis3D;

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    Q3DSurface surfaceChart;

    QList<qreal> lowList;
    lowList << 15.0 << 65.0 << 105.0 << 65.0 << 15.0;
    lowList << 35.0 << 105.0 << 170.0 << 105.0 << 35;
    lowList << 55.0 << 135.0 << 215.0 << 135.0 << 55;
    lowList << 75.0 << 155.0 << 240.0 << 155.0 << 75;
    lowList << 80.0 << 190.0 << 245.0 << 190.0 << 80;
    lowList << 75.0 << 155.0 << 240.0 << 155.0 << 75.0;
    lowList << 55.0 << 135.0 << 215.0 << 135.0 << 55;
    lowList << 35.0 << 105.0 << 170.0 << 105.0 << 35.0;
    lowList << 15.0 << 65.0 << 105.0 << 65.0 << 16.1;

    surfaceChart.appendSeries(lowList);

//    QList<qreal> topList;
//    topList << 2.1 << 2.2;
//    surfaceChart.appendSeries(topList);

    surfaceChart.showData();
    surfaceChart.show();

    return a.exec();
}

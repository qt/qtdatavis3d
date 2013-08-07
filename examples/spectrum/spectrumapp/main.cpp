/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

#include "q3dbars.h"
#include "engine.h"
#include "utils.h"
#include "qbardataproxy.h"
#include "qvalueaxis.h"

#include <QGuiApplication>
#include <QAudio>
#include <QTimer>

//#define USE_CONES

using namespace QtDataVis3D;

class MainApp : public QObject
{
public:
    MainApp(Q3DBars *window);
    ~MainApp();

    void start(QString fileName);

public slots:
    void spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum);
    void stateChanged(QAudio::Mode mode, QAudio::State state);

private slots:
    void restart();

private:
    int barIndex(qreal frequency) const;

private:
    Q3DBars                 *m_chart;
    Engine                  *m_engine;
    QTimer                  *m_restartTimer;
    // Lower bound of first band in the spectrum in Hz
    qreal                   m_lowFreq;
    // Upper band of last band in the spectrum in Hz
    qreal                   m_highFreq;
};

MainApp::MainApp(Q3DBars *window)
    : m_chart(window),
      m_engine(new Engine(this)),
      m_restartTimer(new QTimer(this)),
      m_lowFreq(SpectrumLowFreq),
      m_highFreq(SpectrumHighFreq)
{
    m_chart->setupSampleSpace(SpectrumNumBands * 2, SpectrumNumBands);
    // Disable grid
    m_chart->setGridVisible(false);
    // Disable auto-scaling of height by defining explicit range
    // By setting count to 0 we avoid getting any grid
    m_chart->valueAxis()->setTickCount(0);
    m_chart->valueAxis()->setRange(0.0, 1.0);
    // Disable shadows
    m_chart->setShadowQuality(ShadowNone);
#if USE_CONES
    // Set bar specifications; make them a bit wider than deep and make them be drawn 75%
    // inside each other
    m_chart->setBarSpecs(QSizeF(1.0f, 0.75f), QSizeF(0.2f, -0.75f));
    // Set bar type, smooth cones
    m_chart->setBarType(Cones, true);
    // Adjust zoom manually; automatic zoom level calculation does not work well with negative
    // spacings (in setBarSpecs)
    m_chart->setCameraPosition(10.0f, 5.0f, 70);
#else
    // Set bar specifications; make them twice as wide as they're deep
    m_chart->setBarSpecs(QSizeF(1.0f, 0.5f), QSizeF(0.0f, 0.0f));
    // Set bar type, flat bars
    m_chart->setBarType(Bars, false);
    // Adjust camera position
    m_chart->setCameraPosition(10.0f, 7.5f, 75);
#endif
    // Set color scheme
    m_chart->setBarColor(QColor(Qt::black), QColor(Qt::red), QColor(Qt::darkYellow));
    // Disable selection
    m_chart->setSelectionMode(ModeNone);
    QObject::connect(m_engine, &Engine::changedSpectrum, this, &MainApp::spectrumChanged);
    QObject::connect(m_engine, &Engine::stateChanged, this, &MainApp::stateChanged);
    m_restartTimer->setSingleShot(true);
    QObject::connect(m_restartTimer, &QTimer::timeout, this, &MainApp::restart);

    QBarDataProxy *proxy = new QBarDataProxy;
    m_chart->setDataProxy(proxy);
}

MainApp::~MainApp()
{
    delete m_engine;
    delete m_restartTimer;
}

void MainApp::start(QString fileName)
{
    m_engine->loadFile(fileName);
    m_engine->startPlayback();
}

//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void MainApp::spectrumChanged(qint64 position, qint64 length, const FrequencySpectrum &spectrum)
{
    Q_UNUSED(position);
    Q_UNUSED(length);
    //qDebug() << "updating bar values" << position << length;
    QBarDataRow *data = new QBarDataRow(SpectrumNumBands);
    for (int bar = 0; bar < SpectrumNumBands; bar++) {
        // init data set
        (*data)[bar].setValue(qreal(0.0));
    }
    FrequencySpectrum::const_iterator i = spectrum.begin();
    const FrequencySpectrum::const_iterator end = spectrum.end();
    for ( ; i != end; ++i) {
        const FrequencySpectrum::Element e = *i;
        if (e.frequency >= m_lowFreq && e.frequency < m_highFreq) {
            (*data)[barIndex(e.frequency)].setValue(qMax(data->at(barIndex(e.frequency)).value(), qreal(e.amplitude)));
        }
    }
    static_cast<QBarDataProxy *>(m_chart->dataProxy())->insertRow(0, data);
}

void MainApp::stateChanged(QAudio::Mode mode, QAudio::State state)
{
    //qDebug() << "mode:" << mode << " state: " << state;
    // Restart once playback is finished
    if (QAudio::AudioOutput == mode && QAudio::StoppedState == state)
        m_restartTimer->start(500);
}

//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void MainApp::restart()
{
    // Change file each time
    QString fileToLoad = QStringLiteral(":/file");
    static int fileNo = 3;
    QString nrStr;
    nrStr.setNum(fileNo);
    fileToLoad.append(nrStr);
    //qDebug() << fileToLoad;
    start(fileToLoad);
    fileNo++;
    if (fileNo > 3)
        fileNo = 1;
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

int MainApp::barIndex(qreal frequency) const
{
    Q_ASSERT(frequency >= m_lowFreq && frequency < m_highFreq);
    const qreal bandWidth = (m_highFreq - m_lowFreq) / SpectrumNumBands;
    const int index = (frequency - m_lowFreq) / bandWidth;
    if (index < 0 || index >= SpectrumNumBands)
        Q_ASSERT(false);
    //qDebug() << "insert to" << index;
    return index;
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("QtDataVis3D spectrum analyzer");

    Q3DBars window;
    window.resize(1024, 768);
    window.show();

    MainApp *mainApp = new MainApp(&window);
    mainApp->start(QStringLiteral(":/file2"));

    return app.exec();
}

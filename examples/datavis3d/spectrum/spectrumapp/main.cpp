/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include <QGuiApplication>
#include <QAudio>
#include <QTimer>

using namespace QtDataVis3D;

class MainApp : public QObject
{
public:
    MainApp(Q3DBars *window);
    ~MainApp();

    void start(QString fileName);

public slots:
    void spectrumChanged(qint64 position, qint64 length,
                         const FrequencySpectrum &spectrum);
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
    : m_chart(window)
    , m_engine(new Engine(this))
    , m_restartTimer(new QTimer(this))
    , m_lowFreq(SpectrumLowFreq)
    , m_highFreq(SpectrumHighFreq)
{
    m_chart->setupSampleSpace(QPoint(SpectrumNumBands, SpectrumNumBands*2));
    m_chart->setBarSpecs(QPointF(1.0f, 0.75f), QPointF(0.2f, 0.1f));
    m_chart->setBarType(Q3DBars::Bars, false);
    QObject::connect(m_engine, &Engine::changedSpectrum, this, &MainApp::spectrumChanged);
    QObject::connect(m_engine, &Engine::stateChanged, this, &MainApp::stateChanged);
    m_restartTimer->setSingleShot(true);
    QObject::connect(m_restartTimer, &QTimer::timeout, this, &MainApp::restart);
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
    QVector<float> data;
    for (int bar = 0; bar < SpectrumNumBands; bar++) {
        // init data set
        data.append(0.0f);
    }
    FrequencySpectrum::const_iterator i = spectrum.begin();
    const FrequencySpectrum::const_iterator end = spectrum.end();
    for ( ; i != end; ++i) {
        const FrequencySpectrum::Element e = *i;
        if (e.frequency >= m_lowFreq && e.frequency < m_highFreq) {
            data.replace(barIndex(e.frequency)
                         , qMax(data.at(barIndex(e.frequency)), (float)e.amplitude));
        }
    }
    if (data.size() > 0)
        m_chart->addDataSet(data);
}

void MainApp::stateChanged(QAudio::Mode mode, QAudio::State state)
{
    qDebug() << "mode:" << mode << " state: " << state;
    // Restart once playback is finished
    if (QAudio::AudioOutput == mode && QAudio::StoppedState == state) {
        m_restartTimer->start(500);
    }
}

//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void MainApp::restart()
{
    // Change file each time
    QString fileToLoad = QStringLiteral(":/file");
    static int fileNo = 1;
    QString nrStr;
    nrStr.setNum(fileNo);
    fileToLoad.append(nrStr);
    qDebug() << fileToLoad;
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
    mainApp->start(QStringLiteral(":/file3"));

    return app.exec();
}

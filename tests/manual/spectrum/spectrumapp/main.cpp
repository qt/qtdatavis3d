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

#include "engine.h"
#include "utils.h"

#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/qcategory3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qbar3dseries.h>
#include <QtDataVisualization/q3dtheme.h>

#include <QGuiApplication>
#include <QAudio>
#include <QTimer>

//#define USE_CONES

using namespace QtDataVisualization;

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
    m_chart->rowAxis()->setMax(SpectrumNumBands * 2);
    m_chart->columnAxis()->setMax(SpectrumNumBands - 1);
    // Disable grid
    m_chart->activeTheme()->setGridEnabled(false);
    // Disable auto-scaling of height by defining explicit range
    m_chart->valueAxis()->setRange(0.0f, 1.0f);
    // Disable shadows
    m_chart->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
#if USE_CONES
    // Set bar specifications; make them a bit wider than deep and make them be drawn 75%
    // inside each other
    m_chart->setBarThickness(1.25);
    m_chart->setBarSpacing(QSizeF(0.2, -0.75));
    // Set bar type, smooth cones
    m_chart->setBarType(QAbstract3DGraph::MeshCones, true);
    // Adjust zoom manually; automatic zoom level calculation does not work well with negative
    // spacings (in setBarSpacing)
    m_chart->setCameraPosition(10.0f, 5.0f, 70.0f);
#else
    // Set bar specifications; make them twice as wide as they're deep
    m_chart->setBarThickness(2.0f);
    m_chart->setBarSpacing(QSizeF(0.0, 0.0));
    // Set bar type, flat bars
    // Adjust camera position
    m_chart->scene()->activeCamera()->setCameraPosition(10.0f, 7.5f, 75.0f);
#endif
    // Disable selection
    m_chart->setSelectionMode(QAbstract3DGraph::SelectionNone);
    QObject::connect(m_engine, &Engine::changedSpectrum, this, &MainApp::spectrumChanged);
    QObject::connect(m_engine, &Engine::stateChanged, this, &MainApp::stateChanged);
    m_restartTimer->setSingleShot(true);
    QObject::connect(m_restartTimer, &QTimer::timeout, this, &MainApp::restart);

    QBar3DSeries *series = new QBar3DSeries();
    series->setBaseColor(QColor(Qt::red));
    series->setMesh(QAbstract3DSeries::MeshBar);
    m_chart->addSeries(series);
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
            (*data)[barIndex(e.frequency)].setValue(qMax(data->at(barIndex(e.frequency)).value(), float(e.amplitude)));
        }
    }
    m_chart->seriesList().at(0)->dataProxy()->insertRow(0, data);
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
    app.setApplicationName("QtDataVisualization spectrum analyzer");

    Q3DBars window;
    window.setFlags(window.flags() ^ Qt::FramelessWindowHint);
    window.resize(1024, 768);
    window.show();

    MainApp *mainApp = new MainApp(&window);
    mainApp->start(QStringLiteral(":/file2"));

    return app.exec();
}
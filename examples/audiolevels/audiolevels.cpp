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

#include "audiolevelsiodevice.h"
#include "audiolevels.h"

#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/q3dvalueaxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>

#include <QAudioDeviceInfo>
#include <QAudioInput>

QT_DATAVISUALIZATION_USE_NAMESPACE

AudioLevels::AudioLevels(Q3DBars *graph, QObject *parent)
    : QObject(parent),
      m_graph(graph),
      m_device(0),
      m_audioInput(0)
{
    // Set up the graph
    m_graph->setBarThickness(0.5);
    m_graph->setBarSpacing(QSizeF(0.0, 1.0));
    m_graph->setGridVisible(false);
    m_graph->setBackgroundVisible(false);
    m_graph->valueAxis()->setRange(0.0, 2.0);
    m_graph->setShadowQuality(QDataVis::ShadowQualityNone);
    m_graph->scene()->activeCamera()->setCameraPosition(-20.0, 10.0, 20);
    m_graph->setTheme(QDataVis::ThemeIsabelle);
    m_graph->setBarType(QDataVis::MeshStyleBars);

    //! [0]
    QAudioFormat formatAudio;
    formatAudio.setSampleRate(8000);
    formatAudio.setChannelCount(1);
    formatAudio.setSampleSize(8);
    formatAudio.setCodec("audio/pcm");
    formatAudio.setByteOrder(QAudioFormat::LittleEndian);
    formatAudio.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo inputDevices = QAudioDeviceInfo::defaultInputDevice();
    m_audioInput = new QAudioInput(inputDevices, formatAudio, this);

    m_device = new AudioLevelsIODevice(m_graph->activeDataProxy(), this);
    m_device->open(QIODevice::WriteOnly);

    m_audioInput->start(m_device);
    //! [0]
}

AudioLevels::~AudioLevels()
{
    m_audioInput->stop();
    m_device->close();
}

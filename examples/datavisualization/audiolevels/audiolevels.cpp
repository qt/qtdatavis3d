/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qbar3dseries.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtDataVisualization/qcustom3dlabel.h>

#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>

using namespace QtDataVisualization;

AudioLevels::AudioLevels(Q3DBars *graph, QObject *parent)
    : QObject(parent),
      m_graph(graph),
      m_device(0),
      m_audioInput(0)
{
    // Set up the graph
    m_graph->setBarThickness(0.5f);
    m_graph->setBarSpacing(QSizeF(0.0, 1.0));
    m_graph->valueAxis()->setRange(-100.0f, 100.0f);
    m_graph->valueAxis()->setSegmentCount(20);
    m_graph->valueAxis()->setLabelFormat(QStringLiteral("%d%%"));
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    m_graph->setSelectionMode(QAbstract3DGraph::SelectionNone);
    m_graph->scene()->activeCamera()->setCameraPosition(-25.0f, 10.0f, 190.0f);
    m_graph->activeTheme()->setType(Q3DTheme::ThemeIsabelle);
    m_graph->activeTheme()->setGridEnabled(true);
    m_graph->activeTheme()->setBackgroundEnabled(false);
    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(10);
    m_graph->activeTheme()->setFont(font);
    QBar3DSeries *series = new QBar3DSeries;
    series->setMesh(QAbstract3DSeries::MeshBar);
    m_graph->addSeries(series);

    //! [0]
    QAudioDeviceInfo inputDevice = QAudioDeviceInfo::defaultInputDevice();

    if (inputDevice.supportedSampleRates().size() > 0
            && inputDevice.supportedChannelCounts().size() > 0
            && inputDevice.supportedSampleSizes().size() > 0
            && inputDevice.supportedCodecs().size() > 0) {
        QAudioFormat formatAudio;
        formatAudio.setSampleRate(inputDevice.supportedSampleRates().at(0));
        formatAudio.setChannelCount(inputDevice.supportedChannelCounts().at(0));
        formatAudio.setSampleSize(inputDevice.supportedSampleSizes().at(0));
        formatAudio.setCodec(inputDevice.supportedCodecs().at(0));
        formatAudio.setByteOrder(QAudioFormat::LittleEndian);
        formatAudio.setSampleType(QAudioFormat::UnSignedInt);

        m_audioInput = new QAudioInput(inputDevice, formatAudio, this);
#ifdef Q_OS_MAC
        // OS X seems to wait for entire buffer to fill before calling writeData, so use smaller buffer
        m_audioInput->setBufferSize(256);
#else
        m_audioInput->setBufferSize(1024);
#endif

        m_device = new AudioLevelsIODevice(m_graph->seriesList().at(0)->dataProxy(), this);
        m_device->open(QIODevice::WriteOnly);

        m_audioInput->start(m_device);
    } else {
        // No graph content can be shown, so add a custom warning label
        QCustom3DLabel *titleLabel = new QCustom3DLabel("No valid audio input device found",
                                                        QFont(),
                                                        QVector3D(0.2f, 0.2f, 0.0f),
                                                        QVector3D(1.0f, 1.0f, 0.0f),
                                                        QQuaternion());
        titleLabel->setPositionAbsolute(true);
        titleLabel->setFacingCamera(true);
        m_graph->addCustomItem(titleLabel);
    }
    //! [0]
}

AudioLevels::~AudioLevels()
{
    if (m_audioInput)
        m_audioInput->stop();
    if (m_device)
        m_device->close();
}

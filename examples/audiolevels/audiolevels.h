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

#ifndef AUDIOLEVELS_H
#define AUDIOLEVELS_H

#include <QtDataVisualization/q3dbars.h>

QT_DATAVISUALIZATION_USE_NAMESPACE

class AudioLevelsIODevice;
class QAudioInput;

class AudioLevels : public QObject
{
    Q_OBJECT

public:
    AudioLevels(Q3DBars *graph, QObject *parent = 0);
    ~AudioLevels();

private:
    AudioLevelsIODevice *m_device;
    Q3DBars *m_graph;
    QAudioInput *m_audioInput;
};

#endif

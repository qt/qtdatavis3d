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

#ifndef AUDIOLEVELS_H
#define AUDIOLEVELS_H

#include <QtDataVisualization/q3dbars.h>

using namespace QtDataVisualization;

class AudioLevelsIODevice;

QT_BEGIN_NAMESPACE
class QAudioInput;
QT_END_NAMESPACE

class AudioLevels : public QObject
{
    Q_OBJECT

public:
    AudioLevels(Q3DBars *graph, QObject *parent = 0);
    ~AudioLevels();

private:
    //! [0]
    Q3DBars *m_graph;
    AudioLevelsIODevice *m_device;
    QAudioInput *m_audioInput;
    //! [0]
};

#endif

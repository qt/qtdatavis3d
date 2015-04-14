/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef AUDIOLEVELS_H
#define AUDIOLEVELS_H

#include <QtDataVisualization/q3dbars.h>

using namespace QtDataVisualization;

class AudioLevelsIODevice;
class QAudioInput;

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

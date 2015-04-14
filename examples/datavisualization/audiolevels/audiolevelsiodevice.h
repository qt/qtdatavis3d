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

#ifndef AUDIOLEVELSIODEVICE_H
#define AUDIOLEVELSIODEVICE_H

#include <QtDataVisualization/qbardataproxy.h>
#include <QtCore/QIODevice>

using namespace QtDataVisualization;

class AudioLevelsIODevice : public QIODevice
{
    Q_OBJECT
public:
    explicit AudioLevelsIODevice(QBarDataProxy *proxy, QObject *parent = 0);

protected:
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

private:
    //! [0]
    QBarDataProxy *m_proxy;
    QBarDataArray *m_array;
    //! [0]
};

#endif

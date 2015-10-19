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

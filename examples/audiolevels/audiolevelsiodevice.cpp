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
#include <QDebug>

QT_DATAVISUALIZATION_USE_NAMESPACE

AudioLevelsIODevice::AudioLevelsIODevice(QBarDataProxy *proxy, QObject *parent)
    : QIODevice(parent),
      m_proxy(proxy)
{
}

// Implementation required for this pure virtual function
qint64 AudioLevelsIODevice::readData(char *data, qint64 maxSize)
{
    Q_UNUSED(data)
    Q_UNUSED(maxSize)
    return -1;
}

qint64 AudioLevelsIODevice::writeData(const char *data, qint64 maxSize)
{
    static const int resolution = 8;
    static const int maxRowSize = 1000;

    int newDataSize = maxSize / resolution;

    const QBarDataRow *oldRow = m_proxy->rowAt(0);

    int rowSize = qMin((newDataSize + oldRow->size()), maxRowSize);

    QBarDataRow *row = new QBarDataRow(rowSize);
    int bottom = qMax(0, (newDataSize - rowSize));

    // Insert data in reverse order, so that newest data is always at the front of the row
    int index = 0;
    for (int i = newDataSize - 1; i >= bottom; i--)
        (*row)[index++].setValue(((quint8)data[resolution * i] - 128) / 2.0);

    // Append old data to new row
    for (int i = newDataSize; i < rowSize; i++)
        (*row)[i].setValue(oldRow->at(i - newDataSize).value());

    m_proxy->setRow(0, row);

    return maxSize;
}



// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "datagenerator.h"
#include <QDebug>
#include <QRandomGenerator>

Q_DECLARE_METATYPE(QScatter3DSeries *)

DataGenerator::DataGenerator(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<QScatter3DSeries *>();

    m_file = new QFile("results.txt");
    if (!m_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        delete m_file;
        m_file = 0;
    }
}

DataGenerator::~DataGenerator()
{
    m_file->close();
    delete m_file;
}

void DataGenerator::generateData(QScatter3DSeries *series, uint count)
{
    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(count);
    QScatterDataItem *ptrToDataArray = &dataArray->first();

    for (uint i = 0; i < count; i++) {
            ptrToDataArray->setPosition(QVector3D(QRandomGenerator::global()->generateDouble(),
                                                  QRandomGenerator::global()->generateDouble(),
                                                  QRandomGenerator::global()->generateDouble()));
            ptrToDataArray++;
    }

    series->dataProxy()->resetArray(dataArray);
}

void DataGenerator::add(QScatter3DSeries *series, uint count)
{
    QScatterDataArray appendArray;
    appendArray.resize(count);

    for (uint i = 0; i < count; i++) {
            appendArray[i].setPosition(QVector3D(QRandomGenerator::global()->generateDouble(),
                                                 QRandomGenerator::global()->generateDouble(),
                                                 QRandomGenerator::global()->generateDouble()));
    }

    series->dataProxy()->addItems(appendArray);
}

void DataGenerator::writeLine(int itemCount, float fps)
{
    if (m_file) {
        QTextStream out(m_file);

        QString fpsFormatString(QStringLiteral("%1   %2\n"));
        QString fpsString = fpsFormatString.arg(itemCount).arg(fps);

        out << fpsString;
    }
}

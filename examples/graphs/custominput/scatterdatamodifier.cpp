// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "scatterdatamodifier.h"

#include <QtGraphs/QScatterDataProxy>
#include <QtGraphs/QValue3DAxis>
#include <QtGraphs/Q3DScene>
#include <QtGraphs/Q3DCamera>
#include <QtGraphs/QScatter3DSeries>
#include <QtGraphs/Q3DTheme>
#include <QtCore/qmath.h>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

ScatterDataModifier::ScatterDataModifier(Q3DScatter *scatter)
    : m_graph(scatter),
      m_inputHandler(new CustomInputHandler())
{
    m_graph->activeTheme()->setType(Q3DTheme::ThemeDigia);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityMedium);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    m_graph->axisX()->setRange(-10.0f, 10.0f);
    m_graph->axisY()->setRange(-5.0f, 5.0f);
    m_graph->axisZ()->setRange(-5.0f, 5.0f);

    QScatter3DSeries *series = new QScatter3DSeries;
    series->setItemLabelFormat(QStringLiteral("@xLabel, @yLabel, @zLabel"));
    series->setMesh(QAbstract3DSeries::MeshCube);
    series->setItemSize(0.15f);
    m_graph->addSeries(series);

    //! [2]
    m_animationCameraX = new QPropertyAnimation(m_graph->scene()->activeCamera(), "xRotation");
    m_animationCameraX->setDuration(20000);
    m_animationCameraX->setStartValue(QVariant::fromValue(0.0f));
    m_animationCameraX->setEndValue(QVariant::fromValue(360.0f));
    m_animationCameraX->setLoopCount(-1);
    //! [2]

    //! [3]
    QPropertyAnimation *upAnimation = new QPropertyAnimation(m_graph->scene()->activeCamera(), "yRotation");
    upAnimation->setDuration(9000);
    upAnimation->setStartValue(QVariant::fromValue(5.0f));
    upAnimation->setEndValue(QVariant::fromValue(45.0f));

    QPropertyAnimation *downAnimation = new QPropertyAnimation(m_graph->scene()->activeCamera(), "yRotation");
    downAnimation->setDuration(9000);
    downAnimation->setStartValue(QVariant::fromValue(45.0f));
    downAnimation->setEndValue(QVariant::fromValue(5.0f));

    m_animationCameraY = new QSequentialAnimationGroup();
    m_animationCameraY->setLoopCount(-1);
    m_animationCameraY->addAnimation(upAnimation);
    m_animationCameraY->addAnimation(downAnimation);
    //! [3]

    m_animationCameraX->start();
    m_animationCameraY->start();

    // Give ownership of the handler to the graph and make it the active handler
    //! [0]
    m_graph->setActiveInputHandler(m_inputHandler);
    //! [0]

    //! [1]
    m_selectionTimer = new QTimer(this);
    m_selectionTimer->setInterval(10);
    m_selectionTimer->setSingleShot(false);
    QObject::connect(m_selectionTimer, &QTimer::timeout, this,
                     &ScatterDataModifier::triggerSelection);
    m_selectionTimer->start();
    //! [1]
}

ScatterDataModifier::~ScatterDataModifier()
{
    delete m_graph;
}

void ScatterDataModifier::start()
{
    addData();
}

void ScatterDataModifier::addData()
{
    QList<QVector3D> itemList;

    // Read data items from the file to QList
    QTextStream stream;
    QFile dataFile(":/data/data.txt");
    if (dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        stream.setDevice(&dataFile);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.startsWith("#")) // Ignore comments
                continue;
            QStringList strList = line.split(",", Qt::SkipEmptyParts);
            // Each line has three data items: xPos, yPos and zPos value
            if (strList.size() < 3) {
                qWarning() << "Invalid row read from data:" << line;
                continue;
            }
            itemList.append(QVector3D(
                                 strList.at(0).trimmed().toFloat(),
                                 strList.at(1).trimmed().toFloat(),
                                 strList.at(2).trimmed().toFloat()));
        }
    } else {
        qWarning() << "Unable to open data file:" << dataFile.fileName();
    }

    // Add data from the QList to datamodel
    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(itemList.size());
    QScatterDataItem *ptrToDataArray = &dataArray->first();
    for (int i = 0; i < itemList.size(); i++) {
        ptrToDataArray->setPosition(itemList.at(i));
        ptrToDataArray++;
    }

    m_graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
}

void ScatterDataModifier::toggleCameraAnimation()
{
    if (m_animationCameraX->state() != QAbstractAnimation::Paused) {
        m_animationCameraX->pause();
        m_animationCameraY->pause();
    } else {
        m_animationCameraX->resume();
        m_animationCameraY->resume();
    }
}

void ScatterDataModifier::triggerSelection()
{
    m_graph->scene()->setSelectionQueryPosition(m_inputHandler->inputPosition());
}

void ScatterDataModifier::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq)
{
    int quality = int(sq);
    emit shadowQualityChanged(quality); // connected to a checkbox in main.cpp
}

void ScatterDataModifier::changeShadowQuality(int quality)
{
    QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
}

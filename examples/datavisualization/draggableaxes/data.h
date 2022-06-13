// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef DATA_H
#define DATA_H

#include "axesinputhandler.h"

#include <QtDataVisualization/q3dscatter.h>
#include <QtGui/QVector3D>

class Data : public QObject
{
    Q_OBJECT
public:
    explicit Data(Q3DScatter *scatter);
    ~Data();

    void toggleRanges();

private:
    void addData();
    QVector3D randVector();

private:
    Q3DScatter *m_graph;
    AxesInputHandler *m_inputHandler;
    bool m_autoAdjust;
};

#endif

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

#ifndef DATA_H
#define DATA_H

#include "axesinputhandler.h"

#include <QtDataVisualization/q3dscatter.h>
#include <QtGui/QVector3D>

using namespace QtDataVisualization;

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

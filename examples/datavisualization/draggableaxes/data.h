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

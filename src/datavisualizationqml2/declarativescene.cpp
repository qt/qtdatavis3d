/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
******************************************************************************/

#include "declarativescene_p.h"

QT_BEGIN_NAMESPACE

Declarative3DScene::Declarative3DScene(QObject *parent)
    : Q3DScene(parent)
{
    QObject::connect(this, &Q3DScene::selectionQueryPositionChanged, this,
                     &Declarative3DScene::selectionQueryPositionChanged);
}

Declarative3DScene::~Declarative3DScene()
{
}

void Declarative3DScene::setSelectionQueryPosition(const QPointF &point)
{
    Q3DScene::setSelectionQueryPosition(point.toPoint());
}

QPointF Declarative3DScene::selectionQueryPosition() const
{
    return QPointF(Q3DScene::selectionQueryPosition());
}

QPoint Declarative3DScene::invalidSelectionPoint() const
{
    return Q3DScene::invalidSelectionPoint();
}

QT_END_NAMESPACE

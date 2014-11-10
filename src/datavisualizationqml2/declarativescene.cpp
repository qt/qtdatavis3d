/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include "declarativescene_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

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

QT_END_NAMESPACE_DATAVISUALIZATION

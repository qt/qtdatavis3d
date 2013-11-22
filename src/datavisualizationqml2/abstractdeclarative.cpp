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

#include "abstractdeclarative_p.h"
#include "q3dvalueaxis.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

AbstractDeclarative::AbstractDeclarative(QQuickItem *parent) :
    QQuickItem(parent)
{
}

AbstractDeclarative::~AbstractDeclarative()
{
}

Q3DScene* AbstractDeclarative::scene() const
{
    return m_controller->scene();
}

void AbstractDeclarative::setTheme(Q3DTheme *theme)
{
    m_controller->setTheme(theme);
}

Q3DTheme *AbstractDeclarative::theme() const
{
    return m_controller->theme();
}

void AbstractDeclarative::setSelectionMode(QDataVis::SelectionFlags mode)
{
    m_controller->setSelectionMode(mode);
}

QDataVis::SelectionFlags AbstractDeclarative::selectionMode() const
{
    return m_controller->selectionMode();
}

void AbstractDeclarative::setFont(const QFont &font)
{
    m_controller->setFont(font);
}

QFont AbstractDeclarative::font() const
{
    return m_controller->font();
}

void AbstractDeclarative::setLabelStyle(QDataVis::LabelStyle style)
{
    m_controller->setLabelStyle(style);
}

QDataVis::LabelStyle AbstractDeclarative::labelStyle() const
{
    return m_controller->labelStyle();
}

void AbstractDeclarative::setGridVisible(bool visible)
{
    m_controller->setGridEnabled(visible);
}

bool AbstractDeclarative::isGridVisible() const
{
    return m_controller->gridEnabled();
}

void AbstractDeclarative::setBackgroundVisible(bool visible)
{
    m_controller->setBackgroundEnabled(visible);
}

bool AbstractDeclarative::isBackgroundVisible() const
{
    return m_controller->backgroundEnabled();
}

void AbstractDeclarative::setShadowQuality(QDataVis::ShadowQuality quality)
{
    m_controller->setShadowQuality(quality);
}

QDataVis::ShadowQuality AbstractDeclarative::shadowQuality() const
{
    return m_controller->shadowQuality();
}

void AbstractDeclarative::setSharedController(Abstract3DController *controller)
{
    Q_ASSERT(controller);
    m_controller = controller;
    QObject::connect(m_controller, &Abstract3DController::shadowQualityChanged, this,
                     &AbstractDeclarative::shadowQualityChanged);
    QObject::connect(m_controller, &Abstract3DController::activeInputHandlerChanged, this,
                     &AbstractDeclarative::inputHandlerChanged);
    QObject::connect(m_controller, &Abstract3DController::themeChanged, this,
                     &AbstractDeclarative::themeChanged);
    QObject::connect(m_controller, &Abstract3DController::fontChanged, this,
                     &AbstractDeclarative::fontChanged);
    QObject::connect(m_controller, &Abstract3DController::selectionModeChanged, this,
                     &AbstractDeclarative::selectionModeChanged);
    QObject::connect(m_controller, &Abstract3DController::labelStyleChanged, this,
                     &AbstractDeclarative::labelStyleChanged);
    QObject::connect(m_controller, &Abstract3DController::backgroundVisibleChanged, this,
                     &AbstractDeclarative::backgroundVisibleChanged);
    QObject::connect(m_controller, &Abstract3DController::gridVisibleChanged, this,
                     &AbstractDeclarative::gridVisibleChanged);
}

QAbstract3DInputHandler* AbstractDeclarative::inputHandler() const
{
    return m_controller->activeInputHandler();
}

void AbstractDeclarative::setInputHandler(QAbstract3DInputHandler *inputHandler)
{
    m_controller->setActiveInputHandler(inputHandler);
}

void AbstractDeclarative::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_controller->mouseDoubleClickEvent(event);
}

void AbstractDeclarative::touchEvent(QTouchEvent *event)
{
    m_controller->touchEvent(event);
    update();
}

void AbstractDeclarative::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_controller->mousePressEvent(event, mousePos);
}

void AbstractDeclarative::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_controller->mouseReleaseEvent(event, mousePos);
}

void AbstractDeclarative::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_controller->mouseMoveEvent(event, mousePos);
}

void AbstractDeclarative::wheelEvent(QWheelEvent *event)
{
    m_controller->wheelEvent(event);
}

QT_DATAVISUALIZATION_END_NAMESPACE

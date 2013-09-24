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
#include "theme_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

AbstractDeclarative::AbstractDeclarative(QQuickItem *parent) :
    QQuickItem(parent)
{
}

AbstractDeclarative::~AbstractDeclarative()
{
}

void AbstractDeclarative::setCameraPosition(qreal horizontal, qreal vertical, int distance)
{
    m_controller->setCameraPosition(GLfloat(horizontal), GLfloat(vertical), GLint(distance));
}

void AbstractDeclarative::setCameraPreset(QDataVis::CameraPreset preset)
{
    m_controller->setCameraPreset(preset);
}

QDataVis::CameraPreset AbstractDeclarative::cameraPreset() const
{
    return m_controller->cameraPreset();
}

void AbstractDeclarative::setTheme(QDataVis::ColorTheme theme)
{
    // TODO: Implement correctly once "user-modifiable themes" (QTRD-2120) is implemented
    m_controller->setColorTheme(theme);
}

QDataVis::ColorTheme AbstractDeclarative::theme() const
{
    return m_controller->theme().colorTheme();
}

void AbstractDeclarative::setSelectionMode(QDataVis::SelectionMode mode)
{
    m_controller->setSelectionMode(mode);
}

QDataVis::SelectionMode AbstractDeclarative::selectionMode() const
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

void AbstractDeclarative::setLabelTransparency(QDataVis::LabelTransparency transparency)
{
    m_controller->setLabelTransparency(transparency);
}

QDataVis::LabelTransparency AbstractDeclarative::labelTransparency() const
{
    return m_controller->labelTransparency();
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

void AbstractDeclarative::setItemLabelFormat(const QString &format)
{
    m_controller->activeDataProxy()->setItemLabelFormat(format);
}

QString AbstractDeclarative::itemLabelFormat() const
{
    return m_controller->activeDataProxy()->itemLabelFormat();
}

void AbstractDeclarative::setSharedController(Abstract3DController *controller)
{
    Q_ASSERT(controller);
    m_controller = controller;
    QObject::connect(m_controller, &Abstract3DController::shadowQualityChanged, this,
                     &AbstractDeclarative::handleShadowQualityUpdate);
}


void AbstractDeclarative::mouseDoubleClickEvent(QMouseEvent *event)
{
#if defined(Q_OS_ANDROID)
    m_controller->mouseDoubleClickEvent(event);
#else
    Q_UNUSED(event)
#endif
}

void AbstractDeclarative::touchEvent(QTouchEvent *event)
{
#if defined(Q_OS_ANDROID)
    m_controller->touchEvent(event);
    update();
#else
    Q_UNUSED(event)
#endif
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

void AbstractDeclarative::handleShadowQualityUpdate(QDataVis::ShadowQuality quality)
{
    emit shadowQualityChanged(quality);
}

QT_DATAVISUALIZATION_END_NAMESPACE

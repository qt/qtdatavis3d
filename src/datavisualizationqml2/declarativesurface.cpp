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

#include "declarativesurface_p.h"
#include "declarativesurfacerenderer_p.h"
#include "q3dvalueaxis.h"
#include "qitemmodelsurfacedataproxy.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeSurface::DeclarativeSurface(QQuickItem *parent)
    : QQuickItem(parent),
      m_shared(0),
      m_initialisedSize(0, 0),
      m_theme(QDataVis::ThemeDefault)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_shared = new Surface3DController(boundingRect().toRect());
    QObject::connect(m_shared, &Abstract3DController::shadowQualityChanged, this,
                     &DeclarativeSurface::handleShadowQualityUpdate);

    QItemModelSurfaceDataProxy *proxy = new QItemModelSurfaceDataProxy;
    m_shared->setActiveDataProxy(proxy);
}

DeclarativeSurface::~DeclarativeSurface()
{
    delete m_shared;
}

void DeclarativeSurface::handleShadowQualityUpdate(QDataVis::ShadowQuality quality)
{
    emit shadowQualityChanged(quality);
}

QSGNode *DeclarativeSurface::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    // If old node exists and has right size, reuse it.
    if (oldNode && m_initialisedSize == boundingRect().size().toSize()) {
        // Update bounding rectangle (that has same size as before).
        static_cast<DeclarativeSurfaceRenderer *>( oldNode )->setRect(boundingRect());
        return oldNode;
    }

    // Create a new render node when size changes or if there is no node yet
    m_initialisedSize = boundingRect().size().toSize();

    // Delete old node
    if (oldNode)
        delete oldNode;

    // Create a new one and set it's bounding rectangle
    DeclarativeSurfaceRenderer *node = new DeclarativeSurfaceRenderer(window(), m_shared);
    node->setRect(boundingRect());
    m_shared->setBoundingRect(boundingRect().toRect());
    return node;
}

void DeclarativeSurface::setGradientColorAt(qreal pos, const QColor &color)
{
    m_shared->setGradientColorAt(pos, color);
}

void DeclarativeSurface::setDataProxy(QSurfaceDataProxy *dataProxy)
{
    m_shared->setActiveDataProxy(dataProxy);
}

QSurfaceDataProxy *DeclarativeSurface::dataProxy() const
{
    return static_cast<QSurfaceDataProxy *>(m_shared->activeDataProxy());
}

void DeclarativeSurface::setCameraPreset(QDataVis::CameraPreset preset)
{
    m_shared->setCameraPreset(preset);
}

QDataVis::CameraPreset DeclarativeSurface::cameraPreset()
{
    return m_shared->cameraPreset();

}

Q3DValueAxis *DeclarativeSurface::axisX() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisX());
}

void DeclarativeSurface::setAxisX(Q3DValueAxis *axis)
{
    m_shared->setAxisX(axis);
}

Q3DValueAxis *DeclarativeSurface::axisY() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisY());
}

void DeclarativeSurface::setAxisY(Q3DValueAxis *axis)
{
    m_shared->setAxisY(axis);
}

Q3DValueAxis *DeclarativeSurface::axisZ() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisZ());
}

void DeclarativeSurface::setAxisZ(Q3DValueAxis *axis)
{
    m_shared->setAxisZ(axis);
}

void DeclarativeSurface::setTheme(QDataVis::ColorTheme theme)
{
    // TODO: Implement correctly once "user-modifiable themes" (QTRD-2120) is implemented
    // We need to save this locally, as there are no getters for it in controller
    m_theme = theme;
    m_shared->setColorTheme(theme);
}

QDataVis::ColorTheme DeclarativeSurface::theme()
{
    return m_theme;
}

void DeclarativeSurface::setFont(const QFont &font)
{
    m_shared->setFont(font);
}

QFont DeclarativeSurface::font()
{
    return m_shared->font();
}

void DeclarativeSurface::setLabelTransparency(QDataVis::LabelTransparency transparency)
{
    m_shared->setLabelTransparency(transparency);
}

QDataVis::LabelTransparency DeclarativeSurface::labelTransparency()
{
    return m_shared->labelTransparency();
}

void DeclarativeSurface::setGridVisible(bool visible)
{
    m_shared->setGridEnabled(visible);
}

bool DeclarativeSurface::isGridVisible()
{
    return m_shared->gridEnabled();
}

void DeclarativeSurface::setBackgroundVisible(bool visible)
{
    m_shared->setBackgroundEnabled(visible);
}

bool DeclarativeSurface::isBackgroundVisible()
{
    return m_shared->backgroundEnabled();
}

void DeclarativeSurface::setSmoothSurfaceEnabled(bool enabled)
{
    m_shared->setSmoothSurface(enabled);
}

bool DeclarativeSurface::isSmoothSurfaceEnabled() const
{
    return m_shared->smoothSurface();
}

void DeclarativeSurface::setSurfaceGridEnabled(bool enabled)
{
    m_shared->setSurfaceGrid(enabled);
}

bool DeclarativeSurface::isSurfaceGridEnabled() const
{
    return m_shared->surfaceGrid();
}

void DeclarativeSurface::setSelectionMode(QDataVis::SelectionMode mode)
{
    m_shared->setSelectionMode(mode);
}

QDataVis::SelectionMode DeclarativeSurface::selectionMode()
{
    return m_shared->selectionMode();
}

void DeclarativeSurface::setShadowQuality(QDataVis::ShadowQuality quality)
{
    m_shared->setShadowQuality(quality);
}

QDataVis::ShadowQuality DeclarativeSurface::shadowQuality()
{
    return m_shared->shadowQuality();
}

void DeclarativeSurface::setItemLabelFormat(const QString &format)
{
    m_shared->activeDataProxy()->setItemLabelFormat(format);
}

QString DeclarativeSurface::itemLabelFormat()
{
    return m_shared->activeDataProxy()->itemLabelFormat();
}

void DeclarativeSurface::mouseDoubleClickEvent(QMouseEvent *event)
{
#if defined(Q_OS_ANDROID)
    m_shared->mouseDoubleClickEvent(event);
#else
    Q_UNUSED(event)
#endif
}

void DeclarativeSurface::touchEvent(QTouchEvent *event)
{
#if defined(Q_OS_ANDROID)
    m_shared->touchEvent(event);
    update();
#else
    Q_UNUSED(event)
#endif
}

void DeclarativeSurface::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mousePressEvent(event, mousePos);
}

void DeclarativeSurface::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mouseReleaseEvent(event, mousePos);
}

void DeclarativeSurface::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mouseMoveEvent(event, mousePos);
}

void DeclarativeSurface::wheelEvent(QWheelEvent *event)
{
    m_shared->wheelEvent(event);
}

QT_DATAVISUALIZATION_END_NAMESPACE

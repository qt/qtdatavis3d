/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include "declarativescatter_p.h"
#include "declarativescatterrenderer_p.h"
#include "qitemmodelscatterdataproxy.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

DeclarativeScatter::DeclarativeScatter(QQuickItem *parent)
    : QQuickItem(parent),
      m_shared(0),
      m_initialisedSize(0, 0)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_shared = new Scatter3DController(boundingRect().toRect());
    QObject::connect(m_shared, &Scatter3DController::shadowQualityChanged, this,
                     &DeclarativeScatter::handleShadowQualityUpdate);

    m_shared->setDataProxy(new QItemModelScatterDataProxy);
}

DeclarativeScatter::~DeclarativeScatter()
{
    delete m_shared;
}

void DeclarativeScatter::handleShadowQualityUpdate(QtDataVis3D::ShadowQuality quality)
{
    emit shadowQualityChanged(DeclarativeScatter::ShadowQuality(quality));
}

void DeclarativeScatter::classBegin()
{
    qDebug() << "classBegin";
}

void DeclarativeScatter::componentComplete()
{
    qDebug() << "componentComplete";
}

QSGNode *DeclarativeScatter::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    // Call initialize on each update paint node and let the shared code worry about it.
    m_shared->initializeOpenGL();

    // If old node exists and has right size, reuse it.
    if (oldNode && m_initialisedSize == boundingRect().size().toSize()) {
        // Update bounding rectangle (that has same size as before).
        static_cast<DeclarativeScatterRenderer *>( oldNode )->setRect(boundingRect());
        return oldNode;
    }

    // Create a new render node when size changes or if there is no node yet
    m_initialisedSize = boundingRect().size().toSize();

    // Delete old node
    if (oldNode)
        delete oldNode;

    // Create a new one and set it's bounding rectangle
    DeclarativeScatterRenderer *node = new DeclarativeScatterRenderer(window(), m_shared);
    node->setRect(boundingRect());
    m_shared->setBoundingRect(boundingRect().toRect());
    return node;
}

void DeclarativeScatter::setBarType(BarStyle style, bool smooth)
{
    m_shared->setBarType(style, smooth);
}

void DeclarativeScatter::setCameraPreset(CameraPreset preset)
{
    m_shared->setCameraPreset(preset);
}

void DeclarativeScatter::setCameraPosition(qreal horizontal, qreal vertical, int distance)
{
    m_shared->setCameraPosition(GLfloat(horizontal), GLfloat(vertical), GLint(distance));
}

void DeclarativeScatter::setTheme(ColorTheme theme)
{
    m_shared->setColorTheme(theme);
}

void DeclarativeScatter::setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                                     bool uniform)
{
    m_shared->setBarColor(baseColor, heightColor, depthColor, uniform);
}

void DeclarativeScatter::setFontSize(float fontsize)
{
    m_shared->setFontSize(fontsize);
}

float DeclarativeScatter::fontSize()
{
    return m_shared->fontSize();
}

void DeclarativeScatter::setFont(const QFont &font)
{
    m_shared->setFont(font);
}

QFont DeclarativeScatter::font()
{
    return m_shared->font();
}

void DeclarativeScatter::setLabelTransparency(DeclarativeScatter::LabelTransparency transparency)
{
    m_shared->setLabelTransparency(QtDataVis3D::LabelTransparency(transparency));
}

DeclarativeScatter::LabelTransparency DeclarativeScatter::labelTransparency()
{
    return DeclarativeScatter::LabelTransparency(m_shared->labelTransparency());
}

void DeclarativeScatter::setGridVisible(bool visible)
{
    m_shared->setGridEnabled(visible);
}

bool DeclarativeScatter::isGridVisible()
{
    return m_shared->gridEnabled();
}

void DeclarativeScatter::setBackgroundVisible(bool visible)
{
    m_shared->setBackgroundEnabled(visible);
}

bool DeclarativeScatter::isBackgroundVisible()
{
    return m_shared->backgroundEnabled();
}

void DeclarativeScatter::setData(QAbstractItemModel *data)
{
    static_cast<QItemModelScatterDataProxy *>(m_shared->dataProxy())->setItemModel(data);
}

QAbstractItemModel *DeclarativeScatter::data()
{
    return static_cast<QItemModelScatterDataProxy *>(m_shared->dataProxy())->itemModel();
}

void DeclarativeScatter::setSelectionMode(DeclarativeScatter::SelectionMode mode)
{
    m_shared->setSelectionMode(QtDataVis3D::SelectionMode(mode));
}

DeclarativeScatter::SelectionMode DeclarativeScatter::selectionMode()
{
    return DeclarativeScatter::SelectionMode(m_shared->selectionMode());
}

void DeclarativeScatter::setShadowQuality(DeclarativeScatter::ShadowQuality quality)
{
    m_shared->setShadowQuality(QtDataVis3D::ShadowQuality(quality));
}

DeclarativeScatter::ShadowQuality DeclarativeScatter::shadowQuality()
{
    return DeclarativeScatter::ShadowQuality(m_shared->shadowQuality());
}

QItemModelScatterDataMapping *DeclarativeScatter::mapping() const
{
    return static_cast<QItemModelScatterDataProxy *>(m_shared->dataProxy())->mapping();
}

void DeclarativeScatter::setMapping(QItemModelScatterDataMapping *mapping)
{
    static_cast<QItemModelScatterDataProxy *>(m_shared->dataProxy())->setMapping(mapping);
}

void DeclarativeScatter::setMeshFileName(const QString &objFileName)
{
    m_shared->setMeshFileName(objFileName);
}

void DeclarativeScatter::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mousePressEvent(event, mousePos);
}

void DeclarativeScatter::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mouseReleaseEvent(event, mousePos);
}

void DeclarativeScatter::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mouseMoveEvent(event, mousePos);
}

void DeclarativeScatter::wheelEvent(QWheelEvent *event)
{
    m_shared->wheelEvent(event);
}

QT_DATAVIS3D_END_NAMESPACE

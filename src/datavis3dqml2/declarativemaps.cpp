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

#include "declarativemaps_p.h"
#include "declarativemapsrenderer_p.h"
#include "qitemmodelmapdataproxy.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

DeclarativeMaps::DeclarativeMaps(QQuickItem *parent)
    : QQuickItem(parent),
      m_shared(0),
      m_initializedSize(0, 0)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);
}

DeclarativeMaps::~DeclarativeMaps()
{
    delete m_shared;
}

void DeclarativeMaps::classBegin()
{
    //qDebug() << "classBegin";
}

void DeclarativeMaps::componentComplete()
{
    //qDebug() << "componentComplete";
}

QSGNode *DeclarativeMaps::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (!m_shared) {
        m_shared = new Maps3DController(boundingRect().toRect());
        m_shared->setDataProxy(new QItemModelMapDataProxy);
        m_shared->initializeOpenGL();
    }

    // If old node exists and has right size, reuse it.
    if (oldNode && m_initializedSize == boundingRect().size().toSize()) {
        // Update bounding rectangle (that has same size as before).
        static_cast<DeclarativeMapsRenderer *>( oldNode )->setRect(boundingRect());
        return oldNode;
    }

    // Create a new render node when size changes or if there is no node yet
    m_initializedSize = boundingRect().size().toSize();

    // Delete old node
    if (oldNode)
        delete oldNode;

    // Create a new one and set it's bounding rectangle
    DeclarativeMapsRenderer *node = new DeclarativeMapsRenderer(window(), m_shared);
    node->setRect(boundingRect());
    m_shared->setBoundingRect(boundingRect().toRect());
    return node;
}

void DeclarativeMaps::setData(QAbstractItemModel *data)
{
    static_cast<QItemModelMapDataProxy *>(m_shared->dataProxy())->setItemModel(data);
}

QAbstractItemModel *DeclarativeMaps::data()
{
    return static_cast<QItemModelMapDataProxy *>(m_shared->dataProxy())->itemModel();
}

void DeclarativeMaps::setBarSpecs(const QVector3D &thickness,
                                  Q3DMaps::AdjustmentDirection direction)
{
    m_shared->setBarSpecs(thickness, direction);
}

void DeclarativeMaps::setBarType(QDataVis::MeshStyle style, bool smooth)
{
    m_shared->setBarType(style, smooth);
}

void DeclarativeMaps::setMeshFileName(const QString &objFileName)
{
    m_shared->setMeshFileName(objFileName);
}

void DeclarativeMaps::setCameraPreset(QDataVis::CameraPreset preset)
{
    m_shared->setCameraPreset(preset);
}

void DeclarativeMaps::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    m_shared->setCameraPosition(horizontal, vertical, distance);
}

void DeclarativeMaps::setTheme(QDataVis::ColorTheme theme)
{
    m_shared->setTheme(theme);
}

void DeclarativeMaps::setBarColor(const QColor &baseColor, const QColor &heightColor, bool uniform)
{
    m_shared->setBarColor(baseColor, heightColor, uniform);
}

void DeclarativeMaps::setAreaSpecs(const QRect &areaRect, const QImage &image)
{
    m_shared->setAreaSpecs(areaRect, image);
}

void DeclarativeMaps::setImage(const QImage &image)
{
    m_shared->setImage(image);
}

void DeclarativeMaps::setImage(const QString &imageUrl)
{
    m_shared->setImage(QImage(imageUrl));
}

void DeclarativeMaps::setSelectionMode(QDataVis::SelectionMode mode)
{
    m_shared->setSelectionMode(mode);
}

QDataVis::SelectionMode DeclarativeMaps::selectionMode()
{
    return m_shared->selectionMode();
}

void DeclarativeMaps::setFont(const QFont &font)
{
    m_shared->setFont(font);
}

QFont DeclarativeMaps::font()
{
    return m_shared->font();
}

void DeclarativeMaps::setLabelTransparency(QDataVis::LabelTransparency transparency)
{
    m_shared->setLabelTransparency(transparency);
}

QDataVis::LabelTransparency DeclarativeMaps::labelTransparency()
{
    return m_shared->labelTransparency();
}

void DeclarativeMaps::setShadowQuality(QDataVis::ShadowQuality quality)
{
    m_shared->setShadowQuality(quality);
}

QDataVis::ShadowQuality DeclarativeMaps::shadowQuality()
{
    return m_shared->shadowQuality();
}

QItemModelMapDataMapping *DeclarativeMaps::mapping() const
{
    return static_cast<QItemModelMapDataProxy *>(m_shared->dataProxy())->mapping();
}

void DeclarativeMaps::setMapping(QItemModelMapDataMapping *mapping)
{
    static_cast<QItemModelMapDataProxy *>(m_shared->dataProxy())->setMapping(mapping);
}

void DeclarativeMaps::mouseDoubleClickEvent(QMouseEvent *event)
{
#if defined(Q_OS_ANDROID)
    m_shared->mouseDoubleClickEvent(event);
#endif
}

void DeclarativeMaps::touchEvent(QTouchEvent *event)
{
#if defined(Q_OS_ANDROID)
    m_shared->touchEvent(event);
    update();
#endif
}

void DeclarativeMaps::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mousePressEvent(event, mousePos);
}

void DeclarativeMaps::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mouseReleaseEvent(event, mousePos);
}

void DeclarativeMaps::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mouseMoveEvent(event, mousePos);
}

void DeclarativeMaps::wheelEvent(QWheelEvent *event)
{
    m_shared->wheelEvent(event);
}

QT_DATAVIS3D_END_NAMESPACE

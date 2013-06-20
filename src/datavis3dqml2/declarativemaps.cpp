/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "declarativemaps.h"
#include "maps3dcontroller_p.h"
#include "qdataitem.h"
#include "qdatarow.h"
#include "qdatarow_p.h"

#include <QQuickWindow>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>
#include <QGuiApplication>
#include <QThread>
#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

DeclarativeMaps::DeclarativeMaps(QQuickItem *parent)
    : QQuickItem(parent),
      m_shared(0),
      m_cachedState(new DeclarativeMapsCachedStatePrivate()),
      m_initializedSize(0,0)
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
    qDebug() << "classBegin";
}

void DeclarativeMaps::componentComplete()
{
    qDebug() << "componentComplete";
}

QSGNode *DeclarativeMaps::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (!m_shared) {
        m_shared = new Maps3DController(boundingRect().toRect());
        m_shared->initializeOpenGL();
    }

    // Check if properites have changed that need to be applied while on the SGRenderThread
    if (m_cachedState->m_isAreaRectSet) {
        m_shared->setAreaSpecs(m_cachedState->m_arearect, m_cachedState->m_image);
        m_cachedState->m_isAreaRectSet = false;
    }

    if (m_cachedState->m_isImageSet) {
        m_shared->setImage(m_cachedState->m_image);
        m_cachedState->m_isImageSet = false;
    }

    if (m_cachedState->m_isSelectionModeSet) {
        m_shared->setSelectionMode(m_cachedState->m_selectionMode);
        m_cachedState->m_isSelectionModeSet = false;
    }

    if (m_cachedState->m_isLabelTransparencySet) {
        m_shared->setLabelTransparency(m_cachedState->m_labelTransparency);
        m_cachedState->m_isLabelTransparencySet = false;
    }

    if (m_cachedState->m_isShadowQualitySet) {
        m_shared->setShadowQuality(m_cachedState->m_shadowQuality);
        m_cachedState->m_isShadowQualitySet = false;
    }

    if (m_cachedState->m_data) {
        if (m_cachedState->m_replaceData)
            m_shared->setData(m_cachedState->m_data);
        else
            m_shared->addData(*m_cachedState->m_data);
        m_cachedState->m_data = 0;
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

void DeclarativeMaps::setBarSpecs(const QVector3D &thickness,
                                  Q3DMaps::AdjustmentDirection direction)
{
    m_shared->setBarSpecs(thickness, direction);
}

void DeclarativeMaps::setBarType(BarStyle style, bool smooth)
{
    m_shared->setBarType(style, smooth);
}

void DeclarativeMaps::setMeshFileName(const QString &objFileName)
{
    m_shared->setMeshFileName(objFileName);
}

void DeclarativeMaps::setCameraPreset(CameraPreset preset)
{
    m_shared->setCameraPreset(preset);
}

void DeclarativeMaps::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    m_shared->setCameraPosition(horizontal, vertical, distance);
}

void DeclarativeMaps::setTheme(ColorTheme theme)
{
    m_shared->setTheme(theme);
}

void DeclarativeMaps::setBarColor(QColor baseColor, QColor heightColor, bool uniform)
{
    m_shared->setBarColor(baseColor, heightColor, uniform);
}

void DeclarativeMaps::setAreaSpecs(const QRect &areaRect, const QImage &image)
{
    m_cachedState->m_arearect = areaRect;
    m_cachedState->m_isAreaRectSet = true;
    m_cachedState->m_image = image;
    update();
}

void DeclarativeMaps::setImage(const QImage &image)
{
    m_cachedState->m_image = image;
    m_cachedState->m_isImageSet = true;
    update();
}

void DeclarativeMaps::setImage(const QString &imageUrl)
{
    m_cachedState->m_image = QImage(imageUrl);
    m_cachedState->m_isImageSet = true;
    update();
}

void DeclarativeMaps::setSelectionMode(DeclarativeMaps::SelectionMode mode)
{
    m_cachedState->m_selectionMode = QtDataVis3D::SelectionMode(mode);
    m_cachedState->m_isSelectionModeSet = true;
    update();
}

DeclarativeMaps::SelectionMode DeclarativeMaps::selectionMode()
{
    return DeclarativeMaps::SelectionMode(m_shared->selectionMode());
}

void DeclarativeMaps::setFontSize(float fontsize)
{
    m_shared->setFontSize(fontsize);
}

float DeclarativeMaps::fontSize()
{
    return m_shared->fontSize();
}

void DeclarativeMaps::setFont(const QFont &font)
{
    m_shared->setFont(font);
}

QFont DeclarativeMaps::font()
{
    return m_shared->font();
}

void DeclarativeMaps::setLabelTransparency(DeclarativeMaps::LabelTransparency transparency)
{
    m_cachedState->m_labelTransparency = QtDataVis3D::LabelTransparency(transparency);
    m_cachedState->m_isLabelTransparencySet = true;
    update();
}

DeclarativeMaps::LabelTransparency DeclarativeMaps::labelTransparency()
{
    return DeclarativeMaps::LabelTransparency(m_shared->labelTransparency());
}

void DeclarativeMaps::setShadowQuality(DeclarativeMaps::ShadowQuality quality)
{
    m_cachedState->m_shadowQuality = QtDataVis3D::ShadowQuality(quality);
    m_cachedState->m_isShadowQualitySet = true;
    update();
}

DeclarativeMaps::ShadowQuality DeclarativeMaps::shadowQuality()
{
    return DeclarativeMaps::ShadowQuality(m_shared->shadowQuality());
}

void DeclarativeMaps::addDataItem(QDataItem *dataItem)
{
    qDebug() << "Enter DeclarativeMaps::addDataItem(QDataItem *dataItem)";
    QDataItem *newItem = new QDataItem(*dataItem);
    m_cachedState->appendData(newItem);
    update();
}

void DeclarativeMaps::addData(const QVector<QDataItem *> &data)
{
    qDebug() << "Enter DeclarativeMaps::addData(const QVector<QDataItem *> &data)";
    QDataItem *newItem;
    for (int i = 0; i < data.count(); i++) {
        newItem = new QDataItem(*data.at(i));
        m_cachedState->appendData(newItem);
    }
    update();
}

void DeclarativeMaps::addData(const QDataRow &dataRow)
{
    qDebug() << "Enter DeclarativeMaps::addData(const QDataRow &dataRow)";
    QDataItem *newItem;
    for (int i = 0; i < dataRow.d_ptr->row().count(); i++) {
        newItem = new QDataItem(*dataRow.d_ptr->getItem(i));
        m_cachedState->appendData(newItem);
    }
    update();
}

void DeclarativeMaps::setData(const QVector<QDataItem *> &data)
{
    qDebug() << "Enter DeclarativeMaps::setData(const QVector<QDataItem *> &data)";
    QDataItem *newItem;
    delete m_cachedState->m_data;
    m_cachedState->m_data = new QDataRow();
    for (int i = 0; i < data.count(); i++) {
        newItem = new QDataItem(*data.at(i));
        m_cachedState->appendData(newItem);
    }
    m_cachedState->m_replaceData = true;
    update();
}

void DeclarativeMaps::setData(QDataRow *data)
{
    qDebug() << "Enter DeclarativeMaps::setData(const QVector<QDataItem*> &data)";
    QDataItem *newItem;
    delete m_cachedState->m_data;
    m_cachedState->m_data = new QDataRow();
    for (int i = 0; i < data->d_ptr->row().count(); i++) {
        newItem = new QDataItem(*data->d_ptr->getItem(i));
        m_cachedState->appendData(newItem);
    }
    m_cachedState->m_replaceData = true;
    update();
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

DeclarativeMapsRenderer::DeclarativeMapsRenderer(QQuickWindow *window, Maps3DController *renderer)
    : m_fbo(0),
      m_texture(0),
      m_window(window),
      m_mapsRenderer(renderer)
{
    connect(m_window, SIGNAL(beforeRendering()), this, SLOT(render()), Qt::DirectConnection);
}

DeclarativeMapsRenderer::~DeclarativeMapsRenderer()
{
    delete m_texture;
    delete m_fbo;
}

void DeclarativeMapsRenderer::render()
{
    QSize size = rect().size().toSize();

    // Create FBO
    if (!m_fbo) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Depth);
        m_fbo = new QOpenGLFramebufferObject(size, format);
        m_texture = m_window->createTextureFromId(m_fbo->texture(), size);

        setTexture(m_texture);

        // Flip texture
        // TODO: Can be gotten rid of once support for texture flipping becomes available (in Qt5.2)
        QSize ts = m_texture->textureSize();
        QRectF sourceRect(0, 0, ts.width(), ts.height());
        float tmp = sourceRect.top();
        sourceRect.setTop(sourceRect.bottom());
        sourceRect.setBottom(tmp);
        QSGGeometry *geometry = this->geometry();
        QSGGeometry::updateTexturedRectGeometry(geometry, rect(),
                                                m_texture->convertToNormalizedSourceRect(sourceRect));
        markDirty(DirtyMaterial);
        //qDebug() << "create node" << m_fbo->handle() << m_texture->textureId() << m_fbo->size();
    }

    // Call the shared rendering function
    m_fbo->bind();

    m_mapsRenderer->render(m_fbo->handle());

    m_fbo->release();

    // New view is in the FBO, request repaint of scene graph
    m_window->update();
}


DeclarativeMapsCachedStatePrivate::DeclarativeMapsCachedStatePrivate()
    : m_data(0)
{
}

DeclarativeMapsCachedStatePrivate::~DeclarativeMapsCachedStatePrivate()
{
}

void DeclarativeMapsCachedStatePrivate::appendData(QDataItem *item)
{
    if (!m_data)
        m_data = new QDataRow();

    m_data->addItem(item);
    m_replaceData = false;
}

QT_DATAVIS3D_END_NAMESPACE

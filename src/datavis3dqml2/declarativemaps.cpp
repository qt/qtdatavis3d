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
#include "qitemmodelmapdataproxy.h"
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

void DeclarativeMaps::setData(QAbstractItemModel *data)
{
    m_cachedState->m_model = data;
}

QAbstractItemModel *DeclarativeMaps::data()
{
    if (m_cachedState->m_model)
        return m_cachedState->m_model;
    else
        return static_cast<QItemModelMapDataProxy *>(m_shared->dataProxy())->itemModel();
}

QSGNode *DeclarativeMaps::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    if (!m_shared) {
        m_shared = new Maps3DController(boundingRect().toRect());
        m_shared->setDataProxy(new QItemModelMapDataProxy);
        m_shared->initializeOpenGL();
    }

    if (m_cachedState->m_model) {
        static_cast<QItemModelMapDataProxy *>(m_shared->dataProxy())->setItemModel(m_cachedState->m_model);
        m_cachedState->m_model = 0;
    }

    if (m_cachedState->m_mapping) {
        static_cast<QItemModelMapDataProxy *>(m_shared->dataProxy())->setMapping(m_cachedState->m_mapping);
        m_cachedState->m_mapping = 0;
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

    if (m_cachedState->m_isBarSpecsSet) {
        m_shared->setBarSpecs(m_cachedState->m_thickness, m_cachedState->m_direction);
        m_cachedState->m_isBarSpecsSet = false;
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

    if (m_cachedState->m_fontsize) {
        m_shared->setFontSize(m_cachedState->m_fontsize);
        m_cachedState->m_fontsize = 0.0;
    }

    if (m_cachedState->m_isBarStyleSet) {
        m_shared->setBarType(m_cachedState->m_barstyle, m_cachedState->m_barsmooth);
        m_cachedState->m_isBarStyleSet = false;
    }

    if (m_cachedState->m_isMeshSet) {
        m_shared->setMeshFileName(m_cachedState->m_meshfilename);
        m_cachedState->m_isMeshSet = false;
    }

    if (m_cachedState->m_isCameraPresetSet) {
        m_shared->setCameraPreset(m_cachedState->m_camerapreset);
        m_cachedState->m_isCameraPresetSet = false;
    }

    if (m_cachedState->m_camdistance) {
        m_shared->setCameraPosition(m_cachedState->m_camhorizontal,
                                    m_cachedState->m_camvertical,
                                    m_cachedState->m_camdistance);
        m_cachedState->m_camdistance = 0.0f;
    }

    if (m_cachedState->m_isThemeSet) {
        m_shared->setTheme(m_cachedState->m_theme);
        m_cachedState->m_isThemeSet = false;
    }

    if (m_cachedState->m_isColorSet) {
        m_shared->setBarColor(m_cachedState->m_basecolor,
                              m_cachedState->m_heightcolor,
                              m_cachedState->m_coloruniform);
        m_cachedState->m_isColorSet = false;
    }

    if (m_cachedState->m_isFontSet) {
        m_shared->setFont(m_cachedState->m_font);
        m_cachedState->m_isFontSet = false;
    }

    if (m_cachedState->m_isLabelTransparencySet) {
        m_shared->setLabelTransparency(m_cachedState->m_labelTransparency);
        m_cachedState->m_isLabelTransparencySet = false;
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
    m_cachedState->m_thickness = thickness;
    m_cachedState->m_direction = direction;
    m_cachedState->m_isBarSpecsSet = true;
    update();
}

void DeclarativeMaps::setBarType(BarStyle style, bool smooth)
{
    m_cachedState->m_barstyle = style;
    m_cachedState->m_barsmooth = smooth;
    m_cachedState->m_isBarStyleSet = true;
    update();
}

void DeclarativeMaps::setMeshFileName(const QString &objFileName)
{
    m_cachedState->m_meshfilename = objFileName;
    m_cachedState->m_isMeshSet = true;
    update();
}

void DeclarativeMaps::setCameraPreset(CameraPreset preset)
{
    m_cachedState->m_camerapreset = preset;
    m_cachedState->m_isCameraPresetSet = true;
    update();
}

void DeclarativeMaps::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    m_cachedState->m_camhorizontal = horizontal;
    m_cachedState->m_camvertical = vertical;
    m_cachedState->m_camdistance = distance;
    update();
}

void DeclarativeMaps::setTheme(ColorTheme theme)
{
    m_cachedState->m_theme = theme;
    m_cachedState->m_isThemeSet = true;
    update();
}

void DeclarativeMaps::setBarColor(QColor baseColor, QColor heightColor, bool uniform)
{
    m_cachedState->m_basecolor = baseColor;
    m_cachedState->m_heightcolor = heightColor;
    m_cachedState->m_coloruniform = uniform;
    m_cachedState->m_isColorSet = true;
    update();
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
    if (m_shared)
        return DeclarativeMaps::SelectionMode(m_shared->selectionMode());
    else
        return DeclarativeMaps::ModeBar;
}

void DeclarativeMaps::setFontSize(float fontsize)
{
    m_cachedState->m_fontsize = fontsize;
}

float DeclarativeMaps::fontSize()
{
    if (m_shared)
        return m_shared->fontSize();
    else
        return QFont(QStringLiteral("Arial")).pointSizeF();
}

void DeclarativeMaps::setFont(const QFont &font)
{
    m_cachedState->m_font = font;
    m_cachedState->m_isFontSet = true;
    update();
}

QFont DeclarativeMaps::font()
{
    if (m_shared)
        return m_shared->font();
    else
        return QFont(QStringLiteral("Arial"));
}

void DeclarativeMaps::setLabelTransparency(DeclarativeMaps::LabelTransparency transparency)
{
    m_cachedState->m_labelTransparency = QtDataVis3D::LabelTransparency(transparency);
    m_cachedState->m_isLabelTransparencySet = true;
    update();
}

DeclarativeMaps::LabelTransparency DeclarativeMaps::labelTransparency()
{
    if (m_shared)
        return DeclarativeMaps::LabelTransparency(m_shared->labelTransparency());
    else
        return DeclarativeMaps::TransparencyFromTheme;
}

void DeclarativeMaps::setShadowQuality(DeclarativeMaps::ShadowQuality quality)
{
    m_cachedState->m_shadowQuality = QtDataVis3D::ShadowQuality(quality);
    m_cachedState->m_isShadowQualitySet = true;
    update();
}

DeclarativeMaps::ShadowQuality DeclarativeMaps::shadowQuality()
{
    if (m_shared)
        return DeclarativeMaps::ShadowQuality(m_shared->shadowQuality());
    else
        return DeclarativeMaps::ShadowLow;
}

QItemModelMapDataMapping *DeclarativeMaps::mapping() const
{
    if (m_cachedState->m_mapping)
        return m_cachedState->m_mapping;
    else
        return static_cast<QItemModelMapDataProxy *>(m_shared->dataProxy())->mapping();
}

void DeclarativeMaps::setMapping(QItemModelMapDataMapping *mapping)
{
    m_cachedState->m_mapping = mapping;
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
    : m_isImageSet(false),
      m_isBarSpecsSet(false),
      m_isAreaRectSet(false),
      m_isSelectionModeSet(false),
      m_isLabelTransparencySet(false),
      m_isShadowQualitySet(false),
      m_fontsize(0.0f),
      m_isBarStyleSet(false),
      m_isMeshSet(false),
      m_isCameraPresetSet(false),
      m_camdistance(0.0f),
      m_isThemeSet(false),
      m_isColorSet(false),
      m_isFontSet(false)
{
}

DeclarativeMapsCachedStatePrivate::~DeclarativeMapsCachedStatePrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE

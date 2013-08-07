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

#include "declarativebars.h"
#include "bars3dcontroller_p.h"
#include "qitemmodelbardataproxy.h"
#include "qvalueaxis.h"

#include <QtQuick/QQuickWindow>
#include <QtGui/QOpenGLFramebufferObject>
#include <QOpenGLContext>
#include <QGuiApplication>
#include <QThread>
#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

DeclarativeBars::DeclarativeBars(QQuickItem *parent)
    : QQuickItem(parent),
      m_shared(0),
      m_cachedState(new DeclarativeBarsCachedStatePrivate()),
      m_initialisedSize(0,0)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_shared = new Bars3dController(boundingRect().toRect());
    QObject::connect(m_shared, &Bars3dController::shadowQualityChanged, this,
                     &DeclarativeBars::handleShadowQualityUpdate);

    m_shared->setDataProxy(new QItemModelBarDataProxy);
}

DeclarativeBars::~DeclarativeBars()
{
    delete m_shared;
}

void DeclarativeBars::handleShadowQualityUpdate(QtDataVis3D::ShadowQuality quality)
{
    emit shadowQualityChanged(DeclarativeBars::ShadowQuality(quality));
}


void DeclarativeBars::classBegin()
{
    qDebug() << "classBegin";
}

void DeclarativeBars::componentComplete()
{
    qDebug() << "componentComplete";
}

QSGNode *DeclarativeBars::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    // Call initialize on each update paint node and let the shared code worry about it.
    m_shared->initializeOpenGL();

    // TODO: Setting stuff to controller in render thread. Isn't this incorrect?

    // Check if properites have changed that need to be applied while on the SGRenderThread
    if (m_cachedState->m_isSampleSpaceSet) {
        m_shared->setupSampleSpace(m_cachedState->m_samplesRow, m_cachedState->m_samplesColumn);
        m_cachedState->m_isSampleSpaceSet = false;
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

    if (m_cachedState->m_isGridSet) {
        m_shared->setGridEnabled(m_cachedState->m_isGridEnabled);
        m_cachedState->m_isGridSet = false;
    }

    if (m_cachedState->m_isTickCountSet) {
        // TODO needs proper axis support on QML side, too
        QValueAxis *axis = static_cast<QValueAxis *>(m_shared->axisY());
        if (axis) {
            axis->setTickCount(m_cachedState->m_tickCount);
            axis->setRange(m_cachedState->m_tickMin,
                          (m_cachedState->m_tickCount * m_cachedState->m_tickStep));
        }
        m_cachedState->m_isTickCountSet = false;
    }


    // If old node exists and has right size, reuse it.
    if (oldNode && m_initialisedSize == boundingRect().size().toSize()) {
        // Update bounding rectangle (that has same size as before).
        static_cast<DeclarativeBarsRenderer *>( oldNode )->setRect(boundingRect());
        return oldNode;
    }

    // Create a new render node when size changes or if there is no node yet
    m_initialisedSize = boundingRect().size().toSize();

    // Delete old node
    if (oldNode)
        delete oldNode;

    // Create a new one and set it's bounding rectangle
    DeclarativeBarsRenderer *node = new DeclarativeBarsRenderer(window(), m_shared);
    node->setRect(boundingRect());
    m_shared->setBoundingRect(boundingRect().toRect());
    return node;
}

void DeclarativeBars::setBarSpecs(QSizeF thickness, QSizeF spacing, bool relative)
{
    m_shared->setBarSpecs(thickness, spacing, relative);
}

void DeclarativeBars::setBarType(BarStyle style, bool smooth)
{
    m_shared->setBarType(style, smooth);
}

void DeclarativeBars::setupSampleSpace(int samplesRow, int samplesColumn)
{
    m_cachedState->m_samplesRow = samplesRow;
    m_cachedState->m_samplesColumn = samplesColumn;
    m_cachedState->m_isSampleSpaceSet = true;

    update();
}

void DeclarativeBars::setCameraPreset(CameraPreset preset)
{
    m_shared->setCameraPreset(preset);
}

void DeclarativeBars::setCameraPosition(qreal horizontal, qreal vertical, int distance)
{
    m_shared->setCameraPosition(GLfloat(horizontal), GLfloat(vertical), GLint(distance));
}

void DeclarativeBars::setTheme(ColorTheme theme)
{
    m_shared->setColorTheme(theme);
}

void DeclarativeBars::setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                                  bool uniform)
{
    m_shared->setBarColor(baseColor, heightColor, depthColor, uniform);
}


void DeclarativeBars::setFontSize(float fontsize)
{
    m_shared->setFontSize(fontsize);
}

float DeclarativeBars::fontSize()
{
    return m_shared->fontSize();
}

void DeclarativeBars::setFont(const QFont &font)
{
    m_shared->setFont(font);
}

QFont DeclarativeBars::font()
{
    return m_shared->font();
}

void DeclarativeBars::setLabelTransparency(DeclarativeBars::LabelTransparency transparency)
{
    m_cachedState->m_labelTransparency = QtDataVis3D::LabelTransparency(transparency);
    m_cachedState->m_isLabelTransparencySet = true;

    update();
}

DeclarativeBars::LabelTransparency DeclarativeBars::labelTransparency()
{
    return DeclarativeBars::LabelTransparency(m_shared->labelTransparency());
}

void DeclarativeBars::setGridVisible(bool visible)
{
    m_cachedState->m_isGridEnabled = visible;
    m_cachedState->m_isGridSet = true;

    update();
}

bool DeclarativeBars::isGridVisible()
{
    return m_shared->gridEnabled();
}

void DeclarativeBars::setBackgroundVisible(bool visible)
{
    m_shared->setBackgroundEnabled(visible);
}

bool DeclarativeBars::isBackgroundVisible()
{
    return m_shared->backgroundEnabled();
}

// TODO needs proper axis support also in QML
void DeclarativeBars::setTickCount(int tickCount, qreal step, qreal minimum)
{
    m_cachedState->m_isTickCountSet = true;
    m_cachedState->m_tickCount = tickCount;
    m_cachedState->m_tickStep = step;
    m_cachedState->m_tickMin = minimum;

    update();
}

void DeclarativeBars::setData(QAbstractItemModel *data)
{
    static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->setItemModel(data);
}

QAbstractItemModel *DeclarativeBars::data()
{
    return static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->itemModel();
}

void DeclarativeBars::setSelectionMode(DeclarativeBars::SelectionMode mode)
{
    m_cachedState->m_selectionMode = QtDataVis3D::SelectionMode(mode);
    m_cachedState->m_isSelectionModeSet = true;
    update();
}

DeclarativeBars::SelectionMode DeclarativeBars::selectionMode()
{
    return DeclarativeBars::SelectionMode(m_shared->selectionMode());
}

void DeclarativeBars::setShadowQuality(DeclarativeBars::ShadowQuality quality)
{
    m_cachedState->m_shadowQuality = QtDataVis3D::ShadowQuality(quality);
    m_cachedState->m_isShadowQualitySet = true;
    update();
}

DeclarativeBars::ShadowQuality DeclarativeBars::shadowQuality()
{
    return DeclarativeBars::ShadowQuality(m_shared->shadowQuality());
}

QItemModelBarDataMapping *DeclarativeBars::mapping() const
{
    return static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->mapping();
}

void DeclarativeBars::setMapping(QItemModelBarDataMapping *mapping)
{
    qDebug() << "Setting the mapping!!";
    static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->setMapping(mapping);
}

int DeclarativeBars::rows() const
{
    return m_cachedState->m_samplesRow;
}

void DeclarativeBars::setRows(int rows)
{
    m_cachedState->m_samplesRow = rows;
    setupSampleSpace(m_cachedState->m_samplesRow, m_cachedState->m_samplesColumn);
}

int DeclarativeBars::columns() const
{
    return m_cachedState->m_samplesColumn;
}

void DeclarativeBars::setColumns(int columns)
{
    m_cachedState->m_samplesColumn = columns;
    setupSampleSpace(m_cachedState->m_samplesRow, m_cachedState->m_samplesColumn);
}

void DeclarativeBars::setMeshFileName(const QString &objFileName)
{
    m_shared->setMeshFileName(objFileName);
}

void DeclarativeBars::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mousePressEvent(event, mousePos);
}

void DeclarativeBars::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mouseReleaseEvent(event, mousePos);
}

void DeclarativeBars::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    //mousePos.setY(height() - mousePos.y());
    m_shared->mouseMoveEvent(event, mousePos);
}

void DeclarativeBars::wheelEvent(QWheelEvent *event)
{
    m_shared->wheelEvent(event);
}

DeclarativeBarsRenderer::DeclarativeBarsRenderer(QQuickWindow *window, Bars3dController *renderer)
    : m_fbo(0),
      m_texture(0),
      m_window(window),
      m_barsRenderer(renderer)
{
    connect(m_window, SIGNAL(beforeRendering()), this, SLOT(render()), Qt::DirectConnection);
}

DeclarativeBarsRenderer::~DeclarativeBarsRenderer()
{
    delete m_texture;
    delete m_fbo;
}

void DeclarativeBarsRenderer::render()
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

    m_barsRenderer->render(m_fbo->handle());

    m_fbo->release();

    // New view is in the FBO, request repaint of scene graph
    m_window->update();
}

DeclarativeBarsCachedStatePrivate::DeclarativeBarsCachedStatePrivate() :
    m_isSampleSpaceSet(false),
    m_samplesRow(0),
    m_samplesColumn(0),
    m_labelRow(QStringLiteral("")),
    m_labelColumn(QStringLiteral("")),
    m_labelHeight(QStringLiteral("")),
    m_isSelectionModeSet(false),
    m_selectionMode(ModeNone),
    m_isLabelTransparencySet(false),
    m_labelTransparency(TransparencyNone),
    m_isShadowQualitySet(false),
    m_shadowQuality(ShadowNone),
    m_isGridSet(false),
    m_isGridEnabled(true),
    m_isTickCountSet(false),
    m_tickCount(5),
    m_tickStep(1),
    m_tickMin(0)


{
}

DeclarativeBarsCachedStatePrivate::~DeclarativeBarsCachedStatePrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE

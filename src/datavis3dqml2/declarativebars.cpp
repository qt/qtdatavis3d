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
#include "bars3dshared_p.h"
#include "qdataset.h"

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
      m_cachedState(new DeclarativeBarsCachedStatePrivate())
{
    setFlags(QQuickItem::ItemHasContents);

    // TODO: Note; this does not flip the render result correctly. It is in mirror image.
    setRotation(180.0);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);
}

DeclarativeBars::~DeclarativeBars()
{
    delete m_shared;
}

QSGNode *DeclarativeBars::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    qDebug() << "Enter DeclarativeBars::updatePaintNode";
    // Delete old node and recreate it. This function gets called when window geometry changes.
    if (oldNode)
        delete oldNode;

    if (!m_shared)
        m_shared = new Bars3dRenderer(boundingRect().toRect());

    // Lazy initialization of shared object on the SGRenderThread
    m_shared->initializeOpenGL();

    // We need to create a node class that does the rendering (ie. a node that "captures" the rendering we do)
    qDebug() << "DeclarativeBars::updatePaintNode Creating new node";

    // Create the new render node
    DeclarativeBarsRenderer *node = new DeclarativeBarsRenderer(window(), m_shared);
    node->setRect(boundingRect());
    m_shared->setBoundingRect(boundingRect().toRect());

    if (m_cachedState->m_isSampleSpaceSet) {
        m_shared->setupSampleSpace(m_cachedState->m_samplesRow, m_cachedState->m_samplesColumn,
                                   m_cachedState->m_labelRow, m_cachedState->m_labelColumn,
                                   m_cachedState->m_labelHeight);
        m_cachedState->m_isSampleSpaceSet = false;
    }

    if (m_cachedState->m_dataRow) {
        m_shared->addDataRow(m_cachedState->m_dataRow);
        m_cachedState->m_dataRow = 0;
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

    qDebug() << "Exit DeclarativeBars::updatePaintNode";
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

void DeclarativeBars::setupSampleSpace(int samplesRow, int samplesColumn, const QString &labelRow,
                                       const QString &labelColumn, const QString &labelHeight)
{
    m_cachedState->m_samplesRow = samplesRow;
    m_cachedState->m_samplesColumn = samplesColumn;
    m_cachedState->m_labelRow = labelRow;
    m_cachedState->m_labelColumn = labelColumn;
    m_cachedState->m_labelHeight = labelHeight;
    m_cachedState->m_isSampleSpaceSet = true;
}

void DeclarativeBars::setCameraPreset(CameraPreset preset)
{
    m_shared->setCameraPreset(preset);
}

void DeclarativeBars::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    m_shared->setCameraPosition(horizontal, vertical, distance);
}

void DeclarativeBars::setTheme(ColorTheme theme)
{
    m_shared->setTheme(theme);
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
}

DeclarativeBars::LabelTransparency DeclarativeBars::labelTransparency()
{
    return DeclarativeBars::LabelTransparency(m_shared->labelTransparency());
}

void DeclarativeBars::setGridVisible(bool visible)
{
    m_cachedState->m_isGridEnabled = visible;
    m_cachedState->m_isGridSet = true;
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

void DeclarativeBars::setTickCount(GLint tickCount, GLfloat step, GLfloat minimum)
{
    m_shared->setTickCount(tickCount, step, minimum);
}

void DeclarativeBars::addData(QAbstractItemModel *data)
{
    QDataSet *dataset = new QDataSet();
    // TODO: Maps data in QAbstractItemModel to qdatarows and qdataset
    m_shared->addDataSet(dataset);
}

QAbstractItemModel *DeclarativeBars::data()
{
    // TODO: Map back from qdataset, or store as QAbstractItemModel as well?
    return NULL;
}

void DeclarativeBars::addDataRow(const QVector<float> &dataRow, const QString &labelRow,
                                 const QVector<QString> &labelsColumn)
{
    qDebug() << "Enter DeclarativeBars::addDataRow(const QVector<float> &dataRow...)";
    m_shared->addDataRow(dataRow, labelRow, labelsColumn);
}

void DeclarativeBars::addDataRow(const QVector<QDataItem*> &dataRow, const QString &labelRow,
                                 const QVector<QString> &labelsColumn)
{
    qDebug() << "Enter DeclarativeBars::addDataRow(const QVector<QDataItem*> &dataRow...)";
    m_shared->addDataRow(dataRow, labelRow, labelsColumn);
}

void DeclarativeBars::addDataRow(QDataRow *dataRow)
{
    qDebug() << "Enter DeclarativeBars::addDataRow(QDataRow *dataRow)";
    m_cachedState->m_dataRow = dataRow;
}

void DeclarativeBars::addDataSet(const QVector< QVector<float> > &data,
                                 const QVector<QString> &labelsRow,
                                 const QVector<QString> &labelsColumn)
{
    m_shared->addDataSet(data, labelsRow, labelsColumn);
}

void DeclarativeBars::addDataSet(const QVector< QVector<QDataItem*> > &data,
                                 const QVector<QString> &labelsRow,
                                 const QVector<QString> &labelsColumn)
{
    m_shared->addDataSet(data, labelsRow, labelsColumn);
}

void DeclarativeBars::addDataSet(QDataSet* dataSet)
{
    m_shared->addDataSet(dataSet);
}

void DeclarativeBars::setSelectionMode(DeclarativeBars::SelectionMode mode)
{
    m_cachedState->m_selectionMode = QtDataVis3D::SelectionMode(mode);
    m_cachedState->m_isSelectionModeSet = true;
}

DeclarativeBars::SelectionMode DeclarativeBars::selectionMode()
{
    return DeclarativeBars::SelectionMode(m_shared->selectionMode());
}

void DeclarativeBars::setShadowQuality(DeclarativeBars::ShadowQuality quality)
{
    m_cachedState->m_shadowQuality = QtDataVis3D::ShadowQuality(quality);
    m_cachedState->m_isShadowQualitySet = true;
}

DeclarativeBars::ShadowQuality DeclarativeBars::shadowQuality()
{
    return DeclarativeBars::ShadowQuality(m_shared->shadowQuality());
}

void DeclarativeBars::setMeshFileName(const QString &objFileName)
{
    m_shared->setMeshFileName(objFileName);
}




DeclarativeBarsRenderer::DeclarativeBarsRenderer(QQuickWindow *window, Bars3dRenderer *renderer)
    : m_fbo(0),
      m_texture(0),
      m_window(window),
      m_barsRenderer(renderer)
{
    qDebug() << "DeclarativeBarsRenderer::DeclarativeBarsRenderer()";
    connect(m_window, SIGNAL(beforeRendering()), this, SLOT(render()), Qt::DirectConnection);
    qDebug() << "QQuickWindow::openglContext()->thread()" << m_window->openglContext()->thread();
    qDebug() << "QGuiApplication::instance()->thread()" << QGuiApplication::instance()->thread();
}

DeclarativeBarsRenderer::~DeclarativeBarsRenderer()
{
    delete m_texture;
    delete m_fbo;
}

void DeclarativeBarsRenderer::render()
{
    static bool firstRender = true;
    // TODO: Remove if once done testing
    if (firstRender)
        qDebug() << "DeclarativeBarsRenderer::render() running on thread " << QThread::currentThread();
    firstRender = false;

    QSize size = rect().size().toSize();

    if (!m_fbo) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        m_fbo = new QOpenGLFramebufferObject(size, format);
        m_texture = m_window->createTextureFromId(m_fbo->texture(), size);
        setTexture(m_texture);
    }

    m_fbo->bind();

    // SGRendering State resets between calls...
    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Emulate mouse movement
    // TODO: Remove this and implement properly
    static int rot = 0;
    rot += 5;
    if (rot > 2870) rot = 0;
    m_barsRenderer->m_mousePos.setX(rot);
    m_barsRenderer->m_mousePos.setY(100);

    // Call the shared rendering function
    m_barsRenderer->render();

    m_fbo->bindDefault();

    m_window->update();
}


DeclarativeBarsCachedStatePrivate::DeclarativeBarsCachedStatePrivate() :
    m_isSampleSpaceSet(false),
    m_labelRow(QStringLiteral("")),
    m_labelColumn(QStringLiteral("")),
    m_labelHeight(QStringLiteral("")),
    m_dataRow(0)
{
}

DeclarativeBarsCachedStatePrivate::~DeclarativeBarsCachedStatePrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE

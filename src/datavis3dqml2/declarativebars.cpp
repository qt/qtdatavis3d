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

#include "declarativebars_p.h"
#include "declarativebarsrenderer_p.h"
#include "bars3dcontroller_p.h"
#include "qitemmodelbardataproxy.h"
#include "qvalueaxis.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

DeclarativeBars::DeclarativeBars(QQuickItem *parent)
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
    m_shared->setupSampleSpace(samplesRow, samplesColumn);
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
    m_shared->setLabelTransparency(QtDataVis3D::LabelTransparency(transparency));
}

DeclarativeBars::LabelTransparency DeclarativeBars::labelTransparency()
{
    return DeclarativeBars::LabelTransparency(m_shared->labelTransparency());
}

void DeclarativeBars::setGridVisible(bool visible)
{
    m_shared->setGridEnabled(visible);
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
void DeclarativeBars::setSegmentCount(int segmentCount, qreal step, qreal minimum)
{
    QValueAxis *axis = static_cast<QValueAxis *>(m_shared->axisY());
    if (axis) {
        axis->setSegmentCount(segmentCount);
        axis->setRange(minimum, segmentCount * step);
    }
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
    m_shared->setSelectionMode(QtDataVis3D::SelectionMode(mode));
}

DeclarativeBars::SelectionMode DeclarativeBars::selectionMode()
{
    return DeclarativeBars::SelectionMode(m_shared->selectionMode());
}

void DeclarativeBars::setShadowQuality(DeclarativeBars::ShadowQuality quality)
{
    m_shared->setShadowQuality(QtDataVis3D::ShadowQuality(quality));
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
    static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->setMapping(mapping);
}

int DeclarativeBars::rows() const
{
    return m_shared->rowCount();
}

void DeclarativeBars::setRows(int rows)
{
    setupSampleSpace(rows, columns());
}

int DeclarativeBars::columns() const
{
    return m_shared->columnCount();
}

void DeclarativeBars::setColumns(int columns)
{
    setupSampleSpace(rows(), columns);
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

QT_DATAVIS3D_END_NAMESPACE

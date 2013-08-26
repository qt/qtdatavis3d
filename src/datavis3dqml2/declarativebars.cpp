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

#include "declarativebars_p.h"
#include "declarativebarsrenderer_p.h"
#include "qitemmodelbardataproxy.h"
#include "qvalueaxis.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

const QString smoothString(QStringLiteral("Smooth"));

DeclarativeBars::DeclarativeBars(QQuickItem *parent)
    : QQuickItem(parent),
      m_shared(0),
      m_initialisedSize(0, 0),
      m_cameraPreset(QDataVis::NoPreset),
      m_theme(QDataVis::ThemeDefault)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_shared = new Bars3dController(boundingRect().toRect());
    QObject::connect(m_shared, &Abstract3DController::shadowQualityChanged, this,
                     &DeclarativeBars::handleShadowQualityUpdate);

    QItemModelBarDataProxy *proxy = new QItemModelBarDataProxy;
    m_shared->setDataProxy(proxy);

    QObject::connect(proxy, &QBarDataProxy::arrayReset, this,
                     &DeclarativeBars::dataResolved);
}

DeclarativeBars::~DeclarativeBars()
{
    delete m_shared;
}

void DeclarativeBars::handleShadowQualityUpdate(QDataVis::ShadowQuality quality)
{
    emit shadowQualityChanged(quality);
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

void DeclarativeBars::setDataWindow(int rowCount, int columnCount)
{
    m_shared->setDataWindow(rowCount, columnCount);
}

void DeclarativeBars::setBarColor(const QColor &baseColor, const QColor &heightColor,
                                  const QColor &depthColor, bool uniform)
{
    m_shared->setObjectColor(baseColor, heightColor, depthColor, uniform);
}

void DeclarativeBars::setCameraPosition(qreal horizontal, qreal vertical, int distance)
{
    m_shared->setCameraPosition(GLfloat(horizontal), GLfloat(vertical), GLint(distance));
}

void DeclarativeBars::setData(QAbstractItemModel *data)
{
    static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->setItemModel(data);
}

QAbstractItemModel *DeclarativeBars::data()
{
    return static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->itemModel();
}

void DeclarativeBars::setMapping(QItemModelBarDataMapping *mapping)
{
    static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->setMapping(mapping);
}

QCategoryAxis *DeclarativeBars::axisX() const
{
    return static_cast<QCategoryAxis *>(m_shared->axisX());
}

void DeclarativeBars::setAxisX(QCategoryAxis *axis)
{
    m_shared->setAxisX(axis);
}

QValueAxis *DeclarativeBars::axisY() const
{
    return static_cast<QValueAxis *>(m_shared->axisY());
}

void DeclarativeBars::setAxisY(QValueAxis *axis)
{
    m_shared->setAxisY(axis);
}

QCategoryAxis *DeclarativeBars::axisZ() const
{
    return static_cast<QCategoryAxis *>(m_shared->axisZ());
}

void DeclarativeBars::setAxisZ(QCategoryAxis *axis)
{
    m_shared->setAxisZ(axis);
}

QItemModelBarDataMapping *DeclarativeBars::mapping() const
{
    return static_cast<QItemModelBarDataProxy *>(m_shared->dataProxy())->mapping();
}

void DeclarativeBars::setBarThickness(qreal thicknessRatio)
{
    m_shared->setBarSpecs(GLfloat(thicknessRatio), barSpacing(), isBarSpacingRelative());
}

qreal DeclarativeBars::barThickness()
{
    return m_shared->barThickness();
}

void DeclarativeBars::setBarSpacing(QSizeF spacing)
{
    m_shared->setBarSpecs(GLfloat(barThickness()), spacing, isBarSpacingRelative());
}

QSizeF DeclarativeBars::barSpacing()
{
    return m_shared->barSpacing();
}

void DeclarativeBars::setBarSpacingRelative(bool relative)
{
    m_shared->setBarSpecs(GLfloat(barThickness()), barSpacing(), relative);
}

bool DeclarativeBars::isBarSpacingRelative()
{
    return m_shared->isBarSpecRelative();
}

void DeclarativeBars::setBarType(QDataVis::MeshStyle style)
{
    QString objFile = m_shared->meshFileName();
    bool smooth = objFile.endsWith(smoothString);
    m_shared->setBarType(style, smooth);
}

QDataVis::MeshStyle DeclarativeBars::barType()
{
    QString objFile = m_shared->meshFileName();
    if (objFile.contains("/sphere"))
        return QDataVis::Spheres;
    else
        return QDataVis::Dots;
}

void DeclarativeBars::setBarSmooth(bool smooth)
{
    QString objFile = m_shared->meshFileName();
    if (objFile.endsWith(smoothString)) {
        if (smooth)
            return; // Already smooth; do nothing
        else // Rip Smooth off the end
            objFile.resize(objFile.indexOf(smoothString));
    } else {
        if (!smooth) // Already flat; do nothing
            return;
        else // Append Smooth to the end
            objFile.append(smoothString);
    }
    m_shared->setMeshFileName(objFile);
}

bool DeclarativeBars::barSmooth()
{
    QString objFile = m_shared->meshFileName();
    return objFile.endsWith(smoothString);
}

void DeclarativeBars::setMeshFileName(const QString &objFileName)
{
    m_shared->setMeshFileName(objFileName);
}

QString DeclarativeBars::meshFileName()
{
    return m_shared->meshFileName();
}

void DeclarativeBars::setCameraPreset(QDataVis::CameraPreset preset)
{
    // TODO: Implement correctly once "improved camera api" (QTRD-2122) is implemented
    // We need to save this locally, as there are no getters for it in controller
    m_cameraPreset = preset;
    m_shared->setCameraPreset(preset);
}

QDataVis::CameraPreset DeclarativeBars::cameraPreset()
{
    return m_cameraPreset;
}

void DeclarativeBars::setTheme(QDataVis::ColorTheme theme)
{
    // TODO: Implement correctly once "user-modifiable themes" (QTRD-2120) is implemented
    // We need to save this locally, as there are no getters for it in controller
    m_theme = theme;
    m_shared->setColorTheme(theme);
}

QDataVis::ColorTheme DeclarativeBars::theme()
{
    return m_theme;
}

void DeclarativeBars::setFont(const QFont &font)
{
    m_shared->setFont(font);
}

QFont DeclarativeBars::font()
{
    return m_shared->font();
}

void DeclarativeBars::setLabelTransparency(QDataVis::LabelTransparency transparency)
{
    m_shared->setLabelTransparency(transparency);
}

QDataVis::LabelTransparency DeclarativeBars::labelTransparency()
{
    return m_shared->labelTransparency();
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

void DeclarativeBars::setSelectionMode(QDataVis::SelectionMode mode)
{
    m_shared->setSelectionMode(mode);
}

QDataVis::SelectionMode DeclarativeBars::selectionMode()
{
    return m_shared->selectionMode();
}

void DeclarativeBars::setShadowQuality(QDataVis::ShadowQuality quality)
{
    m_shared->setShadowQuality(quality);
}

QDataVis::ShadowQuality DeclarativeBars::shadowQuality()
{
    return m_shared->shadowQuality();
}

int DeclarativeBars::rows() const
{
    return m_shared->rowCount();
}

void DeclarativeBars::setRows(int rows)
{
    setDataWindow(rows, columns());
}

int DeclarativeBars::columns() const
{
    return m_shared->columnCount();
}

void DeclarativeBars::setColumns(int columns)
{
    setDataWindow(rows(), columns);
}

void DeclarativeBars::setItemLabelFormat(const QString &format)
{
    m_shared->dataProxy()->setItemLabelFormat(format);
}

QString DeclarativeBars::itemLabelFormat()
{
    return m_shared->dataProxy()->itemLabelFormat();
}

void DeclarativeBars::setSelectedBarPos(const QPoint &position)
{
    m_shared->setSelectedBarPos(position);
}

QPoint DeclarativeBars::selectedBarPos() const
{
    return m_shared->selectedBarPos();
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

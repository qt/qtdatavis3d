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

#include "declarativebars_p.h"
#include "declarativebarsrenderer_p.h"
#include "q3dvalueaxis.h"
#include "qitemmodelbardataproxy.h"
#include "theme_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

const QString smoothString(QStringLiteral("Smooth"));

DeclarativeBars::DeclarativeBars(QQuickItem *parent)
    : AbstractDeclarative(parent),
      m_shared(0),
      m_initialisedSize(0, 0)
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_shared = new Bars3DController(boundingRect().toRect());
    AbstractDeclarative::setSharedController(m_shared);
    QObject::connect(m_shared, &Bars3DController::selectedBarChanged, this,
                     &DeclarativeBars::selectedBarChanged);

    QItemModelBarDataProxy *proxy = new QItemModelBarDataProxy;
    m_shared->setActiveDataProxy(proxy);
}

DeclarativeBars::~DeclarativeBars()
{
    delete m_shared;
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

void DeclarativeBars::setBarColor(const QColor &baseColor, bool uniform)
{
    m_shared->setObjectColor(baseColor, uniform);
}

void DeclarativeBars::setDataProxy(QBarDataProxy *dataProxy)
{
    m_shared->setActiveDataProxy(dataProxy);
}

QBarDataProxy *DeclarativeBars::dataProxy() const
{
    return static_cast<QBarDataProxy *>(m_shared->activeDataProxy());
}

Q3DCategoryAxis *DeclarativeBars::rowAxis() const
{
    return static_cast<Q3DCategoryAxis *>(m_shared->axisX());
}

void DeclarativeBars::setRowAxis(Q3DCategoryAxis *axis)
{
    m_shared->setAxisX(axis);
}

Q3DValueAxis *DeclarativeBars::valueAxis() const
{
    return static_cast<Q3DValueAxis *>(m_shared->axisY());
}

void DeclarativeBars::setValueAxis(Q3DValueAxis *axis)
{
    m_shared->setAxisY(axis);
}

Q3DCategoryAxis *DeclarativeBars::columnAxis() const
{
    return static_cast<Q3DCategoryAxis *>(m_shared->axisZ());
}

void DeclarativeBars::setColumnAxis(Q3DCategoryAxis *axis)
{
    m_shared->setAxisZ(axis);
}

void DeclarativeBars::setBarThickness(qreal thicknessRatio)
{
    m_shared->setBarSpecs(GLfloat(thicknessRatio), barSpacing(), isBarSpacingRelative());
}

qreal DeclarativeBars::barThickness() const
{
    return m_shared->barThickness();
}

void DeclarativeBars::setBarSpacing(QSizeF spacing)
{
    m_shared->setBarSpecs(GLfloat(barThickness()), spacing, isBarSpacingRelative());
}

QSizeF DeclarativeBars::barSpacing() const
{
    return m_shared->barSpacing();
}

void DeclarativeBars::setBarSpacingRelative(bool relative)
{
    m_shared->setBarSpecs(GLfloat(barThickness()), barSpacing(), relative);
}

bool DeclarativeBars::isBarSpacingRelative() const
{
    return m_shared->isBarSpecRelative();
}

void DeclarativeBars::setBarType(QDataVis::MeshStyle style)
{
    QString objFile = m_shared->meshFileName();
    bool smooth = objFile.endsWith(smoothString);
    m_shared->setBarType(style, smooth);
}

QDataVis::MeshStyle DeclarativeBars::barType() const
{
    QString objFile = m_shared->meshFileName();
    if (objFile.contains("/sphere"))
        return QDataVis::MeshStyleSpheres;
    else
        return QDataVis::MeshStyleDots;
}

void DeclarativeBars::setBarSmoothingEnabled(bool enabled)
{
    QString objFile = m_shared->meshFileName();
    if (objFile.endsWith(smoothString)) {
        if (enabled)
            return; // Already smooth; do nothing
        else // Rip Smooth off the end
            objFile.resize(objFile.indexOf(smoothString));
    } else {
        if (!enabled) // Already flat; do nothing
            return;
        else // Append Smooth to the end
            objFile.append(smoothString);
    }
    m_shared->setMeshFileName(objFile);
}

bool DeclarativeBars::isBarSmoothingEnabled() const
{
    QString objFile = m_shared->meshFileName();
    return objFile.endsWith(smoothString);
}

void DeclarativeBars::setMeshFileName(const QString &objFileName)
{
    m_shared->setMeshFileName(objFileName);
}

QString DeclarativeBars::meshFileName() const
{
    return m_shared->meshFileName();
}

void DeclarativeBars::setSelectedBar(const QPointF &position)
{
    m_shared->setSelectedBar(position.toPoint());
}

QPointF DeclarativeBars::selectedBar() const
{
    return QPointF(m_shared->selectedBar());
}

QT_DATAVISUALIZATION_END_NAMESPACE

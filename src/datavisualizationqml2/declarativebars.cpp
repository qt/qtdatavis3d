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
    QObject::connect(m_shared, &Abstract3DController::meshFileNameChanged, this,
                     &DeclarativeBars::meshFileNameChanged);
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

void DeclarativeBars::setBarColor(const QColor &baseColor)
{
    m_shared->setObjectColor(baseColor);
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

void DeclarativeBars::setBarThickness(float thicknessRatio)
{
    if (thicknessRatio != barThickness()) {
        m_shared->setBarSpecs(GLfloat(thicknessRatio), barSpacing(), isBarSpacingRelative());
        emit barThicknessChanged(thicknessRatio);
    }
}

float DeclarativeBars::barThickness() const
{
    return m_shared->barThickness();
}

void DeclarativeBars::setBarSpacing(QSizeF spacing)
{
    if (spacing != barSpacing()) {
        m_shared->setBarSpecs(GLfloat(barThickness()), spacing, isBarSpacingRelative());
        emit barSpacingChanged(spacing);
    }
}

QSizeF DeclarativeBars::barSpacing() const
{
    return m_shared->barSpacing();
}

void DeclarativeBars::setBarSpacingRelative(bool relative)
{
    if (relative != isBarSpacingRelative()) {
        m_shared->setBarSpecs(GLfloat(barThickness()), barSpacing(), relative);
        emit barSpacingRelativeChanged(relative);
    }
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

QQmlListProperty<QBar3DSeries> DeclarativeBars::seriesList()
{
    return QQmlListProperty<QBar3DSeries>(this, this,
                                          &DeclarativeBars::appendSeriesFunc,
                                          &DeclarativeBars::countSeriesFunc,
                                          &DeclarativeBars::atSeriesFunc,
                                          &DeclarativeBars::clearSeriesFunc);
}

void DeclarativeBars::appendSeriesFunc(QQmlListProperty<QBar3DSeries> *list, QBar3DSeries *series)
{
    reinterpret_cast<DeclarativeBars *>(list->data)->addSeries(series);
}

int DeclarativeBars::countSeriesFunc(QQmlListProperty<QBar3DSeries> *list)
{
    return reinterpret_cast<DeclarativeBars *>(list->data)->m_shared->barSeriesList().size();
}

QBar3DSeries *DeclarativeBars::atSeriesFunc(QQmlListProperty<QBar3DSeries> *list, int index)
{
    return reinterpret_cast<DeclarativeBars *>(list->data)->m_shared->barSeriesList().at(index);
}

void DeclarativeBars::clearSeriesFunc(QQmlListProperty<QBar3DSeries> *list)
{
    DeclarativeBars *declBars = reinterpret_cast<DeclarativeBars *>(list->data);
    QList<QBar3DSeries *> realList = declBars->m_shared->barSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declBars->removeSeries(realList.at(i));
}

void DeclarativeBars::addSeries(QBar3DSeries *series)
{
    m_shared->addSeries(series);
}

void DeclarativeBars::removeSeries(QBar3DSeries *series)
{
    m_shared->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

QT_DATAVISUALIZATION_END_NAMESPACE

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
#include "qvalue3daxis.h"
#include "qitemmodelbardataproxy.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeBars::DeclarativeBars(QQuickItem *parent)
    : AbstractDeclarative(parent),
      m_barsController(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);

    // TODO: These seem to have no effect; find a way to activate anti-aliasing
    setAntialiasing(true);
    setSmooth(true);

    // Create the shared component on the main GUI thread.
    m_barsController = new Bars3DController(boundingRect().toRect());
    AbstractDeclarative::setSharedController(m_barsController);

    // TODO: Uncomment when doing QTRD-2669
//    connect(m_barsController, &Bars3DController::rowAxisChanged,
//            this, &DeclarativeBars::rowAxisChanged);
//    connect(m_barsController, &Bars3DController::valueAxisChanged,
//            this, &DeclarativeBars::valueAxisChanged);
//    connect(m_barsController, &Bars3DController::columnAxisChanged,
//            this, &DeclarativeBars::columnAxisChanged);
}

DeclarativeBars::~DeclarativeBars()
{
    delete m_barsController;
}

QCategory3DAxis *DeclarativeBars::rowAxis() const
{
    return static_cast<QCategory3DAxis *>(m_barsController->axisZ());
}

void DeclarativeBars::setRowAxis(QCategory3DAxis *axis)
{
    m_barsController->setAxisZ(axis);
}

QValue3DAxis *DeclarativeBars::valueAxis() const
{
    return static_cast<QValue3DAxis *>(m_barsController->axisY());
}

void DeclarativeBars::setValueAxis(QValue3DAxis *axis)
{
    m_barsController->setAxisY(axis);
}

QCategory3DAxis *DeclarativeBars::columnAxis() const
{
    return static_cast<QCategory3DAxis *>(m_barsController->axisX());
}

void DeclarativeBars::setColumnAxis(QCategory3DAxis *axis)
{
    m_barsController->setAxisX(axis);
}

void DeclarativeBars::setBarThickness(float thicknessRatio)
{
    if (thicknessRatio != barThickness()) {
        m_barsController->setBarSpecs(GLfloat(thicknessRatio), barSpacing(), isBarSpacingRelative());
        emit barThicknessChanged(thicknessRatio);
    }
}

float DeclarativeBars::barThickness() const
{
    return m_barsController->barThickness();
}

void DeclarativeBars::setBarSpacing(QSizeF spacing)
{
    if (spacing != barSpacing()) {
        m_barsController->setBarSpecs(GLfloat(barThickness()), spacing, isBarSpacingRelative());
        emit barSpacingChanged(spacing);
    }
}

QSizeF DeclarativeBars::barSpacing() const
{
    return m_barsController->barSpacing();
}

void DeclarativeBars::setBarSpacingRelative(bool relative)
{
    if (relative != isBarSpacingRelative()) {
        m_barsController->setBarSpecs(GLfloat(barThickness()), barSpacing(), relative);
        emit barSpacingRelativeChanged(relative);
    }
}

bool DeclarativeBars::isBarSpacingRelative() const
{
    return m_barsController->isBarSpecRelative();
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
    return reinterpret_cast<DeclarativeBars *>(list->data)->m_barsController->barSeriesList().size();
}

QBar3DSeries *DeclarativeBars::atSeriesFunc(QQmlListProperty<QBar3DSeries> *list, int index)
{
    return reinterpret_cast<DeclarativeBars *>(list->data)->m_barsController->barSeriesList().at(index);
}

void DeclarativeBars::clearSeriesFunc(QQmlListProperty<QBar3DSeries> *list)
{
    DeclarativeBars *declBars = reinterpret_cast<DeclarativeBars *>(list->data);
    QList<QBar3DSeries *> realList = declBars->m_barsController->barSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declBars->removeSeries(realList.at(i));
}

void DeclarativeBars::addSeries(QBar3DSeries *series)
{
    m_barsController->addSeries(series);
}

void DeclarativeBars::removeSeries(QBar3DSeries *series)
{
    m_barsController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

QT_DATAVISUALIZATION_END_NAMESPACE

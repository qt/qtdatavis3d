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

#include "declarativetheme_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

DeclarativeTheme3D::DeclarativeTheme3D(QObject *parent)
    : Q3DTheme(parent),
      m_baseGradient(0),
      m_singleHLGradient(0),
      m_multiHLGradient(0)
{
}

DeclarativeTheme3D::~DeclarativeTheme3D()
{
}

QQmlListProperty<QObject> DeclarativeTheme3D::seriesChildren()
{
    return QQmlListProperty<QObject>(this, this, &DeclarativeTheme3D::appendSeriesChildren
                                     , 0, 0, 0);
}

void DeclarativeTheme3D::appendSeriesChildren(QQmlListProperty<QObject> *list, QObject *element)
{
    Q_UNUSED(list)
    Q_UNUSED(element)
    // Nothing to do, seriesChildren is there only to enable scoping gradient items in Theme3D item.
}

void DeclarativeTheme3D::handleBaseGradientUpdate()
{
    if (m_baseGradient)
        setThemeGradient(*m_baseGradient, GradientTypeBase);
}

void DeclarativeTheme3D::handleSingleHLGradientUpdate()
{
    if (m_singleHLGradient)
        setThemeGradient(*m_singleHLGradient, GradientTypeSingleHL);
}

void DeclarativeTheme3D::handleMultiHLGradientUpdate()
{
    if (m_multiHLGradient)
        setThemeGradient(*m_multiHLGradient, GradientTypeMultiHL);
}

void DeclarativeTheme3D::setBaseGradient(ColorGradient *gradient)
{
    // connect new / disconnect old
    if (gradient != m_baseGradient) {
        if (m_baseGradient)
            QObject::disconnect(m_baseGradient, 0, this, 0);

        m_baseGradient = gradient;

        if (m_baseGradient) {
            QObject::connect(m_baseGradient, &ColorGradient::updated, this,
                             &DeclarativeTheme3D::handleBaseGradientUpdate);
        }

        emit baseGradientChanged(m_baseGradient);
    }

    if (m_baseGradient)
        setThemeGradient(*m_baseGradient, GradientTypeBase);
}

ColorGradient *DeclarativeTheme3D::baseGradient() const
{
    return m_baseGradient;
}

void DeclarativeTheme3D::setSingleHighlightGradient(ColorGradient *gradient)
{
    // connect new / disconnect old
    if (gradient != m_singleHLGradient) {
        if (m_singleHLGradient)
            QObject::disconnect(m_singleHLGradient, 0, this, 0);

        m_singleHLGradient = gradient;

        if (m_singleHLGradient) {
            QObject::connect(m_singleHLGradient, &ColorGradient::updated, this,
                             &DeclarativeTheme3D::handleSingleHLGradientUpdate);
        }

        emit singleHighlightGradientChanged(m_singleHLGradient);
    }

    if (m_singleHLGradient)
        setThemeGradient(*m_singleHLGradient, GradientTypeSingleHL);
}

ColorGradient *DeclarativeTheme3D::singleHighlightGradient() const
{
    return m_singleHLGradient;
}

void DeclarativeTheme3D::setMultiHighlightGradient(ColorGradient *gradient)
{
    // connect new / disconnect old
    if (gradient != m_multiHLGradient) {
        if (m_multiHLGradient)
            QObject::disconnect(m_multiHLGradient, 0, this, 0);

        m_multiHLGradient = gradient;

        if (m_multiHLGradient) {
            QObject::connect(m_multiHLGradient, &ColorGradient::updated, this,
                             &DeclarativeTheme3D::handleMultiHLGradientUpdate);
        }

        emit multiHighlightGradientChanged(m_multiHLGradient);
    }

    if (m_multiHLGradient)
        setThemeGradient(*m_baseGradient, GradientTypeMultiHL);
}

ColorGradient *DeclarativeTheme3D::multiHighlightGradient() const
{
    return m_multiHLGradient;
}

void DeclarativeTheme3D::setThemeGradient(const ColorGradient &gradient, GradientType type)
{
    QLinearGradient newGradient;
    QGradientStops stops;
    QList<ColorGradientStop *> qmlstops = gradient.m_stops;

    // Get sorted gradient stops
    for (int i = 0; i < qmlstops.size(); i++) {
        int j = 0;
        while (j < stops.size() && stops.at(j).first < qmlstops[i]->position())
            j++;
        stops.insert(j, QGradientStop(qmlstops.at(i)->position(), qmlstops.at(i)->color()));
    }

    newGradient.setStops(stops);
    switch (type) {
    case GradientTypeBase:
        Q3DTheme::setBaseGradient(newGradient);
        break;
    case GradientTypeSingleHL:
        Q3DTheme::setSingleHighlightGradient(newGradient);
        break;
    case GradientTypeMultiHL:
        break;
        Q3DTheme::setMultiHighlightGradient(newGradient);
    default:
        break;
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE

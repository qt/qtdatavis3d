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
      m_gradients(QList<ColorGradient *>()),
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

void DeclarativeTheme3D::handleSingleHLGradientUpdate()
{
    if (m_singleHLGradient)
        setThemeGradient(m_singleHLGradient, GradientTypeSingleHL);
}

void DeclarativeTheme3D::handleMultiHLGradientUpdate()
{
    if (m_multiHLGradient)
        setThemeGradient(m_multiHLGradient, GradientTypeMultiHL);
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
        setThemeGradient(m_singleHLGradient, GradientTypeSingleHL);
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
        setThemeGradient(m_multiHLGradient, GradientTypeMultiHL);
}

ColorGradient *DeclarativeTheme3D::multiHighlightGradient() const
{
    return m_multiHLGradient;
}

void DeclarativeTheme3D::setThemeGradient(ColorGradient *gradient, GradientType type)
{
    QLinearGradient newGradient = convertGradient(gradient);

    switch (type) {
    case GradientTypeSingleHL:
        Q3DTheme::setSingleHighlightGradient(newGradient);
        break;
    case GradientTypeMultiHL:
        break;
        Q3DTheme::setMultiHighlightGradient(newGradient);
    default:
        qWarning("Incorrect usage. Type may be GradientTypeSingleHL or GradientTypeMultiHL.");
        break;
    }
}

QLinearGradient DeclarativeTheme3D::convertGradient(ColorGradient *gradient)
{
    QLinearGradient newGradient;
    QGradientStops stops;
    QList<ColorGradientStop *> qmlstops = gradient->m_stops;

    // Get sorted gradient stops
    for (int i = 0; i < qmlstops.size(); i++) {
        int j = 0;
        while (j < stops.size() && stops.at(j).first < qmlstops[i]->position())
            j++;
        stops.insert(j, QGradientStop(qmlstops.at(i)->position(), qmlstops.at(i)->color()));
    }

    newGradient.setStops(stops);

    return newGradient;
}

//void DeclarativeTheme3D::addColor(const DeclarativeColor &color)
//{
//    QList<QColor> list = Q3DTheme::baseColors();
//    list.append(color);
//    Q3DTheme::setBaseColors(list);
//}

//QList<DeclarativeColor> DeclarativeTheme3D::colorList() const
//{
//    return Q3DTheme::baseColors();
//}

//void DeclarativeTheme3D::clearColors()
//{
//    Q3DTheme::setBaseColors(QList<QColor>());
//}

void DeclarativeTheme3D::addGradient(ColorGradient *gradient)
{
    m_gradients.append(gradient);
    QList<QLinearGradient> list = Q3DTheme::baseGradients();
    list.append(convertGradient(gradient));
    Q3DTheme::setBaseGradients(list);
}

QList<ColorGradient *> DeclarativeTheme3D::gradientList() const
{
    return m_gradients;
}

void DeclarativeTheme3D::clearGradients()
{
    m_gradients.clear();
    Q3DTheme::setBaseGradients(QList<QLinearGradient>());
}

//QQmlListProperty<DeclarativeColor> DeclarativeTheme3D::baseColors()
//{
//    return QQmlListProperty<DeclarativeColor>(this, this,
//                                    &DeclarativeTheme3D::appendBaseColorsFunc,
//                                    &DeclarativeTheme3D::countBaseColorsFunc,
//                                    &DeclarativeTheme3D::atBaseColorsFunc,
//                                    &DeclarativeTheme3D::clearBaseColorsFunc);
//}

//void DeclarativeTheme3D::appendBaseColorsFunc(QQmlListProperty<DeclarativeColor> *list, DeclarativeColor *color)
//{
//    reinterpret_cast<DeclarativeTheme3D *>(list->data)->addColor(*color);
//}

//int DeclarativeTheme3D::countBaseColorsFunc(QQmlListProperty<DeclarativeColor> *list)
//{
//    return reinterpret_cast<DeclarativeTheme3D *>(list->data)->colorList().size();
//}

//DeclarativeColor *DeclarativeTheme3D::atBaseColorsFunc(QQmlListProperty<DeclarativeColor> *list, int index)
//{
//    return &(reinterpret_cast<DeclarativeTheme3D *>(list->data)->colorList()[index]);
//}

//void DeclarativeTheme3D::clearBaseColorsFunc(QQmlListProperty<DeclarativeColor> *list)
//{
//    reinterpret_cast<DeclarativeTheme3D *>(list->data)->clearGradients();
//}

QQmlListProperty<ColorGradient> DeclarativeTheme3D::baseGradients()
{
    return QQmlListProperty<ColorGradient>(this, this,
                                           &DeclarativeTheme3D::appendBaseGradientsFunc,
                                           &DeclarativeTheme3D::countBaseGradientsFunc,
                                           &DeclarativeTheme3D::atBaseGradientsFunc,
                                           &DeclarativeTheme3D::clearBaseGradientsFunc);
}

void DeclarativeTheme3D::appendBaseGradientsFunc(QQmlListProperty<ColorGradient> *list,
                                                 ColorGradient *gradient)
{
    reinterpret_cast<DeclarativeTheme3D *>(list->data)->addGradient(gradient);
}

int DeclarativeTheme3D::countBaseGradientsFunc(QQmlListProperty<ColorGradient> *list)
{
    return reinterpret_cast<DeclarativeTheme3D *>(list->data)->gradientList().size();
}

ColorGradient *DeclarativeTheme3D::atBaseGradientsFunc(QQmlListProperty<ColorGradient> *list,
                                                       int index)
{
    return reinterpret_cast<DeclarativeTheme3D *>(list->data)->gradientList().at(index);
}

void DeclarativeTheme3D::clearBaseGradientsFunc(QQmlListProperty<ColorGradient> *list)
{
    reinterpret_cast<DeclarativeTheme3D *>(list->data)->clearGradients();
}

QT_DATAVISUALIZATION_END_NAMESPACE

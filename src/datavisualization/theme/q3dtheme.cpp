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

#include "q3dtheme_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Q3DTheme::Q3DTheme(QObject *parent)
    : QObject(parent),
      d_ptr(new Q3DThemePrivate(this, QDataVis::ThemeUserDefined))
{
}

Q3DTheme::Q3DTheme(QDataVis::Theme themeType, QObject *parent)
    : QObject(parent),
      d_ptr(new Q3DThemePrivate(this, themeType))
{
}

Q3DTheme::Q3DTheme(Q3DThemePrivate *d, QDataVis::Theme themeType,
                   QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
    d_ptr->m_themeId = themeType;
}

Q3DTheme::~Q3DTheme()
{

}

// TODO: Basecolors as a list, containing one for each series?
void Q3DTheme::setBaseColor(const QColor &color)
{
    if (d_ptr->m_baseColor != color) {
        d_ptr->m_dirtyBits.baseColorDirty = true;
        d_ptr->m_baseColor = color;
        emit baseColorChanged(color);
    }
}

QColor Q3DTheme::baseColor() const
{
    return d_ptr->m_baseColor;
}

void Q3DTheme::setBackgroundColor(const QColor &color)
{
    if (d_ptr->m_backgroundColor != color) {
        d_ptr->m_dirtyBits.backgroundColorDirty = true;
        d_ptr->m_backgroundColor = color;
        emit backgroundColorChanged(color);
    }
}

QColor Q3DTheme::backgroundColor() const
{
    return d_ptr->m_backgroundColor;
}

void Q3DTheme::setWindowColor(const QColor &color)
{
    if (d_ptr->m_windowColor != color) {
        d_ptr->m_dirtyBits.windowColorDirty = true;
        d_ptr->m_windowColor = color;
        emit windowColorChanged(color);
    }
}

QColor Q3DTheme::windowColor() const
{
    return d_ptr->m_windowColor;
}

void Q3DTheme::setTextColor(const QColor &color)
{
    if (d_ptr->m_textColor != color) {
        d_ptr->m_dirtyBits.textColorDirty = true;
        d_ptr->m_textColor = color;
        emit textColorChanged(color);
    }
}

QColor Q3DTheme::textColor() const
{
    return d_ptr->m_textColor;
}

void Q3DTheme::setTextBackgroundColor(const QColor &color)
{
    if (d_ptr->m_textBackgroundColor != color) {
        d_ptr->m_dirtyBits.textBackgroundColorDirty = true;
        d_ptr->m_textBackgroundColor = color;
        emit textBackgroundColorChanged(color);
    }
}

QColor Q3DTheme::textBackgroundColor() const
{
    return d_ptr->m_textBackgroundColor;
}

void Q3DTheme::setGridLineColor(const QColor &color)
{
    if (d_ptr->m_gridLineColor != color) {
        d_ptr->m_dirtyBits.gridLineColorDirty = true;
        d_ptr->m_gridLineColor = color;
        emit gridLineColorChanged(color);
    }
}

QColor Q3DTheme::gridLineColor() const
{
    return d_ptr->m_gridLineColor;
}

void Q3DTheme::setSingleHighlightColor(const QColor &color)
{
    if (d_ptr->m_singleHighlightColor != color) {
        d_ptr->m_dirtyBits.singleHighlightColorDirty = true;
        d_ptr->m_singleHighlightColor = color;
        emit singleHighlightColorChanged(color);
    }
}

QColor Q3DTheme::singleHighlightColor() const
{
    return d_ptr->m_singleHighlightColor;
}

void Q3DTheme::setMultiHighlightColor(const QColor &color)
{
    if (d_ptr->m_multiHighlightColor != color) {
        d_ptr->m_dirtyBits.multiHighlightColorDirty = true;
        d_ptr->m_multiHighlightColor = color;
        emit multiHighlightColorChanged(color);
    }
}

QColor Q3DTheme::multiHighlightColor() const
{
    return d_ptr->m_multiHighlightColor;
}

void Q3DTheme::setLightColor(const QColor &color)
{
    if (d_ptr->m_lightColor != color) {
        d_ptr->m_dirtyBits.lightColorDirty = true;
        d_ptr->m_lightColor = color;
        emit lightColorChanged(color);
    }
}

QColor Q3DTheme::lightColor() const
{
    return d_ptr->m_lightColor;
}

// TODO: Surfacegradients as a list, containing one for each series?
void Q3DTheme::setBaseGradient(const QLinearGradient &gradient)
{
    if (d_ptr->m_baseGradient != gradient) {
        d_ptr->m_dirtyBits.baseGradientDirty = true;
        d_ptr->m_baseGradient = gradient;
        emit baseGradientChanged(gradient);
    }
}

QLinearGradient Q3DTheme::baseGradient() const
{
    return d_ptr->m_baseGradient;
}

void Q3DTheme::setSingleHighlightGradient(const QLinearGradient &gradient)
{
    if (d_ptr->m_singleHighlightGradient != gradient) {
        d_ptr->m_dirtyBits.singleHighlightGradientDirty = true;
        d_ptr->m_singleHighlightGradient = gradient;
        emit singleHighlightGradientChanged(gradient);
    }
}

QLinearGradient Q3DTheme::singleHighlightGradient() const
{
    return d_ptr->m_singleHighlightGradient;
}

void Q3DTheme::setMultiHighlightGradient(const QLinearGradient &gradient)
{
    if (d_ptr->m_multiHighlightGradient != gradient) {
        d_ptr->m_dirtyBits.multiHighlightGradientDirty = true;
        d_ptr->m_multiHighlightGradient = gradient;
        emit multiHighlightGradientChanged(gradient);
    }
}

QLinearGradient Q3DTheme::multiHighlightGradient() const
{
    return d_ptr->m_multiHighlightGradient;
}

void Q3DTheme::setLightStrength(float strength)
{
    if (d_ptr->m_lightStrength != strength) {
        d_ptr->m_dirtyBits.lightStrengthDirty = true;
        d_ptr->m_lightStrength = strength;
        emit lightStrengthChanged(strength);
    }
}

float Q3DTheme::lightStrength() const
{
    return d_ptr->m_lightStrength;
}

void Q3DTheme::setAmbientLightStrength(float strength)
{
    if (d_ptr->m_ambientLightStrength != strength) {
        d_ptr->m_dirtyBits.ambientLightStrengthDirty = true;
        d_ptr->m_ambientLightStrength = strength;
        emit ambientLightStrengthChanged(strength);
    }
}

float Q3DTheme::ambientLightStrength() const
{
    return d_ptr->m_ambientLightStrength;
}

void Q3DTheme::setHighlightLightStrength(float strength)
{
    if (d_ptr->m_highlightLightStrength != strength) {
        d_ptr->m_dirtyBits.highlightLightStrengthDirty = true;
        d_ptr->m_highlightLightStrength = strength;
        emit highlightLightStrengthChanged(strength);
    }
}

float Q3DTheme::highlightLightStrength() const
{
    return d_ptr->m_highlightLightStrength;
}

void Q3DTheme::setLabelBorderEnabled(bool enabled)
{
    if (d_ptr->m_labelBorders != enabled) {
        d_ptr->m_dirtyBits.labelBorderEnabledDirty = true;
        d_ptr->m_labelBorders = enabled;
        emit labelBorderEnabledChanged(enabled);
    }
}

bool Q3DTheme::isLabelBorderEnabled() const
{
    return d_ptr->m_labelBorders;
}

void Q3DTheme::setFont(const QFont &font)
{
    if (d_ptr->m_font != font) {
        d_ptr->m_dirtyBits.fontDirty = true;
        d_ptr->m_font = font;
        emit fontChanged(font);
    }
}

QFont Q3DTheme::font() const
{
    return d_ptr->m_font;
}

void Q3DTheme::setBackgroundEnabled(bool enabled)
{
    if (d_ptr->m_backgoundEnabled != enabled) {
        d_ptr->m_dirtyBits.backgroundEnabledDirty = true;
        d_ptr->m_backgoundEnabled = enabled;
        emit backgroundEnabledChanged(enabled);
    }
}

bool Q3DTheme::isBackgroundEnabled() const
{
    return d_ptr->m_backgoundEnabled;
}

void Q3DTheme::setGridEnabled(bool enabled)
{
    if (d_ptr->m_gridEnabled != enabled) {
        d_ptr->m_dirtyBits.gridEnabledDirty = true;
        d_ptr->m_gridEnabled = enabled;
        emit gridEnabledChanged(enabled);
    }
}

bool Q3DTheme::isGridEnabled() const
{
    return d_ptr->m_gridEnabled;
}

void Q3DTheme::setLabelBackgroundEnabled(bool enabled)
{
    if (d_ptr->m_labelBackground != enabled) {
        d_ptr->m_dirtyBits.labelBackgroundEnabledDirty = true;
        d_ptr->m_labelBackground = enabled;
        emit labelBackgroundEnabledChanged(enabled);
    }
}

bool Q3DTheme::isLabelBackgroundEnabled() const
{
    return d_ptr->m_labelBackground;
}

void Q3DTheme::setColorStyle(QDataVis::ColorStyle style)
{
    if (d_ptr->m_colorStyle != style) {
        d_ptr->m_dirtyBits.colorStyleDirty = true;
        d_ptr->m_colorStyle = style;
        emit colorStyleChanged(style);
    }
}

QDataVis::ColorStyle Q3DTheme::colorStyle() const
{
    return d_ptr->m_colorStyle;
}

void Q3DTheme::setType(QDataVis::Theme themeType)
{
    if (d_ptr->m_themeId != themeType) {
        d_ptr->m_themeId = themeType;
        emit typeChanged(themeType);
    }
}

QDataVis::Theme Q3DTheme::type() const
{
    return d_ptr->m_themeId;
}

// Q3DThemePrivate

Q3DThemePrivate::Q3DThemePrivate(Q3DTheme *q, QDataVis::Theme theme_id)
    : QObject(0),
      m_themeId(theme_id),
      m_baseColor(Qt::white),
      m_backgroundColor(Qt::black),
      m_windowColor(Qt::black),
      m_textColor(Qt::white),
      m_textBackgroundColor(Qt::gray),
      m_gridLineColor(Qt::white),
      m_singleHighlightColor(Qt::red),
      m_multiHighlightColor(Qt::blue),
      m_lightColor(Qt::white),
      m_baseGradient(QLinearGradient(qreal(gradientTextureWidth),
                                     qreal(gradientTextureHeight),
                                     0.0, 0.0)),
      m_singleHighlightGradient(QLinearGradient(qreal(gradientTextureWidth),
                                                qreal(gradientTextureHeight),
                                                0.0, 0.0)),
      m_multiHighlightGradient(QLinearGradient(qreal(gradientTextureWidth),
                                               qreal(gradientTextureHeight),
                                               0.0, 0.0)),
      m_lightStrength(0.5f),
      m_ambientLightStrength(0.25f),
      m_highlightLightStrength(0.75f),
      m_labelBorders(true),
      m_colorStyle(QDataVis::ColorStyleUniform),
      m_font(QFont(QStringLiteral("Arial"))),
      m_backgoundEnabled(true),
      m_gridEnabled(true),
      m_labelBackground(true),
      q_ptr(q)
{
}

Q3DThemePrivate::~Q3DThemePrivate()
{
}

void Q3DThemePrivate::resetDirtyBits()
{
    m_dirtyBits.ambientLightStrengthDirty = false;
    m_dirtyBits.backgroundColorDirty = false;
    m_dirtyBits.backgroundEnabledDirty = false;
    m_dirtyBits.baseColorDirty = false;
    m_dirtyBits.baseGradientDirty = false;
    m_dirtyBits.colorStyleDirty = false;
    m_dirtyBits.fontDirty = false;
    m_dirtyBits.gridEnabledDirty = false;
    m_dirtyBits.gridLineColorDirty = false;
    m_dirtyBits.highlightLightStrengthDirty = false;
    m_dirtyBits.labelBackgroundEnabledDirty = false;
    m_dirtyBits.labelBorderEnabledDirty = false;
    m_dirtyBits.lightColorDirty = false;
    m_dirtyBits.lightStrengthDirty = false;
    m_dirtyBits.multiHighlightColorDirty = false;
    m_dirtyBits.multiHighlightGradientDirty = false;
    m_dirtyBits.singleHighlightColorDirty = false;
    m_dirtyBits.singleHighlightGradientDirty = false;
    m_dirtyBits.textBackgroundColorDirty = false;
    m_dirtyBits.textColorDirty = false;
    m_dirtyBits.windowColorDirty = false;
}

QT_DATAVISUALIZATION_END_NAMESPACE

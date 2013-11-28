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

// TODO: Add needRenders if necessary after color api has been added to series (QTRD-2200/2557)
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        emit needRender();
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
        d_ptr->m_dirtyBits.themeIdDirty = true;
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
      m_font(QFont()),
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
    m_dirtyBits.ambientLightStrengthDirty = true;
    m_dirtyBits.backgroundColorDirty = true;
    m_dirtyBits.backgroundEnabledDirty = true;
    m_dirtyBits.baseColorDirty = true;
    m_dirtyBits.baseGradientDirty = true;
    m_dirtyBits.colorStyleDirty = true;
    m_dirtyBits.fontDirty = true;
    m_dirtyBits.gridEnabledDirty = true;
    m_dirtyBits.gridLineColorDirty = true;
    m_dirtyBits.highlightLightStrengthDirty = true;
    m_dirtyBits.labelBackgroundEnabledDirty = true;
    m_dirtyBits.labelBorderEnabledDirty = true;
    m_dirtyBits.lightColorDirty = true;
    m_dirtyBits.lightStrengthDirty = true;
    m_dirtyBits.multiHighlightColorDirty = true;
    m_dirtyBits.multiHighlightGradientDirty = true;
    m_dirtyBits.singleHighlightColorDirty = true;
    m_dirtyBits.singleHighlightGradientDirty = true;
    m_dirtyBits.textBackgroundColorDirty = true;
    m_dirtyBits.textColorDirty = true;
    m_dirtyBits.themeIdDirty = true;
    m_dirtyBits.windowColorDirty = true;
}

bool Q3DThemePrivate::sync(Q3DThemePrivate &other)
{
    bool changed = false;
    if (m_dirtyBits.ambientLightStrengthDirty) {
        other.q_ptr->setAmbientLightStrength(m_ambientLightStrength);
        m_dirtyBits.ambientLightStrengthDirty = false;
        changed = true;
    }
    if (m_dirtyBits.backgroundColorDirty) {
        other.q_ptr->setBackgroundColor(m_backgroundColor);
        m_dirtyBits.backgroundColorDirty = false;
        changed = true;
    }
    if (m_dirtyBits.backgroundEnabledDirty) {
        other.q_ptr->setBackgroundEnabled(m_backgoundEnabled);
        m_dirtyBits.backgroundEnabledDirty = false;
        changed = true;
    }
    if (m_dirtyBits.baseColorDirty) {
        other.q_ptr->setBaseColor(m_baseColor);
        m_dirtyBits.baseColorDirty = false;
        changed = true;
    }
    if (m_dirtyBits.baseGradientDirty) {
        other.q_ptr->setBaseGradient(m_baseGradient);
        m_dirtyBits.baseGradientDirty = false;
        changed = true;
    }
    if (m_dirtyBits.colorStyleDirty) {
        other.q_ptr->setColorStyle(m_colorStyle);
        m_dirtyBits.colorStyleDirty = false;
        changed = true;
    }
    if (m_dirtyBits.fontDirty) {
        other.q_ptr->setFont(m_font);
        m_dirtyBits.fontDirty = false;
        changed = true;
    }
    if (m_dirtyBits.gridEnabledDirty) {
        other.q_ptr->setGridEnabled(m_gridEnabled);
        m_dirtyBits.gridEnabledDirty = false;
        changed = true;
    }
    if (m_dirtyBits.gridLineColorDirty) {
        other.q_ptr->setGridLineColor(m_gridLineColor);
        m_dirtyBits.gridLineColorDirty = false;
        changed = true;
    }
    if (m_dirtyBits.highlightLightStrengthDirty) {
        other.q_ptr->setHighlightLightStrength(m_highlightLightStrength);
        m_dirtyBits.highlightLightStrengthDirty = false;
        changed = true;
    }
    if (m_dirtyBits.labelBackgroundEnabledDirty) {
        other.q_ptr->setLabelBackgroundEnabled(m_labelBackground);
        m_dirtyBits.labelBackgroundEnabledDirty = false;
        changed = true;
    }
    if (m_dirtyBits.labelBorderEnabledDirty) {
        other.q_ptr->setLabelBorderEnabled(m_labelBorders);
        m_dirtyBits.labelBorderEnabledDirty = false;
        changed = true;
    }
    if (m_dirtyBits.lightColorDirty) {
        other.q_ptr->setLightColor(m_lightColor);
        m_dirtyBits.lightColorDirty = false;
        changed = true;
    }
    if (m_dirtyBits.lightStrengthDirty) {
        other.q_ptr->setLightStrength(m_lightStrength);
        m_dirtyBits.lightStrengthDirty = false;
        changed = true;
    }
    if (m_dirtyBits.multiHighlightColorDirty) {
        other.q_ptr->setMultiHighlightColor(m_multiHighlightColor);
        m_dirtyBits.multiHighlightColorDirty = false;
        changed = true;
    }
    if (m_dirtyBits.multiHighlightGradientDirty) {
        other.q_ptr->setMultiHighlightGradient(m_multiHighlightGradient);
        m_dirtyBits.multiHighlightGradientDirty = false;
        changed = true;
    }
    if (m_dirtyBits.singleHighlightColorDirty) {
        other.q_ptr->setSingleHighlightColor(m_singleHighlightColor);
        m_dirtyBits.singleHighlightColorDirty = false;
        changed = true;
    }
    if (m_dirtyBits.singleHighlightGradientDirty) {
        other.q_ptr->setSingleHighlightGradient(m_singleHighlightGradient);
        m_dirtyBits.singleHighlightGradientDirty = false;
        changed = true;
    }
    if (m_dirtyBits.textBackgroundColorDirty) {
        other.q_ptr->setTextBackgroundColor(m_textBackgroundColor);
        m_dirtyBits.textBackgroundColorDirty = false;
        changed = true;
    }
    if (m_dirtyBits.textColorDirty) {
        other.q_ptr->setTextColor(m_textColor);
        m_dirtyBits.textColorDirty = false;
        changed = true;
    }
    if (m_dirtyBits.themeIdDirty) {
        other.m_themeId = m_themeId; // Set directly to avoid a call to ThemeManager's useTheme()
        m_dirtyBits.themeIdDirty = false;
        changed = true;
    }
    if (m_dirtyBits.windowColorDirty) {
        other.q_ptr->setWindowColor(m_windowColor);
        m_dirtyBits.windowColorDirty = false;
        changed = true;
    }
    return changed;
}

QT_DATAVISUALIZATION_END_NAMESPACE

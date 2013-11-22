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

#include "builtin3dthemes_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

BuiltIn3DThemeQt::BuiltIn3DThemeQt()
    : Q3DTheme(QDataVis::ThemeQt)
{
    setBaseColor(QColor(QRgb(0x80c342)));
    setBackgroundColor(QColor(QRgb(0xffffff)));
    setWindowColor(QColor(QRgb(0xffffff)));
    setTextColor(QColor(QRgb(0x35322f)));
    setTextBackgroundColor(QColor(0xff, 0xff, 0xff, 0x99));
    setGridLineColor(QColor(QRgb(0xd7d6d5)));
    setSingleHighlightColor(QColor(QRgb(0x14aaff)));
    setMultiHighlightColor(QColor(QRgb(0x6400aa)));
    setLightStrength(5.0f);
    setAmbientLightStrength(0.5f);
    setHighlightLightStrength(5.0f);
    setLabelBorderEnabled(true);
    setColorStyle(QDataVis::ColorStyleUniform);
}

BuiltIn3DThemePrimaryColors::BuiltIn3DThemePrimaryColors()
    : Q3DTheme(QDataVis::ThemePrimaryColors)
{
    setBaseColor(QColor(QRgb(0xffe400)));
    setBackgroundColor(QColor(QRgb(0xffffff)));
    setWindowColor(QColor(QRgb(0xffffff)));
    setTextColor(QColor(QRgb(0x000000)));
    setTextBackgroundColor(QColor(0xff, 0xff, 0xff, 0x99));
    setGridLineColor(QColor(QRgb(0xd7d6d5)));
    setSingleHighlightColor(QColor(QRgb(0x27beee)));
    setMultiHighlightColor(QColor(QRgb(0xee1414)));
    setLightStrength(5.0f);
    setAmbientLightStrength(0.5f);
    setHighlightLightStrength(5.0f);
    setLabelBorderEnabled(false);
    setColorStyle(QDataVis::ColorStyleUniform);
}

BuiltIn3DThemeDigia::BuiltIn3DThemeDigia()
    : Q3DTheme(QDataVis::ThemeDigia)
{
    setBackgroundColor(QColor(QRgb(0xffffff)));
    setWindowColor(QColor(QRgb(0xffffff)));
    setTextColor(QColor(QRgb(0x000000)));
    setTextBackgroundColor(QColor(0xff, 0xff, 0xff, 0x80));
    setGridLineColor(QColor(QRgb(0xd7d6d5)));
    setSingleHighlightColor(QColor(QRgb(0xfa0000)));
    setMultiHighlightColor(QColor(QRgb(0x555555)));
    setLightStrength(5.0f);
    setAmbientLightStrength(0.5f);
    setHighlightLightStrength(5.0f);
    setLabelBorderEnabled(false);
    setColorStyle(QDataVis::ColorStyleObjectGradient);
    QLinearGradient gradient = QLinearGradient(qreal(gradientTextureWidth),
                                               qreal(gradientTextureHeight),
                                               0.0, 0.0);
    QColor color;
    color.setRed(0xcc * 0.7);
    color.setGreen(0xcc * 0.7);
    color.setBlue(0xcc * 0.7);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0xcccccc)));
    setBaseGradient(gradient);
    color.setRed(0xfa * 0.7);
    color.setGreen(0x00);
    color.setBlue(0x00);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0xfa0000)));
    setSingleHighlightGradient(gradient);
    color.setRed(0x55 * 0.7);
    color.setGreen(0x55 * 0.7);
    color.setBlue(0x55 * 0.7);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0x555555)));
    setMultiHighlightGradient(gradient);
}

BuiltIn3DThemeStoneMoss::BuiltIn3DThemeStoneMoss()
    : Q3DTheme(QDataVis::ThemeStoneMoss)
{
    setBaseColor(QColor(QRgb(0xbeb32b)));
    setBackgroundColor(QColor(QRgb(0x4d4d4f)));
    setWindowColor(QColor(QRgb(0x4d4d4f)));
    setTextColor(QColor(QRgb(0xffffff)));
    setTextBackgroundColor(QColor(0x4d, 0x4d, 0x4f, 0xcd));
    setGridLineColor(QColor(QRgb(0x3e3e40)));
    setSingleHighlightColor(QColor(QRgb(0xfbf6d6)));
    setMultiHighlightColor(QColor(QRgb(0x442f20)));
    setLightStrength(5.0f);
    setAmbientLightStrength(0.5f);
    setHighlightLightStrength(5.0f);
    setLabelBorderEnabled(true);
    setColorStyle(QDataVis::ColorStyleUniform);
}

BuiltIn3DThemeArmyBlue::BuiltIn3DThemeArmyBlue()
    : Q3DTheme(QDataVis::ThemeArmyBlue)
{
    setBackgroundColor(QColor(QRgb(0xd5d6d7)));
    setWindowColor(QColor(QRgb(0xd5d6d7)));
    setTextColor(QColor(QRgb(0x000000)));
    setTextBackgroundColor(QColor(0xd5, 0xd6, 0xd7, 0xcd));
    setGridLineColor(QColor(QRgb(0xaeadac)));
    setSingleHighlightColor(QColor(QRgb(0x2aa2f9)));
    setMultiHighlightColor(QColor(QRgb(0x103753)));
    setLightStrength(5.0f);
    setAmbientLightStrength(0.5f);
    setHighlightLightStrength(5.0f);
    setLabelBorderEnabled(false);
    setColorStyle(QDataVis::ColorStyleObjectGradient);
    QLinearGradient gradient = QLinearGradient(qreal(gradientTextureWidth),
                                               qreal(gradientTextureHeight),
                                               0.0, 0.0);
    QColor color;
    color.setRed(0x49 * 0.7);
    color.setGreen(0x5f * 0.7);
    color.setBlue(0x76 * 0.7);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0x495f76)));
    setBaseGradient(gradient);
    color.setRed(0x2a * 0.7);
    color.setGreen(0xa2 * 0.7);
    color.setBlue(0xf9 * 0.7);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0x2aa2f9)));
    setSingleHighlightGradient(gradient);
    color.setRed(0x10 * 0.7);
    color.setGreen(0x37 * 0.7);
    color.setBlue(0x53 * 0.7);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0x103753)));
    setMultiHighlightGradient(gradient);
}

BuiltIn3DThemeRetro::BuiltIn3DThemeRetro()
    : Q3DTheme(QDataVis::ThemeRetro)
{
    setBackgroundColor(QColor(QRgb(0xe9e2ce)));
    setWindowColor(QColor(QRgb(0xe9e2ce)));
    setTextColor(QColor(QRgb(0x000000)));
    setTextBackgroundColor(QColor(0xe9, 0xe2, 0xce, 0xc0));
    setGridLineColor(QColor(QRgb(0xd0c0b0)));
    setSingleHighlightColor(QColor(QRgb(0x8ea317)));
    setMultiHighlightColor(QColor(QRgb(0xc25708)));
    setLightStrength(5.0f);
    setAmbientLightStrength(0.5f);
    setHighlightLightStrength(5.0f);
    setLabelBorderEnabled(false);
    setColorStyle(QDataVis::ColorStyleObjectGradient);
    QLinearGradient gradient = QLinearGradient(qreal(gradientTextureWidth),
                                               qreal(gradientTextureHeight),
                                               0.0, 0.0);
    QColor color;
    color.setRed(0x53 * 0.7);
    color.setGreen(0x3b * 0.7);
    color.setBlue(0x23 * 0.7);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0x533b23)));
    setBaseGradient(gradient);
    color.setRed(0x8e * 0.7);
    color.setGreen(0xa3 * 0.7);
    color.setBlue(0x17 * 0.7);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0x8ea317)));
    setSingleHighlightGradient(gradient);
    color.setRed(0xc2 * 0.7);
    color.setGreen(0x57 * 0.7);
    color.setBlue(0x08 * 0.7);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, QColor(QRgb(0xc25708)));
    setMultiHighlightGradient(gradient);
}

BuiltIn3DThemeEbony::BuiltIn3DThemeEbony()
    : Q3DTheme(QDataVis::ThemeEbony)
{
    setBaseColor(QColor(QRgb(0xffffff)));
    setBackgroundColor(QColor(QRgb(0x000000)));
    setWindowColor(QColor(QRgb(0x000000)));
    setTextColor(QColor(QRgb(0xaeadac)));
    setTextBackgroundColor(QColor(0x00, 0x00, 0x00, 0xcd));
    setGridLineColor(QColor(QRgb(0x35322f)));
    setSingleHighlightColor(QColor(QRgb(0xf5dc0d)));
    setMultiHighlightColor(QColor(QRgb(0xd72222)));
    setLightStrength(5.0f);
    setAmbientLightStrength(0.5f);
    setHighlightLightStrength(5.0f);
    setLabelBorderEnabled(false);
    setColorStyle(QDataVis::ColorStyleUniform);
}

BuiltIn3DThemeIsabelle::BuiltIn3DThemeIsabelle()
    : Q3DTheme(QDataVis::ThemeIsabelle)
{
    setBaseColor(QColor(QRgb(0xf9d900)));
    setBackgroundColor(QColor(QRgb(0x000000)));
    setWindowColor(QColor(QRgb(0x000000)));
    setTextColor(QColor(QRgb(0xaeadac)));
    setTextBackgroundColor(QColor(0x00, 0x00, 0x00, 0xc0));
    setGridLineColor(QColor(QRgb(0x35322f)));
    setSingleHighlightColor(QColor(QRgb(0xfff7cc)));
    setMultiHighlightColor(QColor(QRgb(0xde0a0a)));
    setLightStrength(5.0f);
    setAmbientLightStrength(0.5f);
    setHighlightLightStrength(5.0f);
    setLabelBorderEnabled(false);
    setColorStyle(QDataVis::ColorStyleUniform);
}

QT_DATAVISUALIZATION_END_NAMESPACE

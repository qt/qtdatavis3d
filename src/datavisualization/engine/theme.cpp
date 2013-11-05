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

#include "theme_p.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <stdio.h>
#endif

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Theme::Theme()
    : m_baseColor(QColor(Qt::gray)),
      m_heightColor(QColor(Qt::black)),
      m_depthColor(QColor(Qt::black)),
      m_backgroundColor(QColor(Qt::gray)),
      m_windowColor(QColor(Qt::gray)),
      m_textColor(QColor(Qt::white)),
      m_textBackgroundColor(QColor(0x00, 0x00, 0x00, 0xa0)),
      m_gridLine(QColor(Qt::black)),
      m_highlightBarColor(QColor(Qt::red)),
      m_highlightRowColor(QColor(Qt::darkRed)),
      m_highlightColumnColor(QColor(Qt::darkMagenta)),
      m_surfaceGradient(QLinearGradient(qreal(gradientTextureWidth), qreal(gradientTextureHeight),
                                        0.0, 0.0)),
      m_lightStrength(4.0f),
      m_ambientStrength(0.3f),
      m_highlightLightStrength(8.0f),
      m_uniformColor(true),
      m_labelBorders(false)
{
    // Default values for surface gradient
}

Theme::~Theme()
{
}

QDataVis::Theme Theme::theme()
{
    return m_theme;
}

void Theme::useTheme(QDataVis::Theme theme)
{
    m_theme = theme;
    switch (theme) {
    case QDataVis::ThemeQt: {
        m_baseColor = QColor(QRgb(0x80c342));
        //m_heightColor = QColor(QRgb(0x));
        //m_depthColor = QColor(QRgb(0x));
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x35322f));
        m_textBackgroundColor = QColor(0xff, 0xff, 0xff, 0x99);
        m_gridLine = QColor(QRgb(0xd7d6d5));
        m_highlightBarColor = QColor(QRgb(0x14aaff));
        m_highlightRowColor = QColor(QRgb(0x6400aa));
        m_highlightColumnColor = QColor(QRgb(0x6400aa));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 5.0f;
        m_uniformColor = true;
        m_labelBorders = true;
        break;
    }
    case QDataVis::ThemePrimaryColors: {
        m_baseColor = QColor(QRgb(0xffe400));
        //m_heightColor = QColor(QRgb(0x));
        //m_depthColor = QColor(QRgb(0x));
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x000000));
        m_textBackgroundColor = QColor(0xff, 0xff, 0xff, 0x99);
        m_gridLine = QColor(QRgb(0xd7d6d5));
        m_highlightBarColor = QColor(QRgb(0x27beee));
        m_highlightRowColor = QColor(QRgb(0xee1414));
        m_highlightColumnColor = QColor(QRgb(0xee1414));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 5.0f;
        m_uniformColor = true;
        m_labelBorders = false;
        break;
    }
    case QDataVis::ThemeDigia: {
        m_baseColor = QColor(QRgb(0xcccccc));
        //m_heightColor = QColor(QRgb(0x));
        //m_depthColor = QColor(QRgb(0x));
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x000000));
        m_textBackgroundColor = QColor(0xff, 0xff, 0xff, 0x80);
        m_gridLine = QColor(QRgb(0xd7d6d5));
        m_highlightBarColor = QColor(QRgb(0xfa0000));
        m_highlightRowColor = QColor(QRgb(0x555555));
        m_highlightColumnColor = QColor(QRgb(0x555555));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 5.0f;
        m_uniformColor = false;
        m_labelBorders = false;
        break;
    }
    case QDataVis::ThemeStoneMoss: {
        m_baseColor = QColor(QRgb(0xbeb32b));
        //m_heightColor = QColor(QRgb(0x));
        //m_depthColor = QColor(QRgb(0x));
        m_backgroundColor = QColor(QRgb(0x4d4d4f));
        m_windowColor = QColor(QRgb(0x4d4d4f));
        m_textColor = QColor(QRgb(0xffffff));
        m_textBackgroundColor = QColor(0x4d, 0x4d, 0x4f, 0xcd);
        m_gridLine = QColor(QRgb(0x3e3e40));
        m_highlightBarColor = QColor(QRgb(0xfbf6d6));
        m_highlightRowColor = QColor(QRgb(0x442f20));
        m_highlightColumnColor = QColor(QRgb(0x442f20));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 5.0f;
        m_uniformColor = true;
        m_labelBorders = true;
        break;
    }
    case QDataVis::ThemeArmyBlue: {
        m_baseColor = QColor(QRgb(0x495f76));
        //m_heightColor = QColor(QRgb(0x));
        //m_depthColor = QColor(QRgb(0x));
        m_backgroundColor = QColor(QRgb(0xd5d6d7));
        m_windowColor = QColor(QRgb(0xd5d6d7));
        m_textColor = QColor(QRgb(0x000000));
        m_textBackgroundColor = QColor(0xd5, 0xd6, 0xd7, 0xcd);
        m_gridLine = QColor(QRgb(0xaeadac));
        m_highlightBarColor = QColor(QRgb(0x2aa2f9));
        m_highlightRowColor = QColor(QRgb(0x103753));
        m_highlightColumnColor = QColor(QRgb(0x103753));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 5.0f;
        m_uniformColor = false;
        m_labelBorders = false;
        break;
    }
    case QDataVis::ThemeRetro: {
        m_baseColor = QColor(QRgb(0x533b23));
        //m_heightColor = QColor(QRgb(0x));
        //m_depthColor = QColor(QRgb(0x));
        m_backgroundColor = QColor(QRgb(0xe9e2ce));
        m_windowColor = QColor(QRgb(0xe9e2ce));
        m_textColor = QColor(QRgb(0x000000));
        m_textBackgroundColor = QColor(0xe9, 0xe2, 0xce, 0xc0);
        m_gridLine = QColor(QRgb(0xd0c0b0));
        m_highlightBarColor = QColor(QRgb(0x8ea317));
        m_highlightRowColor = QColor(QRgb(0xc25708));
        m_highlightColumnColor = QColor(QRgb(0xc25708));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 5.0f;
        m_uniformColor = false;
        m_labelBorders = false;
        break;
    }
    case QDataVis::ThemeEbony: {
        m_baseColor = QColor(QRgb(0xffffff));
        //m_heightColor = QColor(QRgb(0x));
        //m_depthColor = QColor(QRgb(0x));
        m_backgroundColor = QColor(QRgb(0x000000));
        m_windowColor = QColor(QRgb(0x000000));
        m_textColor = QColor(QRgb(0xaeadac));
        m_textBackgroundColor = QColor(0x00, 0x00, 0x00, 0xcd);
        m_gridLine = QColor(QRgb(0x35322f));
        m_highlightBarColor = QColor(QRgb(0xf5dc0d));
        m_highlightRowColor = QColor(QRgb(0xd72222));
        m_highlightColumnColor = QColor(QRgb(0xd72222));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 5.0f;
        m_uniformColor = true;
        m_labelBorders = false;
        break;
    }
    case QDataVis::ThemeIsabelle: {
        m_baseColor = QColor(QRgb(0xf9d900));
        //m_heightColor = QColor(QRgb(0x));
        //m_depthColor = QColor(QRgb(0x));
        m_backgroundColor = QColor(QRgb(0x000000));
        m_windowColor = QColor(QRgb(0x000000));
        m_textColor = QColor(QRgb(0xaeadac));
        m_textBackgroundColor = QColor(0x00, 0x00, 0x00, 0xc0);
        m_gridLine = QColor(QRgb(0x35322f));
        m_highlightBarColor = QColor(QRgb(0xfff7cc));
        m_highlightRowColor = QColor(QRgb(0xde0a0a));
        m_highlightColumnColor = QColor(QRgb(0xde0a0a));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 5.0f;
        m_uniformColor = true;
        m_labelBorders = false;
        break;
    }
    default:
        break;
    }
    if (m_uniformColor) {
        m_surfaceGradient.setColorAt(0.0, m_baseColor);
    } else {
        m_surfaceGradient.setColorAt(0.0, QColor(m_baseColor.redF() * 0.7,
                                                 m_baseColor.greenF() * 0.7,
                                                 m_baseColor.blueF() * 0.7));
    }
    m_surfaceGradient.setColorAt(1.0, m_baseColor);
}

void Theme::setFromTheme(Theme &theme)
{
    m_theme = theme.m_theme;
    m_baseColor = theme.m_baseColor;
    m_heightColor = theme.m_heightColor;
    m_depthColor = theme.m_depthColor;
    m_backgroundColor = theme.m_backgroundColor;
    m_windowColor = theme.m_windowColor;
    m_textColor = theme.m_textColor;
    m_textBackgroundColor = theme.m_textBackgroundColor;
    m_gridLine = theme.m_gridLine;
    m_highlightBarColor = theme.m_highlightBarColor;
    m_highlightRowColor = theme.m_highlightRowColor;
    m_highlightColumnColor = theme.m_highlightColumnColor;
    m_surfaceGradient = theme.m_surfaceGradient;
    m_lightStrength = theme.m_lightStrength;
    m_ambientStrength = theme.m_ambientStrength;
    m_highlightLightStrength = theme.m_highlightLightStrength;
    m_uniformColor = theme.m_uniformColor;
    m_labelBorders = theme.m_labelBorders;
}

QT_DATAVISUALIZATION_END_NAMESPACE

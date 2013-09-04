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

#include "theme_p.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <stdio.h>
#endif

QT_DATAVIS3D_BEGIN_NAMESPACE

Theme::Theme()
    : m_baseColor(QColor(Qt::gray)),
      m_heightColor(QColor(Qt::black)),
      m_depthColor(QColor(Qt::black)),
      m_backgroundColor(QColor(Qt::gray)),
      m_windowColor(QColor(Qt::gray)),
      m_textColor(QColor(Qt::white)),
      m_textBackgroundColor(QColor(0x00, 0x00, 0x00, 0x80)),
      m_gridLine(QColor(Qt::black)),
      m_highlightBarColor(QColor(Qt::red)),
      m_highlightRowColor(QColor(Qt::darkRed)),
      m_highlightColumnColor(QColor(Qt::darkMagenta)),
      m_surfaceGradient(QLinearGradient(1, 100, 0, 0)),
      m_lightStrength(4.0f),
      m_ambientStrength(0.3f),
      m_highlightLightStrength(8.0f),
      m_uniformColor(true)
{
    // Default values for surface gradient
    m_surfaceGradient.setColorAt(0.0, Qt::green);
    m_surfaceGradient.setColorAt(0.5, Qt::yellow);
    m_surfaceGradient.setColorAt(1.0, Qt::red);
}

Theme::~Theme()
{
}

QDataVis::ColorTheme Theme::colorTheme()
{
    return m_colorTheme;
}

void Theme::useColorTheme(QDataVis::ColorTheme colorTheme)
{
    m_colorTheme = colorTheme;
    switch (colorTheme) {
    case QDataVis::ThemeSystem: {
#ifdef Q_OS_WIN
        DWORD colorHighlight;
        colorHighlight = GetSysColor(COLOR_HIGHLIGHT);
        m_baseColor = QColor(GetRValue(colorHighlight),
                             GetGValue(colorHighlight),
                             GetBValue(colorHighlight));
        DWORD colorWindowFrame;
        colorWindowFrame = GetSysColor(COLOR_WINDOWFRAME);
        m_heightColor = QColor(GetRValue(colorWindowFrame),
                               GetGValue(colorWindowFrame),
                               GetBValue(colorWindowFrame));
        m_depthColor = QColor(Qt::black);
        DWORD colorWindow;
        colorWindow = GetSysColor(COLOR_WINDOW);
        m_backgroundColor = QColor(GetRValue(colorWindow),
                                   GetGValue(colorWindow),
                                   GetBValue(colorWindow));
        m_windowColor = QColor(GetRValue(colorWindow),
                               GetGValue(colorWindow),
                               GetBValue(colorWindow));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_gridLine = QColor(QRgb(0xe2e2e2));
        m_highlightBarColor = QColor(QRgb(0xe2e2e2));
        m_highlightRowColor = QColor(QRgb(0xf2f2f2));
        m_highlightColumnColor = QColor(QRgb(0xf2f2f2));
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
#elif defined(Q_OS_LINUX)
        m_baseColor = QColor(QRgb(0x60a6e6));
        m_heightColor = QColor(QRgb(0xfc5751));
        m_depthColor = QColor(QRgb(0x92ca66));
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_gridLine = QColor(QRgb(0xe2e2e2));
        m_highlightBarColor = QColor(QRgb(0xeba85f));
        m_highlightRowColor = QColor(QRgb(0xfc5751));
        m_highlightColumnColor = QColor(QRgb(0xfc5751));
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
#elif defined(Q_OS_MAC)
        m_baseColor = QColor(QRgb(0x60a6e6));
        m_heightColor = QColor(QRgb(0xfc5751));
        m_depthColor = QColor(QRgb(0x92ca66));
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_gridLine = QColor(QRgb(0xe2e2e2));
        m_highlightBarColor = QColor(QRgb(0xeba85f));
        m_highlightRowColor = QColor(QRgb(0xfc5751));
        m_highlightColumnColor = QColor(QRgb(0xfc5751));
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
#else
        m_baseColor = QColor(QRgb(0x60a6e6));
        m_heightColor = QColor(QRgb(0xfc5751));
        m_depthColor = QColor(QRgb(0x92ca66));
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_gridLine = QColor(QRgb(0xe2e2e2));
        m_highlightBarColor = QColor(QRgb(0xeba85f));
        m_highlightRowColor = QColor(QRgb(0xfc5751));
        m_highlightColumnColor = QColor(QRgb(0xfc5751));
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
#endif
        qDebug("ThemeSystem");
        break;
    }
    case QDataVis::ThemeBlueCerulean: {
        m_baseColor = QColor(QRgb(0xc7e85b));
        m_heightColor = QColor(QRgb(0xee7392));
        m_depthColor = QColor(QRgb(0x1cb54f));
        m_backgroundColor = QColor(QRgb(0x056189));
        m_windowColor = QColor(QRgb(0x101a31));
        m_textColor = QColor(QRgb(0xffffff));
        m_textBackgroundColor = QColor(0x05, 0x61, 0x89, 0x80);
        m_gridLine = QColor(QRgb(0x84a2b0));
        m_highlightBarColor = QColor(QRgb(0x5cbf9b));
        m_highlightRowColor = QColor(QRgb(0x009fbf));
        m_highlightColumnColor = QColor(QRgb(0x009fbf));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.2f;
        m_highlightLightStrength = 10.0f;
        m_uniformColor = true;
        qDebug("ThemeBlueCerulean");
        break;
    }
    case QDataVis::ThemeBlueIcy: {
        m_baseColor = QRgb(0x3daeda);
        m_heightColor = QRgb(0x2fa3b4);
        m_depthColor = QColor(QRgb(0x2685bf));
        m_backgroundColor = QColor(QRgb(0x2fa3b4));
        //m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xff, 0xff, 0xff, 0x80);
        m_gridLine = QColor(QRgb(0xe2e2e2));
        m_highlightBarColor = QColor(QRgb(0x0c2673));
        m_highlightRowColor = QColor(QRgb(0x5f3dba));
        m_highlightColumnColor = QColor(QRgb(0x5f3dba));
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 8.0f;
        m_uniformColor = true;
        qDebug("ThemeBlueIcy");
        break;
    }
    case QDataVis::ThemeBlueNcs: {
        m_baseColor = QColor(QRgb(0x1db0da));
        m_heightColor = QColor(QRgb(0x398ca3));
        m_depthColor = QColor(QRgb(0x1341a6));
        m_backgroundColor = QColor(QRgb(0x398ca3));
        //m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xff, 0xff, 0xff, 0x80);
        m_gridLine = QColor(QRgb(0xe2e2e2));
        m_highlightBarColor = QColor(QRgb(0x88d41e));
        m_highlightRowColor = QColor(QRgb(0xff8e1a));
        m_highlightColumnColor = QColor(QRgb(0xff8e1a));
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.2f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
        qDebug("ThemeBlueNcs");
        break;
    }
    case QDataVis::ThemeBrownSand: {
        m_baseColor = QColor(QRgb(0xb39b72));
        m_heightColor = QColor(QRgb(0x494345));
        m_depthColor = QColor(QRgb(0xb3b376));
        m_backgroundColor = QColor(QRgb(0x494345));
        //m_backgroundColor = QColor(QRgb(0xf3ece0));
        m_windowColor = QColor(QRgb(0xf3ece0));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xb5, 0xb0, 0xa7, 0xC0);
        m_gridLine = QColor(QRgb(0xd4cec3));
        m_highlightBarColor = QColor(QRgb(0xc35660));
        m_highlightRowColor = QColor(QRgb(0x536780));
        m_highlightColumnColor = QColor(QRgb(0x536780));
        m_lightStrength = 6.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 8.0f;
        m_uniformColor = false;
        qDebug("ThemeBrownSand");
        break;
    }
    case QDataVis::ThemeDark: {
        m_baseColor = QColor(QRgb(0x38ad6b));               // charts: series color 1
        m_heightColor = QColor(QRgb(0xbf593e));             // charts: series color 5
        m_depthColor = QColor(QRgb(0x3c84a7));              // charts: series color 2
        m_backgroundColor = QColor(QRgb(0x2e303a));         // charts: background color 1
        m_windowColor = QColor(QRgb(0x121218));             // charts: background color 2
        m_textColor = QColor(QRgb(0xffffff));               // charts: label color
        m_textBackgroundColor = QColor(0x86, 0x87, 0x8c, 0x80); // charts: axis line pen OR background color 2
        m_gridLine = QColor(QRgb(0x86878c));                // charts: grid line color
        m_highlightBarColor = QColor(QRgb(0xeb8817));       // charts: series color 3
        m_highlightRowColor = QColor(QRgb(0x7b7f8c));       // charts: series color 4
        m_highlightColumnColor = QColor(QRgb(0x7b7f8c));    // charts: series color 4
        m_lightStrength = 6.0f;
        m_ambientStrength = 0.2f;
        m_highlightLightStrength = 8.0f;
        m_uniformColor = false;
        qDebug("ThemeDark");
        break;
    }
    case QDataVis::ThemeHighContrast: {
        m_baseColor = QColor(QRgb(0xff4a41));
        m_heightColor = QColor(QRgb(0x202020));
        m_depthColor = QColor(QRgb(0x596a74));
        m_backgroundColor = QColor(QRgb(0x596a74));
        //m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0x000000));
        m_textColor = QColor(QRgb(0xffffff));
        m_textBackgroundColor = QColor(0x18, 0x18, 0x18, 0x80);
        //m_textColor = QColor(QRgb(0x181818));
        //m_textBackgroundColor = QColor(0xff, 0xff, 0xff, 0x80);
        m_gridLine = QColor(QRgb(0x8c8c8c));
        m_highlightBarColor = QColor(QRgb(0xffab03));
        m_highlightRowColor = QColor(QRgb(0x038e9b));
        m_highlightColumnColor = QColor(QRgb(0x038e9b));
        m_lightStrength = 8.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 10.0f;
        m_uniformColor = false;
        qDebug("ThemeHighContrast");
        break;
    }
    case QDataVis::ThemeLight: {
        m_baseColor = QColor(QRgb(0x209fdf));
        m_heightColor = QColor(QRgb(0xbf593e));
        m_depthColor = QColor(QRgb(0x99ca53));
        m_backgroundColor = QColor(QRgb(0x99ca53));
        //m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_gridLine = QColor(QRgb(0xe2e2e2));
        m_highlightBarColor = QColor(QRgb(0xf6a625));
        m_highlightRowColor = QColor(QRgb(0x6d5fd5));
        m_highlightColumnColor = QColor(QRgb(0x6d5fd5));
        m_lightStrength = 6.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 7.0f;
        m_uniformColor = true;
        qDebug("ThemeLight");
        break;
    }
    default:
        break;
    }
}

void Theme::setFromTheme(Theme &theme)
{
    m_colorTheme = theme.m_colorTheme;
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
}

QT_DATAVIS3D_END_NAMESPACE

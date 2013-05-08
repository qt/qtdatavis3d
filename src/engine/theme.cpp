/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "theme_p.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <stdio.h>
#endif

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

Theme::Theme()
    : m_baseColor(QColor(Qt::gray)),
      m_heightColor(QColor(Qt::white)),
      m_depthColor(QColor(Qt::darkGray)),
      m_backgroundColor(QColor(Qt::gray)),
      m_windowColor(QColor(Qt::gray)),
      m_textColor(QColor(Qt::white)),
      m_textBackgroundColor(QColor(0x00, 0x00, 0x00, 0x80)),
      m_highlightBarColor(QColor(Qt::red)),
      m_highlightRowColor(QColor(Qt::darkRed)),
      m_highlightColumnColor(QColor(Qt::darkMagenta)),
      m_lightStrength(4.0f),
      m_ambientStrength(0.3f),
      m_highlightLightStrength(8.0f),
      m_uniformColor(true)
{
}

Theme::~Theme()
{
}

void Theme::useTheme(ColorTheme theme)
{
    switch (theme) {
    case ThemeSystem: {
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
        m_depthColor = QColor(Qt::black);
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_highlightBarColor = QColor(QRgb(0xe2e2e2));
        m_highlightRowColor = QColor(QRgb(0xf2f2f2));
        m_highlightColumnColor = QColor(QRgb(0xf2f2f2));
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
#elif defined(Q_OS_MAC)
        m_baseColor = QColor(QRgb(0x60a6e6));
        m_heightColor = QColor(QRgb(0xfc5751));
        m_depthColor = QColor(Qt::black);
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_highlightBarColor = QColor(QRgb(0xe2e2e2));
        m_highlightRowColor = QColor(QRgb(0xf2f2f2));
        m_highlightColumnColor = QColor(QRgb(0xf2f2f2));
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
#else
        m_baseColor = QColor(QRgb(0x60a6e6));
        m_heightColor = QColor(QRgb(0xfc5751));
        m_depthColor = QColor(Qt::black);
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_highlightBarColor = QColor(QRgb(0xe2e2e2));
        m_highlightRowColor = QColor(QRgb(0xf2f2f2));
        m_highlightColumnColor = QColor(QRgb(0xf2f2f2));
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
#endif
        qDebug("ThemeSystem");
        break;
    }
    case ThemeBlueCerulean: {
        m_baseColor = QColor(QRgb(0xc7e85b));
        m_heightColor = QColor(QRgb(0xee7392));
        m_depthColor = QColor(Qt::black);
        m_backgroundColor = QColor(QRgb(0x056189));
        m_windowColor = QColor(QRgb(0x101a31));
        m_textColor = QColor(QRgb(0xffffff));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_highlightBarColor = QColor(Qt::blue);
        m_highlightRowColor = QColor(Qt::darkBlue);
        m_highlightColumnColor = QColor(Qt::darkBlue);
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.2f;
        m_highlightLightStrength = 10.0f;
        m_uniformColor = true;
        qDebug("ThemeBlueCerulean");
        break;
    }
    case ThemeBlueIcy: {
        m_baseColor = QRgb(0x3daeda);
        m_heightColor = QRgb(0x2fa3b4);
        m_depthColor = QColor(Qt::lightGray);
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_highlightBarColor = QColor(Qt::white);
        m_highlightRowColor = QColor(Qt::lightGray);
        m_highlightColumnColor = QColor(Qt::lightGray);
        m_lightStrength = 5.0f;
        m_ambientStrength = 0.4f;
        m_highlightLightStrength = 8.0f;
        m_uniformColor = true;
        qDebug("ThemeBlueIcy");
        break;
    }
    case ThemeBlueNcs: {
        m_baseColor = QColor(QRgb(0x1db0da));
        m_heightColor = QColor(QRgb(0x398ca3));
        m_depthColor = QColor(Qt::lightGray);
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_highlightBarColor = QColor(Qt::lightGray);
        m_highlightRowColor = QColor(Qt::gray);
        m_highlightColumnColor = QColor(Qt::gray);
        m_lightStrength = 4.0f;
        m_ambientStrength = 0.2f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
        qDebug("ThemeBlueNcs");
        break;
    }
    case ThemeBrownSand: {
        m_baseColor = QColor(QRgb(0xb39b72));
        m_heightColor = QColor(QRgb(0x494345));
        m_depthColor = QColor(Qt::darkYellow);
        m_backgroundColor = QColor(QRgb(0xf3ece0));
        m_windowColor = QColor(QRgb(0xf3ece0));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xb5, 0xb0, 0xa7, 0x80);
        m_highlightBarColor = QColor(Qt::yellow);
        m_highlightRowColor = QColor(Qt::darkYellow);
        m_highlightColumnColor = QColor(Qt::darkYellow);
        m_lightStrength = 6.0f;
        m_ambientStrength = 0.3f;
        m_highlightLightStrength = 8.0f;
        m_uniformColor = false;
        qDebug("ThemeBrownSand");
        break;
    }
    case ThemeDark: {
        m_baseColor = QColor(QRgb(0x38ad6b));
        m_heightColor = QColor(QRgb(0xbf593e));
        m_depthColor = QColor(Qt::black);
        m_backgroundColor = QColor(QRgb(0x2e303a));
        m_windowColor = QColor(QRgb(0x121218));
        m_textColor = QColor(QRgb(0xffffff));
        m_textBackgroundColor = QColor(0x86, 0x87, 0x8c, 0x80);
        m_highlightBarColor = QColor(Qt::gray);
        m_highlightRowColor = QColor(Qt::darkGray);
        m_highlightColumnColor = QColor(Qt::darkGray);
        m_lightStrength = 6.0f;
        m_ambientStrength = 0.2f;
        m_highlightLightStrength = 8.0f;
        m_uniformColor = false;
        qDebug("ThemeDark");
        break;
    }
    case ThemeHighContrast: {
        m_baseColor = QColor(QRgb(0x202020));
        m_heightColor = QColor(QRgb(0xff4a41));
        m_depthColor = QColor(Qt::red);
        m_backgroundColor = QColor(QRgb(0xeeeeee));
        m_windowColor = QColor(QRgb(0x000000));
        m_textColor = QColor(QRgb(0x181818));
        m_textBackgroundColor = QColor(0x8c, 0x8c, 0x8c, 0x80);
        m_highlightBarColor = QColor(Qt::black);
        m_highlightRowColor = QColor(Qt::white);
        m_highlightColumnColor = QColor(Qt::white);
        m_lightStrength = 5.0f;
        m_ambientStrength = 1.0f;
        m_highlightLightStrength = 10.0f;
        m_uniformColor = false;
        qDebug("ThemeHighContrast");
        break;
    }
    case ThemeLight: {
        m_baseColor = QColor(QRgb(0x209fdf));
        m_heightColor = QColor(QRgb(0xbf593e));
        m_depthColor = QColor(Qt::lightGray);
        m_backgroundColor = QColor(QRgb(0xffffff));
        m_windowColor = QColor(QRgb(0xffffff));
        m_textColor = QColor(QRgb(0x404044));
        m_textBackgroundColor = QColor(0xd6, 0xd6, 0xd6, 0x80);
        m_highlightBarColor = QColor(Qt::white);
        m_highlightRowColor = QColor(Qt::lightGray);
        m_highlightColumnColor = QColor(Qt::lightGray);
        m_lightStrength = 3.0f;
        m_ambientStrength = 0.5f;
        m_highlightLightStrength = 6.0f;
        m_uniformColor = true;
        qDebug("ThemeLight");
        break;
    }
    default:
        break;
    }
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

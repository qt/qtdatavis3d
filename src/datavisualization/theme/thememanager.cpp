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

#include "thememanager_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

ThemeManager::ThemeManager(Abstract3DController *controller)
    : m_theme(0),
      m_controller(controller)
{
}

ThemeManager::~ThemeManager()
{
}

void ThemeManager::setTheme(Q3DTheme *theme)
{
    if (!theme)
        return;

    if (m_theme.data() != theme) {
        // Disconnect old theme signal connections
        if (m_theme) {
            disconnect(m_theme.data(), 0, m_controller, 0);
            disconnect(m_theme.data(), 0, this, 0);
        }

        m_theme.reset(theme);

        QDataVis::Theme type = m_theme->type();

        if (type != QDataVis::ThemeUserDefined)
            useTheme(type);

        // Connect signals from new one
        connectThemeSignals();
    }
}

Q3DTheme *ThemeManager::theme() const
{
    return m_theme.data();
}

void ThemeManager::connectThemeSignals()
{
    // TODO: Implement receiving end for the commented ones (QTRD-2538)
    connect(m_theme.data(), &Q3DTheme::baseColorChanged,
            m_controller, &Abstract3DController::setObjectColor);
//        connect(m_theme.data(), &Q3DTheme::backgroundColorChanged,
//                m_controller, &Abstract3DController::setBackgroundColorColor);
//        connect(m_theme.data(), &Q3DTheme::windowColorChanged,
//                m_controller, &Abstract3DController::setWindowColor);
//        connect(m_theme.data(), &Q3DTheme::textColorChanged,
//                m_controller, &Abstract3DController::setTextColor);
//        connect(m_theme.data(), &Q3DTheme::textBackgroundColorChanged,
//                m_controller, &Abstract3DController::setTextBackgroundColor);
//        connect(m_theme.data(), &Q3DTheme::gridLineColorChanged,
//                m_controller, &Abstract3DController::setGridLineColor);
    connect(m_theme.data(), &Q3DTheme::singleHighlightColorChanged,
            m_controller, &Abstract3DController::setSingleHighlightColor);
    connect(m_theme.data(), &Q3DTheme::multiHighlightColorChanged,
            m_controller, &Abstract3DController::setMultiHighlightColor);
//        connect(m_theme.data(), &Q3DTheme::lightColorChanged,
//                m_controller, &Abstract3DController::setLightColor);
    connect(m_theme.data(), &Q3DTheme::baseGradientChanged,
            m_controller, &Abstract3DController::setObjectGradient);
    connect(m_theme.data(), &Q3DTheme::singleHighlightGradientChanged,
            m_controller, &Abstract3DController::setSingleHighlightGradient);
    connect(m_theme.data(), &Q3DTheme::multiHighlightGradientChanged,
            m_controller, &Abstract3DController::setMultiHighlightGradient);
//        connect(m_theme.data(), &Q3DTheme::lightStrengthChanged,
//                m_controller, &Abstract3DController::setLightStrength);
//        connect(m_theme.data(), &Q3DTheme::ambientLightStrengthChanged,
//                m_controller, &Abstract3DController::setAmbientLightStrength);
//        connect(m_theme.data(), &Q3DTheme::highlightLightStrengthChanged,
//                m_controller, &Abstract3DController::setHighlightLightStrength);
//        connect(m_theme.data(), &Q3DTheme::labelBorderEnabledChanged,
//                m_controller, &Abstract3DController::setLabelBorderEnabled);
    connect(m_theme.data(), &Q3DTheme::colorStyleChanged,
            m_controller, &Abstract3DController::setColorStyle);

    connect(m_theme.data(), &Q3DTheme::typeChanged, this, &ThemeManager::useTheme);
}

void ThemeManager::useTheme(QDataVis::Theme type)
{
    QColor color;
    QLinearGradient gradient;

    switch (type) {
    case QDataVis::ThemeQt: {
        m_theme->setBaseColor(QColor(QRgb(0x80c342)));
        m_theme->setBackgroundColor(QColor(QRgb(0xffffff)));
        m_theme->setWindowColor(QColor(QRgb(0xffffff)));
        m_theme->setTextColor(QColor(QRgb(0x35322f)));
        m_theme->setTextBackgroundColor(QColor(0xff, 0xff, 0xff, 0x99));
        m_theme->setGridLineColor(QColor(QRgb(0xd7d6d5)));
        m_theme->setSingleHighlightColor(QColor(QRgb(0x14aaff)));
        m_theme->setMultiHighlightColor(QColor(QRgb(0x6400aa)));
        m_theme->setLightStrength(5.0f);
        m_theme->setAmbientLightStrength(0.5f);
        m_theme->setHighlightLightStrength(5.0f);
        m_theme->setLabelBorderEnabled(true);
        m_theme->setColorStyle(QDataVis::ColorStyleUniform);
        gradient = QLinearGradient(qreal(gradientTextureWidth),
                                   qreal(gradientTextureHeight),
                                   0.0, 0.0);
        color.setRed(0x80 * 0.7);
        color.setGreen(0xc3 * 0.7);
        color.setBlue(0x42 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x80c342)));
        m_theme->setBaseGradient(gradient);
        color.setRed(0x14 * 0.7);
        color.setGreen(0xaa * 0.7);
        color.setBlue(0xff * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x14aaff)));
        m_theme->setSingleHighlightGradient(gradient);
        color.setRed(0x64 * 0.7);
        color.setGreen(0x00);
        color.setBlue(0xaa * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x6400aa)));
        m_theme->setMultiHighlightGradient(gradient);
        break;
    }

    case QDataVis::ThemePrimaryColors: {
        m_theme->setBaseColor(QColor(QRgb(0xffe400)));
        m_theme->setBackgroundColor(QColor(QRgb(0xffffff)));
        m_theme->setWindowColor(QColor(QRgb(0xffffff)));
        m_theme->setTextColor(QColor(QRgb(0x000000)));
        m_theme->setTextBackgroundColor(QColor(0xff, 0xff, 0xff, 0x99));
        m_theme->setGridLineColor(QColor(QRgb(0xd7d6d5)));
        m_theme->setSingleHighlightColor(QColor(QRgb(0x27beee)));
        m_theme->setMultiHighlightColor(QColor(QRgb(0xee1414)));
        m_theme->setLightStrength(5.0f);
        m_theme->setAmbientLightStrength(0.5f);
        m_theme->setHighlightLightStrength(5.0f);
        m_theme->setLabelBorderEnabled(false);
        m_theme->setColorStyle(QDataVis::ColorStyleUniform);
        gradient = QLinearGradient(qreal(gradientTextureWidth),
                                   qreal(gradientTextureHeight),
                                   0.0, 0.0);
        color.setRed(0xff * 0.7);
        color.setGreen(0xe4 * 0.7);
        color.setBlue(0x00);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xffe400)));
        m_theme->setBaseGradient(gradient);
        color.setRed(0x27 * 0.7);
        color.setGreen(0xbe * 0.7);
        color.setBlue(0xee * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x27beee)));
        m_theme->setSingleHighlightGradient(gradient);
        color.setRed(0xee * 0.7);
        color.setGreen(0x14 * 0.7);
        color.setBlue(0x14 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xee1414)));
        m_theme->setMultiHighlightGradient(gradient);
        break;
    }

    case QDataVis::ThemeDigia: {
        m_theme->setBaseColor(QColor(QRgb(0xcccccc)));
        m_theme->setBackgroundColor(QColor(QRgb(0xffffff)));
        m_theme->setWindowColor(QColor(QRgb(0xffffff)));
        m_theme->setTextColor(QColor(QRgb(0x000000)));
        m_theme->setTextBackgroundColor(QColor(0xff, 0xff, 0xff, 0x80));
        m_theme->setGridLineColor(QColor(QRgb(0xd7d6d5)));
        m_theme->setSingleHighlightColor(QColor(QRgb(0xfa0000)));
        m_theme->setMultiHighlightColor(QColor(QRgb(0x555555)));
        m_theme->setLightStrength(5.0f);
        m_theme->setAmbientLightStrength(0.5f);
        m_theme->setHighlightLightStrength(5.0f);
        m_theme->setLabelBorderEnabled(false);
        m_theme->setColorStyle(QDataVis::ColorStyleObjectGradient);
        gradient = QLinearGradient(qreal(gradientTextureWidth),
                                   qreal(gradientTextureHeight),
                                   0.0, 0.0);
        color.setRed(0xcc * 0.7);
        color.setGreen(0xcc * 0.7);
        color.setBlue(0xcc * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xcccccc)));
        m_theme->setBaseGradient(gradient);
        color.setRed(0xfa * 0.7);
        color.setGreen(0x00);
        color.setBlue(0x00);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xfa0000)));
        m_theme->setSingleHighlightGradient(gradient);
        color.setRed(0x55 * 0.7);
        color.setGreen(0x55 * 0.7);
        color.setBlue(0x55 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x555555)));
        m_theme->setMultiHighlightGradient(gradient);
        break;
    }

    case QDataVis::ThemeStoneMoss: {
        m_theme->setBaseColor(QColor(QRgb(0xbeb32b)));
        m_theme->setBackgroundColor(QColor(QRgb(0x4d4d4f)));
        m_theme->setWindowColor(QColor(QRgb(0x4d4d4f)));
        m_theme->setTextColor(QColor(QRgb(0xffffff)));
        m_theme->setTextBackgroundColor(QColor(0x4d, 0x4d, 0x4f, 0xcd));
        m_theme->setGridLineColor(QColor(QRgb(0x3e3e40)));
        m_theme->setSingleHighlightColor(QColor(QRgb(0xfbf6d6)));
        m_theme->setMultiHighlightColor(QColor(QRgb(0x442f20)));
        m_theme->setLightStrength(5.0f);
        m_theme->setAmbientLightStrength(0.5f);
        m_theme->setHighlightLightStrength(5.0f);
        m_theme->setLabelBorderEnabled(true);
        m_theme->setColorStyle(QDataVis::ColorStyleUniform);
        gradient = QLinearGradient(qreal(gradientTextureWidth),
                                   qreal(gradientTextureHeight),
                                   0.0, 0.0);
        color.setRed(0xbe * 0.7);
        color.setGreen(0xb3 * 0.7);
        color.setBlue(0x2b * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xbeb32b)));
        m_theme->setBaseGradient(gradient);
        color.setRed(0xfb* 0.7);
        color.setGreen(0xf6 * 0.7);
        color.setBlue(0xd6 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xfbf6d6)));
        m_theme->setSingleHighlightGradient(gradient);
        color.setRed(0x44 * 0.7);
        color.setGreen(0x2f * 0.7);
        color.setBlue(0x20 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x442f20)));
        m_theme->setMultiHighlightGradient(gradient);
        break;
    }

    case QDataVis::ThemeArmyBlue: {
        m_theme->setBaseColor(QColor(QRgb(0x495f76)));
        m_theme->setBackgroundColor(QColor(QRgb(0xd5d6d7)));
        m_theme->setWindowColor(QColor(QRgb(0xd5d6d7)));
        m_theme->setTextColor(QColor(QRgb(0x000000)));
        m_theme->setTextBackgroundColor(QColor(0xd5, 0xd6, 0xd7, 0xcd));
        m_theme->setGridLineColor(QColor(QRgb(0xaeadac)));
        m_theme->setSingleHighlightColor(QColor(QRgb(0x2aa2f9)));
        m_theme->setMultiHighlightColor(QColor(QRgb(0x103753)));
        m_theme->setLightStrength(5.0f);
        m_theme->setAmbientLightStrength(0.5f);
        m_theme->setHighlightLightStrength(5.0f);
        m_theme->setLabelBorderEnabled(false);
        m_theme->setColorStyle(QDataVis::ColorStyleObjectGradient);
        gradient = QLinearGradient(qreal(gradientTextureWidth),
                                   qreal(gradientTextureHeight),
                                   0.0, 0.0);
        color.setRed(0x49 * 0.7);
        color.setGreen(0x5f * 0.7);
        color.setBlue(0x76 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x495f76)));
        m_theme->setBaseGradient(gradient);
        color.setRed(0x2a * 0.7);
        color.setGreen(0xa2 * 0.7);
        color.setBlue(0xf9 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x2aa2f9)));
        m_theme->setSingleHighlightGradient(gradient);
        color.setRed(0x10 * 0.7);
        color.setGreen(0x37 * 0.7);
        color.setBlue(0x53 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x103753)));
        m_theme->setMultiHighlightGradient(gradient);
        break;
    }

    case QDataVis::ThemeRetro: {
        m_theme->setBaseColor(QColor(QRgb(0x533b23)));
        m_theme->setBackgroundColor(QColor(QRgb(0xe9e2ce)));
        m_theme->setWindowColor(QColor(QRgb(0xe9e2ce)));
        m_theme->setTextColor(QColor(QRgb(0x000000)));
        m_theme->setTextBackgroundColor(QColor(0xe9, 0xe2, 0xce, 0xc0));
        m_theme->setGridLineColor(QColor(QRgb(0xd0c0b0)));
        m_theme->setSingleHighlightColor(QColor(QRgb(0x8ea317)));
        m_theme->setMultiHighlightColor(QColor(QRgb(0xc25708)));
        m_theme->setLightStrength(5.0f);
        m_theme->setAmbientLightStrength(0.5f);
        m_theme->setHighlightLightStrength(5.0f);
        m_theme->setLabelBorderEnabled(false);
        m_theme->setColorStyle(QDataVis::ColorStyleObjectGradient);
        gradient = QLinearGradient(qreal(gradientTextureWidth),
                                   qreal(gradientTextureHeight),
                                   0.0, 0.0);
        color.setRed(0x53 * 0.7);
        color.setGreen(0x3b * 0.7);
        color.setBlue(0x23 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x533b23)));
        m_theme->setBaseGradient(gradient);
        color.setRed(0x8e * 0.7);
        color.setGreen(0xa3 * 0.7);
        color.setBlue(0x17 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0x8ea317)));
        m_theme->setSingleHighlightGradient(gradient);
        color.setRed(0xc2 * 0.7);
        color.setGreen(0x57 * 0.7);
        color.setBlue(0x08 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xc25708)));
        m_theme->setMultiHighlightGradient(gradient);
        break;
    }

    case QDataVis::ThemeEbony: {
        m_theme->setBaseColor(QColor(QRgb(0xffffff)));
        m_theme->setBackgroundColor(QColor(QRgb(0x000000)));
        m_theme->setWindowColor(QColor(QRgb(0x000000)));
        m_theme->setTextColor(QColor(QRgb(0xaeadac)));
        m_theme->setTextBackgroundColor(QColor(0x00, 0x00, 0x00, 0xcd));
        m_theme->setGridLineColor(QColor(QRgb(0x35322f)));
        m_theme->setSingleHighlightColor(QColor(QRgb(0xf5dc0d)));
        m_theme->setMultiHighlightColor(QColor(QRgb(0xd72222)));
        m_theme->setLightStrength(5.0f);
        m_theme->setAmbientLightStrength(0.5f);
        m_theme->setHighlightLightStrength(5.0f);
        m_theme->setLabelBorderEnabled(false);
        m_theme->setColorStyle(QDataVis::ColorStyleUniform);
        gradient = QLinearGradient(qreal(gradientTextureWidth),
                                   qreal(gradientTextureHeight),
                                   0.0, 0.0);
        color.setRed(0xff * 0.7);
        color.setGreen(0xff * 0.7);
        color.setBlue(0xff * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xffffff)));
        m_theme->setBaseGradient(gradient);
        color.setRed(0xf5 * 0.7);
        color.setGreen(0xdc * 0.7);
        color.setBlue(0x0d * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xf5dc0d)));
        m_theme->setSingleHighlightGradient(gradient);
        color.setRed(0x72 * 0.7);
        color.setGreen(0x22 * 0.7);
        color.setBlue(0x22 * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xd72222)));
        m_theme->setMultiHighlightGradient(gradient);
        break;
    }

    case QDataVis::ThemeIsabelle: {
        m_theme->setBaseColor(QColor(QRgb(0xf9d900)));
        m_theme->setBackgroundColor(QColor(QRgb(0x000000)));
        m_theme->setWindowColor(QColor(QRgb(0x000000)));
        m_theme->setTextColor(QColor(QRgb(0xaeadac)));
        m_theme->setTextBackgroundColor(QColor(0x00, 0x00, 0x00, 0xc0));
        m_theme->setGridLineColor(QColor(QRgb(0x35322f)));
        m_theme->setSingleHighlightColor(QColor(QRgb(0xfff7cc)));
        m_theme->setMultiHighlightColor(QColor(QRgb(0xde0a0a)));
        m_theme->setLightStrength(5.0f);
        m_theme->setAmbientLightStrength(0.5f);
        m_theme->setHighlightLightStrength(5.0f);
        m_theme->setLabelBorderEnabled(false);
        m_theme->setColorStyle(QDataVis::ColorStyleUniform);
        gradient = QLinearGradient(qreal(gradientTextureWidth),
                                   qreal(gradientTextureHeight),
                                   0.0, 0.0);
        color.setRed(0xf9 * 0.7);
        color.setGreen(0xd9 * 0.7);
        color.setBlue(0x00);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xf9d900)));
        m_theme->setBaseGradient(gradient);
        color.setRed(0xff * 0.7);
        color.setGreen(0xf7 * 0.7);
        color.setBlue(0xcc * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xfff7cc)));
        m_theme->setSingleHighlightGradient(gradient);
        color.setRed(0xde * 0.7);
        color.setGreen(0x0a * 0.7);
        color.setBlue(0x0a * 0.7);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, QColor(QRgb(0xde0a0a)));
        m_theme->setMultiHighlightGradient(gradient);
        break;
    }
    default:
        break;
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE

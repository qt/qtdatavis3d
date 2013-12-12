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
#include "q3dtheme_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

const float defaultBuiltInColorLevel = 0.7f; // for built-in gradient themes
const float defaultColorLevel = 0.5f; // for built-in uniform themes

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
            disconnect(m_theme->d_ptr.data(), 0, m_controller, 0);
            disconnect(m_theme.data(), 0, m_controller, 0);
            disconnect(m_theme.data(), 0, this, 0);
        }

        m_theme.reset(theme);

        Q3DTheme::Theme type = m_theme->type();

        if (type != Q3DTheme::ThemeUserDefined) {
            useTheme(type);
            // Reset all bits to dirty for sync
            m_theme->d_ptr->resetDirtyBits();
        }

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
    connect(m_theme.data(), &Q3DTheme::colorStyleChanged,
            m_controller, &Abstract3DController::handleThemeColorStyleChanged);
    connect(m_theme.data(), &Q3DTheme::baseColorsChanged,
            m_controller, &Abstract3DController::handleThemeBaseColorsChanged);
    connect(m_theme.data(), &Q3DTheme::singleHighlightColorChanged,
            m_controller, &Abstract3DController::handleThemeSingleHighlightColorChanged);
    connect(m_theme.data(), &Q3DTheme::multiHighlightColorChanged,
            m_controller, &Abstract3DController::handleThemeMultiHighlightColorChanged);
    connect(m_theme.data(), &Q3DTheme::baseGradientsChanged,
            m_controller, &Abstract3DController::handleThemeBaseGradientsChanged);
    connect(m_theme.data(), &Q3DTheme::singleHighlightGradientChanged,
            m_controller, &Abstract3DController::handleThemeSingleHighlightGradientChanged);
    connect(m_theme.data(), &Q3DTheme::multiHighlightGradientChanged,
            m_controller, &Abstract3DController::handleThemeMultiHighlightGradientChanged);

    connect(m_theme->d_ptr.data(), &Q3DThemePrivate::needRender,
            m_controller, &Abstract3DController::needRender);

    connect(m_theme.data(), &Q3DTheme::typeChanged, this, &ThemeManager::useTheme);
}

void ThemeManager::useTheme(Q3DTheme::Theme type)
{
    switch (type) {
    case Q3DTheme::ThemeQt: {
        QList<QColor> baseColors;
        baseColors.append(QColor(QRgb(0x80c342)));
        baseColors.append(QColor(QRgb(0x328930)));
        baseColors.append(QColor(QRgb(0x006325)));
        baseColors.append(QColor(QRgb(0x35322f)));
        baseColors.append(QColor(QRgb(0x5d5b59)));

        QList<QLinearGradient> baseGradients;
        baseGradients.append(createGradient(baseColors.at(0), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(1), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(2), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(3), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(4), defaultColorLevel));

        setBackgroundEnabled(true);
        setGridEnabled(true);
        setFont(QFont(QStringLiteral("Arial")));
        setLabelBackgroundEnabled(true);
        setLightColor(Qt::white);
        setBaseColors(baseColors);
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
        setColorStyle(Q3DTheme::ColorStyleUniform);
        setBaseGradients(baseGradients);
        setSingleHighlightGradient(createGradient(QColor(QRgb(0x14aaff)), defaultColorLevel));
        setMultiHighlightGradient(createGradient(QColor(QRgb(0x6400aa)), defaultColorLevel));
        break;
    }

    case Q3DTheme::ThemePrimaryColors: {
        QList<QColor> baseColors;
        baseColors.append(QColor(QRgb(0xffe400)));
        baseColors.append(QColor(QRgb(0xdfd400)));
        baseColors.append(QColor(QRgb(0xbfc400)));
        baseColors.append(QColor(QRgb(0x9fb400)));
        baseColors.append(QColor(QRgb(0x7fa400)));

        QList<QLinearGradient> baseGradients;
        baseGradients.append(createGradient(baseColors.at(0), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(1), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(2), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(3), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(4), defaultColorLevel));

        setBackgroundEnabled(true);
        setGridEnabled(true);
        setFont(QFont(QStringLiteral("Arial")));
        setLabelBackgroundEnabled(true);
        setLightColor(Qt::white);
        setBaseColors(baseColors);
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
        setColorStyle(Q3DTheme::ColorStyleUniform);
        setBaseGradients(baseGradients);
        setSingleHighlightGradient(createGradient(QColor(QRgb(0x27beee)), defaultColorLevel));
        setMultiHighlightGradient(createGradient(QColor(QRgb(0xee1414)), defaultColorLevel));
        break;
    }

    case Q3DTheme::ThemeDigia: {
        QList<QColor> baseColors;
        baseColors.append(QColor(QRgb(0xcccccc)));
        baseColors.append(QColor(QRgb(0xbbbbbb)));
        baseColors.append(QColor(QRgb(0xaaaaaa)));
        baseColors.append(QColor(QRgb(0x999999)));
        baseColors.append(QColor(QRgb(0x888888)));

        QList<QLinearGradient> baseGradients;
        baseGradients.append(createGradient(baseColors.at(0), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(1), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(2), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(3), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(4), defaultBuiltInColorLevel));

        setBackgroundEnabled(true);
        setGridEnabled(true);
        setFont(QFont(QStringLiteral("Arial")));
        setLabelBackgroundEnabled(true);
        setLightColor(Qt::white);
        setBaseColors(baseColors);
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
        setColorStyle(Q3DTheme::ColorStyleObjectGradient);
        setBaseGradients(baseGradients);
        setSingleHighlightGradient(createGradient(QColor(QRgb(0xfa0000)), defaultBuiltInColorLevel));
        setMultiHighlightGradient(createGradient(QColor(QRgb(0x555555)), defaultBuiltInColorLevel));
        break;
    }

    case Q3DTheme::ThemeStoneMoss: {
        QList<QColor> baseColors;
        baseColors.append(QColor(QRgb(0xbeb32b)));
        baseColors.append(QColor(QRgb(0xcec33b)));
        baseColors.append(QColor(QRgb(0xded34b)));
        baseColors.append(QColor(QRgb(0xeee35b)));
        baseColors.append(QColor(QRgb(0xfef36b)));

        QList<QLinearGradient> baseGradients;
        baseGradients.append(createGradient(baseColors.at(0), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(1), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(2), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(3), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(4), defaultColorLevel));

        setBackgroundEnabled(true);
        setGridEnabled(true);
        setFont(QFont(QStringLiteral("Arial")));
        setLabelBackgroundEnabled(true);
        setLightColor(Qt::white);
        setBaseColors(baseColors);
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
        setColorStyle(Q3DTheme::ColorStyleUniform);
        setBaseGradients(baseGradients);
        setSingleHighlightGradient(createGradient(QColor(QRgb(0xfbf6d6)), defaultColorLevel));
        setMultiHighlightGradient(createGradient(QColor(QRgb(0x442f20)), defaultColorLevel));
        break;
    }

    case Q3DTheme::ThemeArmyBlue: {
        QList<QColor> baseColors;
        baseColors.append(QColor(QRgb(0x495f76)));
        baseColors.append(QColor(QRgb(0x495f86)));
        baseColors.append(QColor(QRgb(0x495f96)));
        baseColors.append(QColor(QRgb(0x495fa6)));
        baseColors.append(QColor(QRgb(0x495fb6)));

        QList<QLinearGradient> baseGradients;
        baseGradients.append(createGradient(baseColors.at(0), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(1), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(2), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(3), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(4), defaultBuiltInColorLevel));

        setBackgroundEnabled(true);
        setGridEnabled(true);
        setFont(QFont(QStringLiteral("Arial")));
        setLabelBackgroundEnabled(true);
        setLightColor(Qt::white);
        setBaseColors(baseColors);
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
        setColorStyle(Q3DTheme::ColorStyleObjectGradient);
        setBaseGradients(baseGradients);
        setSingleHighlightGradient(createGradient(QColor(QRgb(0x2aa2f9)), defaultBuiltInColorLevel));
        setMultiHighlightGradient(createGradient(QColor(QRgb(0x103753)), defaultBuiltInColorLevel));
        break;
    }

    case Q3DTheme::ThemeRetro: {
        QList<QColor> baseColors;
        baseColors.append(QColor(QRgb(0x533b23)));
        baseColors.append(QColor(QRgb(0x633b23)));
        baseColors.append(QColor(QRgb(0x733b23)));
        baseColors.append(QColor(QRgb(0x833b23)));
        baseColors.append(QColor(QRgb(0x933b23)));

        QList<QLinearGradient> baseGradients;
        baseGradients.append(createGradient(baseColors.at(0), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(1), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(2), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(3), defaultBuiltInColorLevel));
        baseGradients.append(createGradient(baseColors.at(4), defaultBuiltInColorLevel));

        setBackgroundEnabled(true);
        setGridEnabled(true);
        setFont(QFont(QStringLiteral("Arial")));
        setLabelBackgroundEnabled(true);
        setLightColor(Qt::white);
        setBaseColors(baseColors);
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
        setColorStyle(Q3DTheme::ColorStyleObjectGradient);
        setBaseGradients(baseGradients);
        setSingleHighlightGradient(createGradient(QColor(QRgb(0x8ea317)), defaultBuiltInColorLevel));
        setMultiHighlightGradient(createGradient(QColor(QRgb(0xc25708)), defaultBuiltInColorLevel));
        break;
    }

    case Q3DTheme::ThemeEbony: {
        QList<QColor> baseColors;
        baseColors.append(QColor(QRgb(0xffffff)));
        baseColors.append(QColor(QRgb(0xeeeeee)));
        baseColors.append(QColor(QRgb(0xdddddd)));
        baseColors.append(QColor(QRgb(0xcccccc)));
        baseColors.append(QColor(QRgb(0xbbbbbb)));

        QList<QLinearGradient> baseGradients;
        baseGradients.append(createGradient(baseColors.at(0), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(1), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(2), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(3), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(4), defaultColorLevel));

        setBackgroundEnabled(true);
        setGridEnabled(true);
        setFont(QFont(QStringLiteral("Arial")));
        setLabelBackgroundEnabled(true);
        setLightColor(Qt::white);
        setBaseColors(baseColors);
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
        setColorStyle(Q3DTheme::ColorStyleUniform);
        setBaseGradients(baseGradients);
        setSingleHighlightGradient(createGradient(QColor(QRgb(0xf5dc0d)), defaultColorLevel));
        setMultiHighlightGradient(createGradient(QColor(QRgb(0xd72222)), defaultColorLevel));
        break;
    }

    case Q3DTheme::ThemeIsabelle: {
        QList<QColor> baseColors;
        baseColors.append(QColor(QRgb(0xf9d900)));
        baseColors.append(QColor(QRgb(0xf9d910)));
        baseColors.append(QColor(QRgb(0xf9d920)));
        baseColors.append(QColor(QRgb(0xf9d930)));
        baseColors.append(QColor(QRgb(0xf9d940)));

        QList<QLinearGradient> baseGradients;
        baseGradients.append(createGradient(baseColors.at(0), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(1), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(2), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(3), defaultColorLevel));
        baseGradients.append(createGradient(baseColors.at(4), defaultColorLevel));

        setBackgroundEnabled(true);
        setGridEnabled(true);
        setFont(QFont(QStringLiteral("Arial")));
        setLabelBackgroundEnabled(true);
        setLightColor(Qt::white);
        setBaseColors(baseColors);
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
        setColorStyle(Q3DTheme::ColorStyleUniform);
        setBaseGradients(baseGradients);
        setSingleHighlightGradient(createGradient(QColor(QRgb(0xfff7cc)), defaultColorLevel));
        setMultiHighlightGradient(createGradient(QColor(QRgb(0xde0a0a)), defaultColorLevel));
        break;
    }
    default:
        break;
    }
}

QLinearGradient ThemeManager::createGradient(const QColor &color, float colorLevel)
{
    QColor startColor;
    QLinearGradient gradient = QLinearGradient(qreal(gradientTextureWidth),
                                               qreal(gradientTextureHeight),
                                               0.0, 0.0);;
    startColor.setRed(color.red() * colorLevel);
    startColor.setGreen(color.green() * colorLevel);
    startColor.setBlue(color.blue() * colorLevel);
    gradient.setColorAt(0.0, startColor);
    gradient.setColorAt(1.0, color);
    return gradient;
}

void ThemeManager::setBaseColors(const QList<QColor> &colors)
{
    if (!m_theme->d_ptr->m_dirtyBits.baseColorDirty)
        m_theme->setBaseColors(colors);
}

void ThemeManager::setBackgroundColor(const QColor &color)
{
    if (!m_theme->d_ptr->m_dirtyBits.backgroundColorDirty)
        m_theme->setBackgroundColor(color);
}

void ThemeManager::setWindowColor(const QColor &color)
{
    if (!m_theme->d_ptr->m_dirtyBits.windowColorDirty)
        m_theme->setWindowColor(color);
}

void ThemeManager::setTextColor(const QColor &color)
{
    if (!m_theme->d_ptr->m_dirtyBits.labelTextColorDirty)
        m_theme->setLabelTextColor(color);
}

void ThemeManager::setTextBackgroundColor(const QColor &color)
{
    if (!m_theme->d_ptr->m_dirtyBits.labelBackgroundColorDirty)
        m_theme->setLabelBackgroundColor(color);
}

void ThemeManager::setGridLineColor(const QColor &color)
{
    if (!m_theme->d_ptr->m_dirtyBits.gridLineColorDirty)
        m_theme->setGridLineColor(color);
}

void ThemeManager::setSingleHighlightColor(const QColor &color)
{
    if (!m_theme->d_ptr->m_dirtyBits.singleHighlightColorDirty)
        m_theme->setSingleHighlightColor(color);
}

void ThemeManager::setMultiHighlightColor(const QColor &color)
{
    if (!m_theme->d_ptr->m_dirtyBits.multiHighlightColorDirty)
        m_theme->setMultiHighlightColor(color);
}

void ThemeManager::setLightColor(const QColor &color)
{
    if (!m_theme->d_ptr->m_dirtyBits.lightColorDirty)
        m_theme->setLightColor(color);
}

void ThemeManager::setBaseGradients(const QList<QLinearGradient> &gradients)
{
    if (!m_theme->d_ptr->m_dirtyBits.baseGradientDirty)
        m_theme->setBaseGradients(gradients);
}

void ThemeManager::setSingleHighlightGradient(const QLinearGradient &gradient)
{
    if (!m_theme->d_ptr->m_dirtyBits.singleHighlightGradientDirty)
        m_theme->setSingleHighlightGradient(gradient);
}

void ThemeManager::setMultiHighlightGradient(const QLinearGradient &gradient)
{
    if (!m_theme->d_ptr->m_dirtyBits.multiHighlightGradientDirty)
        m_theme->setMultiHighlightGradient(gradient);
}

void ThemeManager::setLightStrength(float strength)
{
    if (!m_theme->d_ptr->m_dirtyBits.lightStrengthDirty)
        m_theme->setLightStrength(strength);
}

void ThemeManager::setAmbientLightStrength(float strength)
{
    if (!m_theme->d_ptr->m_dirtyBits.ambientLightStrengthDirty)
        m_theme->setAmbientLightStrength(strength);
}

void ThemeManager::setHighlightLightStrength(float strength)
{
    if (!m_theme->d_ptr->m_dirtyBits.highlightLightStrengthDirty)
        m_theme->setHighlightLightStrength(strength);
}

void ThemeManager::setLabelBorderEnabled(bool enabled)
{
    if (!m_theme->d_ptr->m_dirtyBits.labelBorderEnabledDirty)
        m_theme->setLabelBorderEnabled(enabled);
}

void ThemeManager::setFont(const QFont &font)
{
    if (!m_theme->d_ptr->m_dirtyBits.fontDirty)
        m_theme->setFont(font);
}

void ThemeManager::setBackgroundEnabled(bool enabled)
{
    if (!m_theme->d_ptr->m_dirtyBits.backgroundEnabledDirty)
        m_theme->setBackgroundEnabled(enabled);
}

void ThemeManager::setGridEnabled(bool enabled)
{
    if (!m_theme->d_ptr->m_dirtyBits.gridEnabledDirty)
        m_theme->setGridEnabled(enabled);
}

void ThemeManager::setLabelBackgroundEnabled(bool enabled)
{
    if (!m_theme->d_ptr->m_dirtyBits.labelBackgroundEnabledDirty)
        m_theme->setLabelBackgroundEnabled(enabled);
}

void ThemeManager::setColorStyle(Q3DTheme::ColorStyle style)
{
    if (!m_theme->d_ptr->m_dirtyBits.colorStyleDirty)
        m_theme->setColorStyle(style);
}

QT_DATAVISUALIZATION_END_NAMESPACE

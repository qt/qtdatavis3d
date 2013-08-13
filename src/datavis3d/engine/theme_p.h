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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef THEME_P_H
#define THEME_P_H

#include "datavis3dglobal_p.h"
#include "q3dbars.h"
#include <QLinearGradient>

class QColor;

QT_DATAVIS3D_BEGIN_NAMESPACE

class Theme
{
public:
    explicit Theme();
    ~Theme();

    void useColorTheme(ColorTheme theme);
    ColorTheme colorTheme();
    void setFromTheme(Theme &theme);

private:
    friend class Abstract3DController;
    friend class Abstract3DRenderer;
    friend class Bars3dRenderer;
    friend class Maps3DController;
    friend class Surface3dRenderer;
    friend class Surface3dController;
    friend class Scatter3DRenderer;
    friend class Drawer;

    ColorTheme m_colorTheme;
    QColor m_baseColor;
    QColor m_heightColor;
    QColor m_depthColor;
    QColor m_backgroundColor;
    QColor m_windowColor;
    QColor m_textColor;
    QColor m_textBackgroundColor;
    QColor m_gridLine;
    QColor m_highlightBarColor;
    QColor m_highlightRowColor;
    QColor m_highlightColumnColor;
    QLinearGradient m_surfaceGradient;
    float m_lightStrength;
    float m_ambientStrength;
    float m_highlightLightStrength;
    bool m_uniformColor;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

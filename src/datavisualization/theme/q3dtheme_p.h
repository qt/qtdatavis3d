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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q3DTHEME_P_H
#define Q3DTHEME_P_H

#include "datavisualizationglobal_p.h"
#include "q3dtheme.h"

class QColor;
class QLinearGradient;

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DThemePrivate : public QObject
{
    Q_OBJECT
public:
    Q3DThemePrivate(Q3DTheme *q,
                    QDataVis::Theme theme_id = QDataVis::ThemeUserDefined);
    virtual ~Q3DThemePrivate();

public:
    QDataVis::Theme m_themeId;

    QColor m_baseColor;
    QColor m_heightColor;
    QColor m_depthColor;
    QColor m_backgroundColor;
    QColor m_windowColor;
    QColor m_textColor;
    QColor m_textBackgroundColor;
    QColor m_gridLineColor;
    QColor m_singleHighlightColor;
    QColor m_multiHighlightColor;
    QColor m_lightColor;
    QLinearGradient m_baseGradient;
    QLinearGradient m_singleHighlightGradient;
    QLinearGradient m_multiHighlightGradient;
    float m_lightStrength;
    float m_ambientLightStrength;
    float m_highlightLightStrength;
    bool m_labelBorders;
    QDataVis::ColorStyle m_colorStyle;

protected:
    Q3DTheme *q_ptr;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

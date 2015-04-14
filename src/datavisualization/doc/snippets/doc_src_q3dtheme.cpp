/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include <QtDataVisualization/Q3DBars>
#include <QtDataVisualization/Q3DTheme>

using namespace QtDataVisualization;

int main(int argc, char **argv)
{
    //! [0]
    Q3DTheme *theme = new Q3DTheme(Q3DTheme::ThemeQt);
    //! [0]

    //! [1]
    Q3DTheme *theme = new Q3DTheme(Q3DTheme::ThemeQt);
    theme->setBackgroundEnabled(false);
    theme->setLabelBackgroundEnabled(false);
    //! [1]

    //! [2]
    Q3DTheme *theme = new Q3DTheme();
    theme->setAmbientLightStrength(0.3f);
    theme->setBackgroundColor(QColor(QRgb(0x99ca53)));
    theme->setBackgroundEnabled(true);
    theme->setBaseColor(QColor(QRgb(0x209fdf)));
    theme->setColorStyle(Q3DTheme::ColorStyleUniform);
    theme->setFont(QFont(QStringLiteral("Impact"), 35));
    theme->setGridEnabled(true);
    theme->setGridLineColor(QColor(QRgb(0x99ca53)));
    theme->setHighlightLightStrength(7.0f);
    theme->setLabelBackgroundColor(QColor(0xf6, 0xa6, 0x25, 0xa0));
    theme->setLabelBackgroundEnabled(true);
    theme->setLabelBorderEnabled(true);
    theme->setLabelTextColor(QColor(QRgb(0x404044)));
    theme->setLightColor(Qt::white);
    theme->setLightStrength(6.0f);
    theme->setMultiHighlightColor(QColor(QRgb(0x6d5fd5)));
    theme->setSingleHighlightColor(QColor(QRgb(0xf6a625)));
    theme->setWindowColor(QColor(QRgb(0xffffff)));
    //! [2]

    //! [3]
    Q3DBars *graph = new Q3DBars();
    graph->activeTheme()->setType(Q3DTheme::ThemePrimaryColors);
    graph->activeTheme()->setBaseColor(Qt::red);
    graph->activeTheme()->setSingleHighlightColor(Qt::yellow);
    //! [3]
}

//! [4]
Scatter3D {
    ...
    theme: Theme3D { type: Theme3D.ThemeRetro }
    ...
}
//! [4]

//! [5]
Bars3D {
    ...
    theme: Theme3D {
        type: Theme3D.ThemeRetro
        labelBorderEnabled: true
        font.pointSize: 35
        labelBackgroundEnabled: false
    }
    ...
}
//! [5]

//! [6]
Surface3D {
    ...
    theme: Theme3D {
        ambientLightStrength: 0.5
        backgroundColor: "red"
        backgroundEnabled: true
        baseColor: "blue"
        colorStyle: Theme3D.ColorStyleUniform
        font.family: "Lucida Handwriting"
        font.pointSize: 35
        gridEnabled: false
        gridLineColor: "black"
        highlightLightStrength: 0.5
        labelBackgroundColor: "black"
        labelBackgroundEnabled: true
        labelBorderEnabled: false
        labelTextColor: "white"
        lightColor: "yellow"
        lightStrength: 0.4
        multiHighlightColor: "green"
        singleHighlightColor: "darkRed"
        windowColor: "white"
    }
    ...
}
//! [6]

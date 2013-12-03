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

#ifndef DECLARATIVETHEME_P_H
#define DECLARATIVETHEME_P_H

#include "datavisualizationglobal_p.h"
#include "colorgradient_p.h"
#include "q3dtheme.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class DeclarativeTheme3D : public Q3DTheme
{
    Q_OBJECT
    Q_PROPERTY(ColorGradient *baseGradient READ baseGradient WRITE setBaseGradient NOTIFY baseGradientChanged)
    Q_PROPERTY(ColorGradient *singleHighlightGradient READ singleHighlightGradient WRITE setSingleHighlightGradient NOTIFY singleHighlightGradientChanged)
    Q_PROPERTY(ColorGradient *multiHighlightGradient READ multiHighlightGradient WRITE setMultiHighlightGradient NOTIFY multiHighlightGradientChanged)

public:
    DeclarativeTheme3D(QObject *parent = 0);
    virtual ~DeclarativeTheme3D();

    void setBaseGradient(ColorGradient *gradient);
    ColorGradient *baseGradient() const;

    void setSingleHighlightGradient(ColorGradient *gradient);
    ColorGradient *singleHighlightGradient() const;

    void setMultiHighlightGradient(ColorGradient *gradient);
    ColorGradient *multiHighlightGradient() const;

signals:
    void baseGradientChanged(ColorGradient *gradient);
    void singleHighlightGradientChanged(ColorGradient *gradient);
    void multiHighlightGradientChanged(ColorGradient *gradient);

protected:
    void handleBaseGradientUpdate();
    void handleSingleHLGradientUpdate();
    void handleMultiHLGradientUpdate();

    enum GradientType {
        GradientTypeBase = 0,
        GradientTypeSingleHL,
        GradientTypeMultiHL
    };

private:
    void setThemeGradient(const ColorGradient &gradient, GradientType type);

    ColorGradient *m_baseGradient; // Not owned
    ColorGradient *m_singleHLGradient; // Not owned
    ColorGradient *m_multiHLGradient; // Not owned
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

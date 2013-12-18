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

#ifndef BUILTIN3DTHEMES_P_H
#define BUILTIN3DTHEMES_P_H

#include "datavisualizationglobal_p.h"
#include "q3dtheme_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class BuiltIn3DThemeQt : public Q3DTheme
{
public:
    BuiltIn3DThemeQt();
};

class BuiltIn3DThemePrimaryColors : public Q3DTheme
{
public:
    BuiltIn3DThemePrimaryColors();
};

class BuiltIn3DThemeDigia : public Q3DTheme
{
public:
    BuiltIn3DThemeDigia();
};

class BuiltIn3DThemeStoneMoss : public Q3DTheme
{
public:
    BuiltIn3DThemeStoneMoss();
};

class BuiltIn3DThemeArmyBlue : public Q3DTheme
{
public:
    BuiltIn3DThemeArmyBlue();
};

class BuiltIn3DThemeRetro : public Q3DTheme
{
public:
    BuiltIn3DThemeRetro();
};

class BuiltIn3DThemeEbony : public Q3DTheme
{
public:
    BuiltIn3DThemeEbony();
};

class BuiltIn3DThemeIsabelle : public Q3DTheme
{
public:
    BuiltIn3DThemeIsabelle();
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

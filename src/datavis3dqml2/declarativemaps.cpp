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

#include "declarativemaps.h"

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

DeclarativeMaps::DeclarativeMaps()
{
}

DeclarativeMaps::~DeclarativeMaps()
{
}

void DeclarativeMaps::setSelMode(DeclarativeMaps::SelectionMode mode)
{
    setSelectionMode(QtDataVis3D::SelectionMode(mode));
}

DeclarativeMaps::SelectionMode DeclarativeMaps::selMode()
{
    return DeclarativeMaps::SelectionMode(selectionMode());
}

void DeclarativeMaps::setTransparency(DeclarativeMaps::LabelTransparency transparency)
{
    setLabelTransparency(QtDataVis3D::LabelTransparency(transparency));
}

DeclarativeMaps::LabelTransparency DeclarativeMaps::transparency()
{
    return DeclarativeMaps::LabelTransparency(labelTransparency());
}

void DeclarativeMaps::setShadow(DeclarativeMaps::ShadowQuality quality)
{
    setShadowQuality(QtDataVis3D::ShadowQuality(quality));
}

DeclarativeMaps::ShadowQuality DeclarativeMaps::shadow()
{
    return DeclarativeMaps::ShadowQuality(shadowQuality());
}

QT_DATAVIS3D_END_NAMESPACE

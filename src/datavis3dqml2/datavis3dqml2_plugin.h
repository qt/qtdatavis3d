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

#ifndef DATAVIS3DQML2_PLUGIN_H
#define DATAVIS3DQML2_PLUGIN_H

#include "qdatavis3dglobal.h"
#include "qdatavis3namespace.h"
#include "qdataitem.h"
#include "qdatarow.h"
#include "qdataset.h"

#include "declarativebars.h"
#include "declarativemaps.h"
//#include "declarativedataitem.h"
//#include "declarativedatarow.h"
//#include "declarativedataset.h"

#include <QQmlExtensionPlugin>

QT_DATAVIS3D_USE_NAMESPACE

//Q_DECLARE_METATYPE(DeclarativeDataItem *)
//Q_DECLARE_METATYPE(DeclarativeDataRow *)
//Q_DECLARE_METATYPE(DeclarativeDataSet *)

Q_DECLARE_METATYPE(DeclarativeBars *)
Q_DECLARE_METATYPE(DeclarativeMaps *)

Q_DECLARE_METATYPE(QDataItem *)
Q_DECLARE_METATYPE(QDataRow *)
Q_DECLARE_METATYPE(QDataSet *)

QT_DATAVIS3D_BEGIN_NAMESPACE

class Datavis3dqml2Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // DATAVIS3DQML2_PLUGIN_H


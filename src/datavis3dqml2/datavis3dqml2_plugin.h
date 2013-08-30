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

#ifndef DATAVIS3DQML2_PLUGIN_H
#define DATAVIS3DQML2_PLUGIN_H

#include "datavis3dglobal_p.h"
#include "declarativebars_p.h"
#include "declarativemaps_p.h"
#include "declarativescatter_p.h"
#include "qitemmodelbardatamapping.h"
#include "qitemmodelmapdatamapping.h"
#include "qitemmodelscatterdatamapping.h"
#include "qvalueaxis.h"
#include "qcategoryaxis.h"

#include <QQmlExtensionPlugin>

QT_DATAVIS3D_USE_NAMESPACE

Q_DECLARE_METATYPE(DeclarativeBars *)
Q_DECLARE_METATYPE(DeclarativeMaps *)
Q_DECLARE_METATYPE(DeclarativeScatter *)

Q_DECLARE_METATYPE(QItemModelBarDataMapping *)
Q_DECLARE_METATYPE(QItemModelMapDataMapping *)
Q_DECLARE_METATYPE(QItemModelScatterDataMapping *)
Q_DECLARE_METATYPE(QAbstractItemModel *)
Q_DECLARE_METATYPE(QDataVis *)

Q_DECLARE_METATYPE(QAbstractAxis *)
Q_DECLARE_METATYPE(QCategoryAxis *)
Q_DECLARE_METATYPE(QValueAxis *)

QT_DATAVIS3D_BEGIN_NAMESPACE

class Datavis3Dqml2Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // DATAVIS3DQML2_PLUGIN_H


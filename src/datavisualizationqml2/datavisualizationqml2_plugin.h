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

#ifndef DATAVISUALIZATIONQML2_PLUGIN_H
#define DATAVISUALIZATIONQML2_PLUGIN_H

#include "datavisualizationglobal_p.h"
#include "declarativebars_p.h"
#include "declarativescatter_p.h"
#include "declarativesurface_p.h"
#include "qitemmodelbardatamapping.h"
#include "qitemmodelscatterdatamapping.h"
#include "qitemmodelsurfacedatamapping.h"
#include "qitemmodelbardataproxy.h"
#include "qitemmodelscatterdataproxy.h"
#include "qitemmodelsurfacedataproxy.h"
#include "qheightmapsurfacedataproxy.h"
#include "q3dvalueaxis.h"
#include "q3dcategoryaxis.h"

#include <QQmlExtensionPlugin>

QT_DATAVISUALIZATION_USE_NAMESPACE

Q_DECLARE_METATYPE(DeclarativeBars *)
Q_DECLARE_METATYPE(DeclarativeScatter *)
Q_DECLARE_METATYPE(DeclarativeSurface *)

Q_DECLARE_METATYPE(QItemModelBarDataMapping *)
Q_DECLARE_METATYPE(QItemModelScatterDataMapping *)
Q_DECLARE_METATYPE(QItemModelSurfaceDataMapping *)

Q_DECLARE_METATYPE(const QAbstractItemModel *)

Q_DECLARE_METATYPE(QDataVis *)

Q_DECLARE_METATYPE(Q3DAbstractAxis *)
Q_DECLARE_METATYPE(Q3DCategoryAxis *)
Q_DECLARE_METATYPE(Q3DValueAxis *)

Q_DECLARE_METATYPE(QAbstractDataProxy *)
Q_DECLARE_METATYPE(QBarDataProxy *)
Q_DECLARE_METATYPE(QItemModelBarDataProxy *)
Q_DECLARE_METATYPE(QScatterDataProxy *)
Q_DECLARE_METATYPE(QItemModelScatterDataProxy *)
Q_DECLARE_METATYPE(QSurfaceDataProxy *)
Q_DECLARE_METATYPE(QItemModelSurfaceDataProxy *)
Q_DECLARE_METATYPE(QHeightMapSurfaceDataProxy *)

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Datavis3Dqml2Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // DATAVISUALIZATIONQML2_PLUGIN_H


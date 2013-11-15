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
#include "q3dobject.h"
#include "q3dcamera.h"
#include "q3dscene.h"
#include "qabstract3dseries.h"
#include "qbar3dseries.h"
#include "qscatter3dseries.h"
#include "qsurface3dseries.h"
#include "declarativeseries_p.h"

#include <QQmlExtensionPlugin>

QT_DATAVISUALIZATION_USE_NAMESPACE

QML_DECLARE_TYPE(AbstractDeclarative)
QML_DECLARE_TYPE(DeclarativeBars)
QML_DECLARE_TYPE(DeclarativeScatter)
QML_DECLARE_TYPE(DeclarativeSurface)

QML_DECLARE_TYPE(QItemModelBarDataMapping)
QML_DECLARE_TYPE(QItemModelScatterDataMapping)
QML_DECLARE_TYPE(QItemModelSurfaceDataMapping)

QML_DECLARE_TYPE(const QAbstractItemModel)
QML_DECLARE_TYPE(QDataVis)

QML_DECLARE_TYPE(Q3DAbstractAxis)
QML_DECLARE_TYPE(Q3DCategoryAxis)
QML_DECLARE_TYPE(Q3DValueAxis)

QML_DECLARE_TYPE(Q3DScene)
QML_DECLARE_TYPE(Q3DCamera)

QML_DECLARE_TYPE(QAbstractDataProxy)
QML_DECLARE_TYPE(QBarDataProxy)
QML_DECLARE_TYPE(QItemModelBarDataProxy)
QML_DECLARE_TYPE(QScatterDataProxy)
QML_DECLARE_TYPE(QItemModelScatterDataProxy)
QML_DECLARE_TYPE(QSurfaceDataProxy)
QML_DECLARE_TYPE(QItemModelSurfaceDataProxy)
QML_DECLARE_TYPE(QHeightMapSurfaceDataProxy)

QML_DECLARE_TYPE(QAbstract3DSeries)
QML_DECLARE_TYPE(QBar3DSeries)
QML_DECLARE_TYPE(QScatter3DSeries)
QML_DECLARE_TYPE(QSurface3DSeries)
QML_DECLARE_TYPE(DeclarativeBar3DSeries)
QML_DECLARE_TYPE(DeclarativeScatter3DSeries)
QML_DECLARE_TYPE(DeclarativeSurface3DSeries)

QML_DECLARE_TYPE(ColorGradientStop)
QML_DECLARE_TYPE(ColorGradient)

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


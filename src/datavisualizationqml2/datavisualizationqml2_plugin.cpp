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

#include "datavisualizationqml2_plugin.h"

#include <qqml.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

void Datavis3Dqml2Plugin::registerTypes(const char *uri)
{
    // @uri com.digia.QtDataVisualization
    qmlRegisterUncreatableType<const QAbstractItemModel>(uri, 1, 0, "AbstractItemModel",
                                                         QLatin1String("Trying to create uncreatable: AbstractItemModel."));
    qmlRegisterUncreatableType<QDataVis>(uri, 1, 0, "DataVis",
                                         QLatin1String("Trying to create uncreatable: DataVis."));
    qmlRegisterUncreatableType<Q3DAbstractAxis>(uri, 1, 0, "AbstractAxis3D",
                                                QLatin1String("Trying to create uncreatable: AbstractAxis."));
    qmlRegisterUncreatableType<QAbstractDataProxy>(uri, 1, 0, "AbstractDataProxy",
                                                   QLatin1String("Trying to create uncreatable: AbstractDataProxy."));
    qmlRegisterUncreatableType<QBarDataProxy>(uri, 1, 0, "BarDataProxy",
                                              QLatin1String("Trying to create uncreatable: BarDataProxy."));
    qmlRegisterUncreatableType<QScatterDataProxy>(uri, 1, 0, "ScatterDataProxy",
                                                  QLatin1String("Trying to create uncreatable: ScatterDataProxy."));
    qmlRegisterUncreatableType<QSurfaceDataProxy>(uri, 1, 0, "SurfaceDataProxy",
                                                  QLatin1String("Trying to create uncreatable: SurfaceDataProxy."));

    qmlRegisterType<QItemModelBarDataMapping>(uri, 1, 0, "BarDataMapping");
    qmlRegisterType<QItemModelScatterDataMapping>(uri, 1, 0, "ScatterDataMapping");
    qmlRegisterType<QItemModelSurfaceDataMapping>(uri, 1, 0, "SurfaceDataMapping");

    qmlRegisterType<DeclarativeBars>(uri, 1, 0, "Bars3D");
    qmlRegisterType<DeclarativeScatter>(uri, 1, 0, "Scatter3D");
    qmlRegisterType<DeclarativeSurface>(uri, 1, 0, "Surface3D");

    qmlRegisterType<Q3DValueAxis>(uri, 1, 0, "ValueAxis3D");
    qmlRegisterType<Q3DCategoryAxis>(uri, 1, 0, "CategoryAxis3D");

    qmlRegisterType<QItemModelBarDataProxy>(uri, 1, 0, "ItemModelBarDataProxy");
    qmlRegisterType<QItemModelScatterDataProxy>(uri, 1, 0, "ItemModelScatterDataProxy");
    qmlRegisterType<QItemModelSurfaceDataProxy>(uri, 1, 0, "ItemModelSurfaceDataProxy");
    qmlRegisterType<QHeightMapSurfaceDataProxy>(uri, 1, 0, "HeightMapSurfaceDataProxy");
}

QT_DATAVISUALIZATION_END_NAMESPACE


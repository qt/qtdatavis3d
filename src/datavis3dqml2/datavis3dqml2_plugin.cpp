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

#include "datavis3dqml2_plugin.h"

#include <qqml.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

void Datavis3Dqml2Plugin::registerTypes(const char *uri)
{
    // @uri com.digia.QtDataVis3D
    qmlRegisterUncreatableType<const QAbstractItemModel>(uri, 1, 0, "AbstractItemModel",
                                                   QLatin1String("Trying to create uncreatable: AbstractItemModel."));
    qmlRegisterUncreatableType<QDataVis>(uri, 1, 0, "DataVis",
                                         QLatin1String("Trying to create uncreatable: DataVis."));
    qmlRegisterUncreatableType<QAbstractAxis>(uri, 1, 0, "AbstractAxis",
                                              QLatin1String("Trying to create uncreatable: AbstractAxis."));

    qmlRegisterType<QItemModelBarDataMapping>(uri, 1, 0, "BarDataMapping");
    qmlRegisterType<QItemModelScatterDataMapping>(uri, 1, 0, "ScatterDataMapping");

    qmlRegisterType<DeclarativeBars>(uri, 1, 0, "Bars3D");
    qmlRegisterType<DeclarativeScatter>(uri, 1, 0, "Scatter3D");

    qmlRegisterType<QValueAxis>(uri, 1, 0, "ValueAxis");
    qmlRegisterType<QCategoryAxis>(uri, 1, 0, "CategoryAxis");
}

QT_DATAVIS3D_END_NAMESPACE


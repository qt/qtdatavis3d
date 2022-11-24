/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "declarativebars_p.h"
#include "declarativescatter_p.h"
#include "declarativesurface_p.h"
#include "qitemmodelbardataproxy.h"
#include "qitemmodelscatterdataproxy.h"
#include "qitemmodelsurfacedataproxy.h"
#include "qheightmapsurfacedataproxy.h"
#include "qvalue3daxis.h"
#include "qvalue3daxisformatter.h"
#include "qlogvalue3daxisformatter.h"
#include "qcategory3daxis.h"
#include "q3dobject.h"
#include "q3dcamera.h"
#include "q3dscene.h"
#include "q3dlight.h"
#include "qabstract3dseries.h"
#include "qbar3dseries.h"
#include "qscatter3dseries.h"
#include "qsurface3dseries.h"
#include "declarativeseries_p.h"
#include "q3dtheme.h"
#include "declarativetheme_p.h"
#include "q3dinputhandler.h"
#include "qtouch3dinputhandler.h"
#include "declarativecolor_p.h"
#include "declarativescene_p.h"
#include "qcustom3ditem.h"
#include "qcustom3dlabel.h"
#include "qcustom3dvolume.h"
#include "datavisualizationqml2_plugin.h"

#include <QtQml>


QML_DECLARE_TYPE(AbstractDeclarative)
QML_DECLARE_TYPE(DeclarativeBars)
QML_DECLARE_TYPE(DeclarativeScatter)
QML_DECLARE_TYPE(DeclarativeSurface)

QML_DECLARE_TYPE(QAbstractItemModel)

QML_DECLARE_TYPE(QAbstract3DAxis)
QML_DECLARE_TYPE(QCategory3DAxis)
QML_DECLARE_TYPE(QValue3DAxis)
QML_DECLARE_TYPE(QValue3DAxisFormatter)
QML_DECLARE_TYPE(QLogValue3DAxisFormatter)

QML_DECLARE_TYPE(Q3DScene)
QML_DECLARE_TYPE(Declarative3DScene)
QML_DECLARE_TYPE(Q3DObject)
QML_DECLARE_TYPE(Q3DCamera)
QML_DECLARE_TYPE(Q3DLight)

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

QML_DECLARE_TYPE(DeclarativeColor)

QML_DECLARE_TYPE(Q3DTheme)
QML_DECLARE_TYPE(DeclarativeTheme3D)

QML_DECLARE_TYPE(QAbstract3DInputHandler)
QML_DECLARE_TYPE(Q3DInputHandler)
QML_DECLARE_TYPE(QTouch3DInputHandler)

QML_DECLARE_TYPE(QCustom3DItem)
QML_DECLARE_TYPE(QCustom3DLabel)
QML_DECLARE_TYPE(QCustom3DVolume)

QT_BEGIN_NAMESPACE




void QtDataVisualizationQml2Plugin::registerTypes(const char *uri)
{
    // @uri QtDataVisualization

    // QtDataVisualization 1.0

    qmlRegisterUncreatableType<QAbstractItemModel>(uri, 1, 0, "AbstractItemModel",
                                                   QLatin1String("Trying to create uncreatable: AbstractItemModel."));
    qmlRegisterUncreatableType<QAbstract3DAxis>(uri, 1, 0, "AbstractAxis3D",
                                                QLatin1String("Trying to create uncreatable: AbstractAxis."));
    qmlRegisterUncreatableType<QAbstractDataProxy>(uri, 1, 0, "AbstractDataProxy",
                                                   QLatin1String("Trying to create uncreatable: AbstractDataProxy."));
    qmlRegisterUncreatableType<QBarDataProxy>(uri, 1, 0, "BarDataProxy",
                                              QLatin1String("Trying to create uncreatable: BarDataProxy."));
    qmlRegisterUncreatableType<QScatterDataProxy>(uri, 1, 0, "ScatterDataProxy",
                                                  QLatin1String("Trying to create uncreatable: ScatterDataProxy."));
    qmlRegisterUncreatableType<QSurfaceDataProxy>(uri, 1, 0, "SurfaceDataProxy",
                                                  QLatin1String("Trying to create uncreatable: SurfaceDataProxy."));
    qmlRegisterUncreatableType<AbstractDeclarative>(uri, 1, 0, "AbstractGraph3D",
                                                    QLatin1String("Trying to create uncreatable: AbstractGraph3D."));
    qmlRegisterUncreatableType<Declarative3DScene>(uri, 1, 0, "Scene3D",
                                                   QLatin1String("Trying to create uncreatable: Scene3D."));
    qmlRegisterUncreatableType<QAbstract3DSeries>(uri, 1, 0, "Abstract3DSeries",
                                                  QLatin1String("Trying to create uncreatable: Abstract3DSeries."));
    qmlRegisterUncreatableType<QBar3DSeries>(uri, 1, 0, "QBar3DSeries",
                                             QLatin1String("Trying to create uncreatable: QBar3DSeries, use Bar3DSeries instead."));
    qmlRegisterUncreatableType<QScatter3DSeries>(uri, 1, 0, "QScatter3DSeries",
                                                 QLatin1String("Trying to create uncreatable: QScatter3DSeries, use Scatter3DSeries instead."));
    qmlRegisterUncreatableType<QSurface3DSeries>(uri, 1, 0, "QSurface3DSeries",
                                                 QLatin1String("Trying to create uncreatable: QSurface3DSeries, use Surface3DSeries instead."));
    qmlRegisterUncreatableType<Q3DTheme>(uri, 1, 0, "Q3DTheme",
                                         QLatin1String("Trying to create uncreatable: Q3DTheme, use Theme3D instead."));
    qmlRegisterUncreatableType<QAbstract3DInputHandler>(uri, 1, 0, "AbstractInputHandler3D",
                                                        QLatin1String("Trying to create uncreatable: AbstractInputHandler3D."));
    qmlRegisterUncreatableType<Q3DObject>(uri, 1, 0, "Object3D",
                                          QLatin1String("Trying to create uncreatable: Object3D."));

    qmlRegisterType<DeclarativeBars>(uri, 1, 0, "Bars3D");
    qmlRegisterType<DeclarativeScatter>(uri, 1, 0, "Scatter3D");
    qmlRegisterType<DeclarativeSurface>(uri, 1, 0, "Surface3D");

    qmlRegisterType<QValue3DAxis>(uri, 1, 0, "ValueAxis3D");
    qmlRegisterType<QCategory3DAxis>(uri, 1, 0, "CategoryAxis3D");

    qmlRegisterType<Q3DCamera>(uri, 1, 0, "Camera3D");
    qmlRegisterType<Q3DLight>(uri, 1, 0, "Light3D");

    qmlRegisterType<QItemModelBarDataProxy>(uri, 1, 0, "ItemModelBarDataProxy");
    qmlRegisterType<QItemModelScatterDataProxy>(uri, 1, 0, "ItemModelScatterDataProxy");
    qmlRegisterType<QItemModelSurfaceDataProxy>(uri, 1, 0, "ItemModelSurfaceDataProxy");
    qmlRegisterType<QHeightMapSurfaceDataProxy>(uri, 1, 0, "HeightMapSurfaceDataProxy");

    qmlRegisterType<ColorGradientStop>(uri, 1, 0, "ColorGradientStop");
    qmlRegisterType<ColorGradient>(uri, 1, 0, "ColorGradient");

    qmlRegisterType<DeclarativeColor>(uri, 1, 0, "ThemeColor");
    qmlRegisterType<DeclarativeTheme3D>(uri, 1, 0, "Theme3D");

    qmlRegisterType<DeclarativeBar3DSeries>(uri, 1, 0, "Bar3DSeries");
    qmlRegisterType<DeclarativeScatter3DSeries>(uri, 1, 0, "Scatter3DSeries");
    qmlRegisterType<DeclarativeSurface3DSeries>(uri, 1, 0, "Surface3DSeries");

    qRegisterMetaType<QAbstract3DGraph::ShadowQuality>("QAbstract3DGraph::ShadowQuality");

    // QtDataVisualization 1.1

    // New revisions
    qmlRegisterUncreatableType<QAbstract3DAxis, 1>(uri, 1, 1, "AbstractAxis3D",
                                                   QLatin1String("Trying to create uncreatable: AbstractAxis."));
    qmlRegisterUncreatableType<QAbstract3DSeries, 1>(uri, 1, 1, "Abstract3DSeries",
                                                     QLatin1String("Trying to create uncreatable: Abstract3DSeries."));
    qmlRegisterUncreatableType<AbstractDeclarative, 1>(uri, 1, 1, "AbstractGraph3D",
                                                       QLatin1String("Trying to create uncreatable: AbstractGraph3D."));

    qmlRegisterType<QValue3DAxis, 1>(uri, 1, 1, "ValueAxis3D");
    qmlRegisterType<QItemModelBarDataProxy, 1>(uri, 1, 1, "ItemModelBarDataProxy");
    qmlRegisterType<QItemModelSurfaceDataProxy, 1>(uri, 1, 1, "ItemModelSurfaceDataProxy");
    qmlRegisterType<QItemModelScatterDataProxy, 1>(uri, 1, 1, "ItemModelScatterDataProxy");

    // New types
    qmlRegisterType<QValue3DAxisFormatter>(uri, 1, 1, "ValueAxis3DFormatter");
    qmlRegisterType<QLogValue3DAxisFormatter>(uri, 1, 1, "LogValueAxis3DFormatter");
    qmlRegisterType<QCustom3DItem>(uri, 1, 1, "Custom3DItem");
    qmlRegisterType<QCustom3DLabel>(uri, 1, 1, "Custom3DLabel");

    // New metatypes
    qRegisterMetaType<QAbstract3DGraph::ElementType>("QAbstract3DGraph::ElementType");

    // QtDataVisualization 1.2

    // New revisions
    qmlRegisterUncreatableType<AbstractDeclarative, 2>(uri, 1, 2, "AbstractGraph3D",
                                                       QLatin1String("Trying to create uncreatable: AbstractGraph3D."));
    qmlRegisterRevision<Q3DScene, 1>(uri, 1, 2);
    qmlRegisterUncreatableType<Declarative3DScene, 1>(uri, 1, 2, "Scene3D",
                                                      QLatin1String("Trying to create uncreatable: Scene3D."));
    qmlRegisterType<DeclarativeSurface, 1>(uri, 1, 2, "Surface3D");
    qmlRegisterType<Q3DCamera, 1>(uri, 1, 2, "Camera3D");
    qmlRegisterType<QCustom3DItem, 1>(uri, 1, 2, "Custom3DItem");
    qmlRegisterType<DeclarativeBars, 1>(uri, 1, 2, "Bars3D");

    // New types
    qmlRegisterType<Q3DInputHandler>(uri, 1, 2, "InputHandler3D");
    qmlRegisterType<QTouch3DInputHandler>(uri, 1, 2, "TouchInputHandler3D");
    qmlRegisterType<QCustom3DVolume>(uri, 1, 2, "Custom3DVolume");

    // QtDataVisualization 1.3

    // New revisions
    qmlRegisterType<Q3DLight, 1>(uri, 1, 3, "Light3D");

    // The minor version used to be the current Qt 5 minor. For compatibility it is the last
    // Qt 5 release.
    qmlRegisterModule(uri, 1, 15);
}

QT_END_NAMESPACE


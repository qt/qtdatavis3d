/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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
#include "qabstract3dinputhandler.h"
#include "declarativecolor_p.h"
#include "declarativescene_p.h"
#include "qcustom3ditem.h"

#include <QtQml/QQmlExtensionPlugin>

using namespace QtDataVisualization;

QML_DECLARE_TYPE(AbstractDeclarative)
QML_DECLARE_TYPE(DeclarativeBars)
QML_DECLARE_TYPE(DeclarativeScatter)
QML_DECLARE_TYPE(DeclarativeSurface)

QML_DECLARE_TYPE(const QAbstractItemModel)

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

QML_DECLARE_TYPE(QCustom3DItem)

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QtDataVisualizationQml2Plugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif // DATAVISUALIZATIONQML2_PLUGIN_H


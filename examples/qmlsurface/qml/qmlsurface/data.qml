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

import QtQuick 2.1
import com.digia.QtDataVis3D 1.0

Item {
    property alias mapping: surfaceMapping
    property alias model: dataModel
    property alias proxy: modelProxy

    SurfaceDataMapping {
        id: surfaceMapping
        rowRole: "year"
        columnRole: "month"
        valueRole: "expenses"
        rowCategories: ["2000", "2001", "2002", "2003"]
        columnCategories: ["Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    }

    ItemModelSurfaceDataProxy {
        id: modelProxy
        activeMapping: surfaceMapping
        itemModel: dataModel
    }

    ListModel {
        id: dataModel
        ListElement{ year: "2000"; month: "Jan"; expenses: "3"; }
        ListElement{ year: "2000"; month: "Feb"; expenses: "8"; }
        ListElement{ year: "2000"; month: "Mar"; expenses: "10"; }
        ListElement{ year: "2000"; month: "Apr"; expenses: "12"; }
        ListElement{ year: "2000"; month: "May"; expenses: "10"; }
        ListElement{ year: "2000"; month: "Jun"; expenses: "5"; }
        ListElement{ year: "2000"; month: "Jul"; expenses: "1"; }
        ListElement{ year: "2000"; month: "Aug"; expenses: "7"; }
        ListElement{ year: "2000"; month: "Sep"; expenses: "4"; }
        ListElement{ year: "2000"; month: "Oct"; expenses: "22"; }
        ListElement{ year: "2000"; month: "Nov"; expenses: "16"; }
        ListElement{ year: "2000"; month: "Dec"; expenses: "2"; }
     }
}

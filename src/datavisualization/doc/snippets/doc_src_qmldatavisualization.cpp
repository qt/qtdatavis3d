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

//! [0]
import com.digia.QtDataVisualization 1.0
//! [0]

//! [1]
Bars3D {
    rows: 4
    columns: 4
    dataProxy: barProxy // an ItemModelBarDataProxy
    barSpacing: Qt.size(0.5, 0.5)
    barSpacingRelative: false
    itemLabelFormat: "@valueTitle for @colLabel, @rowLabel: @valueLabel"
}
//! [1]

//! [2]
Scatter3D {
    dataProxy: scatterProxy // an ItemModelScatterDataProxy
    itemLabelFormat: "X:@xLabel Y:@yLabel Z:@zLabel"
    axisX.segmentCount: 2
    axisX.subSegmentCount: 2
    axisX.labelFormat: "%.2f"
    axisZ.segmentCount: 2
    axisZ.subSegmentCount: 2
    axisZ.labelFormat: "%.2f"
    axisY.segmentCount: 3
    axisY.subSegmentCount: 2
    axisY.labelFormat: "%.2f"
}
//! [2]

//! [3]
Surface3D {
    dataProxy: surfaceProxy // an ItemModelSurfaceDataProxy
    axisX.min: 0.0
    axisX.max: 10.0
    axisZ.min: 0.0
    axisZ.max: 10.0
    axisY.min: 0.0
    axisY.max: 5.0
    axisX.segmentCount: 5
    axisX.subSegmentCount: 2
    axisX.labelFormat: "%i"
    axisZ.segmentCount: 5
    axisZ.subSegmentCount: 2
    axisZ.labelFormat: "%i"
    axisY.segmentCount: 5
    axisY.labelFormat: "%.1f"
}
//! [3]

//! [4]
BarDataMapping {
    id: barMapping
    rowRole: "year"
    columnRole: "city"
    valueRole: "expenses"
    rowCategories: ["2010", "2011", "2012", "2013"]
    columnCategories: ["Oulu", "Rauma", "Helsinki", "Tampere"]
}
//! [4]

//! [5]
ScatterDataMapping {
    id: scatterMapping
    xPosRole: "xPos"
    yPosRole: "yPos"
    zPosRole: "zPos"
}
//! [5]

//! [6]
SurfaceDataMapping {
    id: surfaceMapping
    rowRole: "latitude"
    columnRole: "longitude"
    valueRole: "population"
    rowCategories: ["64.5", "64.6", "64.7", "64.8", "64.9", "65.0", "65.1", "65.2", "65.3", "65.4"]
    columnCategories: ["24.5", "24.75", "25.0", "25.25", "25.5", "25.75", "26.0", "26.25", "26.5", "26.75"]
}
//! [6]

//! [7]
ItemModelBarDataProxy {
    id: barProxy
    activeMapping: barMapping       // a BarDataMapping
    itemModel: dataModel            // a ListModel
}
//! [7]

//! [8]
ItemModelScatterDataProxy {
    id: scatterProxy
    activeMapping: scatterMapping   // a ScatterDataMapping
    itemModel: dataModel            // a ListModel
}
//! [8]

//! [9]
ItemModelSurfaceDataProxy {
    id: surfaceProxy
    activeMapping: surfaceMapping   // a SurfaceDataMapping
    itemModel: dataModel            // a ListModel
}
//! [9]

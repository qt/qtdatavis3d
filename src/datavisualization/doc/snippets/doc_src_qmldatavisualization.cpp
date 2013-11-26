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
import QtDataVisualization 1.0
//! [0]

//! [1]
Bars3D {
    rows: 4
    columns: 4
    barSpacing: Qt.size(0.5, 0.5)
    barSpacingRelative: false

    Bar3DSeries {
        itemLabelFormat: "@valueTitle for @colLabel, @rowLabel: @valueLabel"

        ItemModelBarDataProxy {
            itemModel: model // E.g. a list model defined elsewhere containing monthly expenses data.
            // Mapping model roles to bar series rows, columns, and values.
            rowRole: "year"
            columnRole: "city"
            valueRole: "expenses"
            rowCategories: ["2010", "2011", "2012", "2013"]
            columnCategories: ["Oulu", "Rauma", "Helsinki", "Tampere"]
        }
    }
}
//! [1]

//! [2]
Scatter3D {
    axisX.segmentCount: 2
    axisX.subSegmentCount: 2
    axisX.labelFormat: "%.2f"
    axisZ.segmentCount: 2
    axisZ.subSegmentCount: 2
    axisZ.labelFormat: "%.2f"
    axisY.segmentCount: 3
    axisY.subSegmentCount: 2
    axisY.labelFormat: "%.2f"

    Scatter3DSeries {
        itemLabelFormat: "X:@xLabel Y:@yLabel Z:@zLabel"

        ItemModelScatterDataProxy {
            itemModel: model // E.g. a list model defined elsewhere containing point coordinates.
            // Mapping model roles to scatter series item coordinates.
            xPosRole: "xPos"
            yPosRole: "yPos"
            zPosRole: "zPos"
        }
    }
}
//! [2]

//! [3]
Surface3D {
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

    Surface3DSeries {
        ItemModelSurfaceDataProxy {
            itemModel: model // E.g. a list model defined elsewhere containing population data.
            // Mapping model roles to surface series rows, columns, and values.
            rowRole: "longitude"
            columnRole: "latitude"
            valueRole: "pop_density"
        }
    }
}
//! [3]

//! [7]
ItemModelBarDataProxy {
    itemModel: model // E.g. a list model defined elsewhere containing monthly expenses data.
    // Mapping model roles to bar series rows, columns, and values.
    rowRole: "year"
    columnRole: "city"
    valueRole: "expenses"
    rowCategories: ["2010", "2011", "2012", "2013"]
    columnCategories: ["Oulu", "Rauma", "Helsinki", "Tampere"]
}
//! [7]

//! [8]
ItemModelScatterDataProxy {
    itemModel: model // E.g. a list model defined elsewhere containing point coordinates.
    // Mapping model roles to scatter series item coordinates.
    xPosRole: "xPos"
    yPosRole: "yPos"
    zPosRole: "zPos"
}
//! [8]

//! [9]
ItemModelSurfaceDataProxy {
    itemModel: model // E.g. a list model defined elsewhere containing population data.
    // Mapping model roles to surface series rows, columns, and values.
    rowRole: "longitude"
    columnRole: "latitude"
    valueRole: "pop_density"
}
//! [9]

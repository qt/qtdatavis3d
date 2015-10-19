/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

import QtQuick 2.0
import QtDataVisualization 1.2

Bars3D {
    width: 300
    height: 300
    Bar3DSeries {
        ItemModelBarDataProxy {
            itemModel: ListModel {
                ListElement{ row: "row 1"; column: "column 1"; value: "1"; }
                ListElement{ row: "row 1"; column: "column 2"; value: "2"; }
                ListElement{ row: "row 1"; column: "column 3"; value: "3"; }
            }

            rowRole: "row"
            columnRole: "column"
            valueRole: "value"
        }
    }
}

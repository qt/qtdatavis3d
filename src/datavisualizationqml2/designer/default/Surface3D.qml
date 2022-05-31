/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
******************************************************************************/

import QtQuick 2.0
import QtDataVisualization 1.2

Surface3D {
    width: 300
    height: 300
    Surface3DSeries {
        ItemModelSurfaceDataProxy {
            itemModel: ListModel {
                ListElement{ row: "1"; column: "1"; y: "1"; }
                ListElement{ row: "1"; column: "2"; y: "2"; }
                ListElement{ row: "2"; column: "1"; y: "3"; }
                ListElement{ row: "2"; column: "2"; y: "4"; }
            }

            rowRole: "row"
            columnRole: "column"
            yPosRole: "y"
        }
    }
}

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

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

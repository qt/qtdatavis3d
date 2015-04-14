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

Scatter3D {
    width: 300
    height: 300
    Scatter3DSeries {
        ItemModelScatterDataProxy {
            itemModel: ListModel {
                ListElement{ x: "1"; y: "2"; z: "3"; }
                ListElement{ x: "2"; y: "3"; z: "4"; }
                ListElement{ x: "3"; y: "4"; z: "1"; }
            }

            xPosRole: "x"
            yPosRole: "y"
            zPosRole: "z"
        }
    }
}

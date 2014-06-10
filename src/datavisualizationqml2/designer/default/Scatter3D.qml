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

import QtQuick 2.0
import QtDataVisualization 1.1

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

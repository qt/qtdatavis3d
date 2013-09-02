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
    property alias mapping: scatterMapping
    property alias model: dataModel

    ScatterDataMapping {
        id: scatterMapping
        xPosRole: "xPos"
        yPosRole: "yPos"
        zPosRole: "zPos"
    }

    ListModel {
        id: dataModel
        ListElement{ xPos: -10.0; yPos: 4.9; zPos: -5.0 }
        ListElement{ xPos: 10.0; yPos: 4.9; zPos: -5.0 }
        ListElement{ xPos: -10.0; yPos: 4.9; zPos: 5.0 }
        ListElement{ xPos: 10.0; yPos: 4.9; zPos: 5.0 }
        ListElement{ xPos: -10.0; yPos: -4.9; zPos: -5.0 }
        ListElement{ xPos: 10.0; yPos: -4.9; zPos: -5.0 }
        ListElement{ xPos: -10.0; yPos: -4.9; zPos: 5.0 }
        ListElement{ xPos: 10.0; yPos: -4.9; zPos: 5.0 }

        ListElement{ xPos: -1.0; yPos: 0.3; zPos: -0.5 }
        ListElement{ xPos: 1.0; yPos: 2.105; zPos: 0.5 }
        ListElement{ xPos: 0.5; yPos: -0.65; zPos: -0.5 }
        ListElement{ xPos: -0.5; yPos: 1.225; zPos: 0.5 }
        ListElement{ xPos: 0.0; yPos: 0.0; zPos: 0.0 }
        ListElement{ xPos: 0.0; yPos: 2.0; zPos: 0.0 }
        ListElement{ xPos: 0.0; yPos: -0.5; zPos: 0.0 }

        ListElement{ xPos: 6.0; yPos: 0.0; zPos: 4.0 }
        ListElement{ xPos: 5.8; yPos: 0.2; zPos: 5.0 }
        ListElement{ xPos: 5.6; yPos: 0.4; zPos: 4.5 }
        ListElement{ xPos: 5.4; yPos: 0.6; zPos: 3.8 }
        ListElement{ xPos: 5.2; yPos: 0.8; zPos: 4.8 }
        ListElement{ xPos: 5.0; yPos: 0.3; zPos: 4.1 }
        ListElement{ xPos: 4.9; yPos: -0.3; zPos: 4.9 }
        ListElement{ xPos: 4.7; yPos: -0.5; zPos: 3.5 }
        ListElement{ xPos: 4.5; yPos: -0.7; zPos: 3.3 }
        ListElement{ xPos: 4.3; yPos: -0.4; zPos: 3.7 }
    }
}

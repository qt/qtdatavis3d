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

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

//function createData(base) {
//    for (var z = 0; z < 30; z++) {
//        for (var x = 0; x < 30; x++) {
//            var angle = (((z - 16) * (x - 16)) / 144.0) * 1.57;
//            var y = Math.sin(angle + base);
//            dataModel.append({"z": z, "x": x, "y": y});
//        }
//    }
//}

function createData(base) {
    for (var i = 0; i < base; i++) {
        dataModel.append({"z": Math.random(), "x": Math.random(), "y": Math.random()});
    }
}

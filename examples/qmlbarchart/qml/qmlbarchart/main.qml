/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE$
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import com.digia.QtDataVis3D 1.0

DataVisView {
    width: 360
    height: 360

    DataItem {
        id: testitem
        label: "Test"
        value: 10
    }
    DataRow {
        id: testrow
        function addData() {
            testrow.addItem(testitem);
        }
    }
    Bars {
        id: testchart
        visible: true
        //width: parent.width
        //height: parent.height

        grid: false
        shadowQuality: DataVisView.ShadowNone
        selectionMode: DataVisView.ModeNone
        labelTransparency: DataVisView.TransparencyNone
        windowTitle: "QmlTest3DBars"
        function setUpBars() {
            //console.log(parent)
            //console.log(parent.width)
            //console.log(parent.height)
            testchart.setupSampleSpace(1, 1);
            testchart.addDataRow(testrow);
        }
    }

    Text {
        text: testitem.label
        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            //console.log("onClicked")
            testrow.addData();
            testchart.setUpBars();
        }
    }

    onHeightChanged: {
        //console.log("onHeightChanged")
    }
}

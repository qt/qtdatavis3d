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
import QtQuick.Window 2.1
import com.digia.QtDataVis3D 1.0

Item {
    id: container
    anchors.fill: parent

    DataVisView {
        id: mainview
        width: 360
        height: 360

        DataItem {
            id: testitem
            label: "Test"
            value: 10
        }
        DataItem {
            id: testitem2
            label: "Test2"
            value: -10
        }
        DataRow {
            id: testrow
            function addData() {
                testrow.addItem(testitem);
                testrow.addItem(testitem2);
            }
        }
        Bars {
            id: testchart
            visible: true
            width: mainview.width
            height: mainview.height
            //x: mainview.x + mainview.width
            //y: mainview.y

            grid: false
            shadowQuality: Bars.ShadowNone
            selectionMode: Bars.ModeNone
            labelTransparency: Bars.TransparencyNone
            windowTitle: "QmlTest3DBars"

            function setUpBars() {
                /*console.log(parent)
                console.log(container.x)
                console.log(container.y)
                console.log(Window.x)
                console.log(Window.y)
                console.log(Screen.desktopAvailableHeight)
                console.log(Screen.desktopAvailableWidth)
                console.log(mainview.x)
                console.log(mainview.y)
                console.log(x)
                console.log(y)*/
                testchart.setupSampleSpace(2, 1);
                testchart.addDataRow(testrow);
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                testchart.destroy();
                testchart.close();
                Qt.quit();
            }
        }

        Component.onCompleted: {
            testrow.addData();
            testchart.setUpBars();
        }
    }
}

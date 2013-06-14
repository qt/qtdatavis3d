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
    id: mainview
    //title: "My MainWindow"
    width: 640
    height: 480
    visible: true

    DataItem {
        id: testitem1
        label: "Test"
        value: 10
    }
    DataItem {
        id: testitem2
        label: "Test2"
        value: -10
    }
    DataItem {
        id: testitem3
        label: "Test3"
        value: 5
    }

    DataItem {
        id: testitem4
        label: "Test4"
        value: -7
    }
    DataItem {
        id: testitem5
        label: "Test5"
        value: 8
    }
    DataItem {
        id: testitem6
        label: "Test6"
        value: 1
    }

    DataRow {
        id: testrow1
        function addData() {
            testrow1.addItem(testitem1);
            testrow1.addItem(testitem2);
            testrow1.addItem(testitem3);
            testrow1.addItem(testitem4);
            testrow1.addItem(testitem5);
            testrow1.addItem(testitem6);
        }
    }
/*
    ListModel {
        id: testdata1
        ListElement{ column1: testitem1 ; column2: testitem2 ; column3: testitem3 }
        ListElement{ column1: testitem4 ; column2: testitem6 ; column3: testitem6 }
    }
*/
    Bars3D {
        id: testchart
        width: mainview.width
        height: mainview.height

        //data: testdata1

        gridVisible: true
        shadowQuality: Bars3D.ShadowNone
        selectionMode: Bars3D.ModeNone
        labelTransparency: Bars3D.TransparencyNone

        function setUpBars3D() {
            testchart.setupSampleSpace(6, 1);
            testchart.addDataRow(testrow1);
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    Component.onCompleted: {
        // This allows us to flip the texture to be displayed correctly in scene graph
        // TODO: Find a way to do it in code..
        //rotation.angle = 180
        testrow1.addData();
        testchart.setUpBars3D();
    }
}

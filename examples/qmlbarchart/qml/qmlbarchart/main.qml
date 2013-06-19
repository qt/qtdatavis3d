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

import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import com.digia.QtDataVis3D 1.0

Item {
    id: mainview
    //title: "My MainWindow"
    width: 640
    height: 480
    visible: true

    Item {
        id: dataView
        width: parent.width - shadowToggle.width
        height: parent.height
        anchors.right: parent.right;
        DataItem {
            id: testitem1
            label: "Test"
            value: -8
        }
        DataItem {
            id: testitem2
            label: "Test2"
            value: -2
        }
        DataItem {
            id: testitem3
            label: "Test3"
            value: 0
        }
        DataItem {
            id: testitem4
            label: "Test4"
            value: 1
        }
        DataItem {
            id: testitem5
            label: "Test5"
            value: 3
        }
        DataItem {
            id: testitem6
            label: "Test6"
            value: 5
        }
        DataItem {
            id: testitem7
            label: "Test7"
            value: 7
        }
        DataItem {
            id: testitem8
            label: "Test8"
            value: 10
        }

        DataRow {
            id: testrow1
        }
        DataRow {
            id: testrow2
        }

        DataSet {
            id: testset1
        }

    //    ListModel {
    //        id: testdata1
    //        ListElement{ column1: "test" ; column2: "test" ; column3: "test" }
    //        ListElement{ column1: "test" ; column2: "test" ; column3: "test" }
    //        ListElement{ column1: testitem1 ; column2: testitem2 ; column3: testitem3 }
    //        ListElement{ column1: testitem4 ; column2: testitem6 ; column3: testitem6 }
    //    }

        Bars3D {
            id: testchart
            width: dataView.width
            height: dataView.height

            Component.onCompleted: {
                //data = testdata1
            }

            gridVisible: true
            shadowQuality: Bars3D.ShadowNone
            selectionMode: Bars3D.ModeNone
            labelTransparency: Bars3D.TransparencyNone
        }
    }


    Component.onCompleted: {
        testrow1.addItem(testitem1);
        testrow1.addItem(testitem2);
        testrow1.addItem(testitem3);
        testrow1.addItem(testitem4);
        testrow2.addItem(testitem5);
        testrow2.addItem(testitem6);
        testrow2.addItem(testitem7);
        testrow2.addItem(testitem8);
        testset1.addRow(testrow1);
        testset1.addRow(testrow2);

        testchart.setupSampleSpace(4, 2);
        testchart.addDataSet(testset1);
    }

    Rectangle {
        id: shadowToggle
        color: "#FFFFFF"
        x: 0
        y: 0
        width: 160
        height: 80

        TextArea {
            id: buttonText
            text: "Toggle Shadows"
            anchors.fill: parent
            textColor: "#000000"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (testchart.shadowQuality == Bars3D.ShadowNone) {
                    testchart.shadowQuality = Bars3D.ShadowLow;
                    buttonText.textColor = "#FFFFFF";
                    shadowToggle.color = "#000000";
                } else {
                    testchart.shadowQuality = Bars3D.ShadowNone;
                    buttonText.textColor = "#000000";
                    shadowToggle.color = "#FFFFFF";
                }
            }
        }
    }
}

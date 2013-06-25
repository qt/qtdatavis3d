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
import QtQuick.Controls 1.0
import com.digia.QtDataVis3D 1.0

Item {
    id: mainview
    width: 800
    height: 500
    visible: true
    //title: "Noise levels from construction site"

    Item {
        id: dataView
        width: parent.width
        height: parent.height - shadowToggle.height
        anchors.bottom: parent.bottom

        Image {
            id: testimage
            source: "qrc:/images/floorplan.jpg"
            visible: false
        }

        DataItem {
            id: testitem1
            label: "dB"
            value: 76
            position: "95.0, 490.0"
        }
        DataItem {
            id: testitem2
            label: "dB"
            value: 88
            position: "185.0, 105.0"
        }
        DataItem {
            id: testitem3
            label: "dB"
            value: 85
            position: "700.0, 465.0"
        }
        DataItem {
            id: testitem4
            label: "dB"
            value: 92
            position: "505.0, 225.0"
        }

        Maps3D {
            id: testmap
            width: dataView.width
            height: dataView.height
            fontSize: 300.0

            Component.onCompleted: {
                console.log("testmap complete");
                console.log(testimage);
                console.log(testimage.sourceSize);
                setBarSpecs(Qt.vector3d(10.0, 10.0, 10.0));
                setAreaSpecs(Qt.rect(0, 0, testimage.sourceSize.width, testimage.sourceSize.height),
                             testimage);
                //setImage(testimage);
                setImage(":/images/floorplan.jpg");
                shadowQuality = Maps3D.ShadowNone
                selectionMode = Maps3D.ModeBar
                labelTransparency = Maps3D.TransparencyNoBackground//.TransparencyFromTheme
                addDataItem(testitem1);
                addDataItem(testitem2);
                addDataItem(testitem3);
                addDataItem(testitem4);
            }
        }
    }

    Component.onCompleted: {
        console.log("mainview complete");
    }

    Rectangle {
        id: shadowToggle
        color: "#FFFFFF"
        x: 0
        y: 0
        width: parent.width
        height: 60

        TextArea {
            id: buttonText
            text: "Toggle Shadows"
            anchors.fill: parent
            textColor: "#000000"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (testmap.shadowQuality === Maps3D.ShadowNone) {
                    testmap.shadowQuality = Maps3D.ShadowLow;
                    buttonText.textColor = "#999999";
                } else {
                    testmap.shadowQuality = Maps3D.ShadowNone;
                    buttonText.textColor = "#000000";
                }
            }
        }
    }
}

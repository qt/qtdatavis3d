// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtDataVisualization
import "."

Item {
    id: mainView
    width: 1280
    height: 1024

    property int sampleColumns: sampleSlider.value
    property int sampleRows: sampleColumns / 2
    property int sampleCache: 24

    property bool portraitMode: width < height

    onSampleRowsChanged: {
        surfaceSeries.selectedPoint = surfaceSeries.invalidSelectionPosition
        generateData()
    }

    Item {
        id: dataView
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height - controlArea.height

        Surface3D {
            id: surfaceGraph

            width: dataView.width
            height: dataView.height
            shadowQuality: AbstractGraph3D.ShadowQualityNone
            selectionMode: AbstractGraph3D.SelectionSlice | AbstractGraph3D.SelectionItemAndRow

            axisX.labelFormat: "%d ms"
            axisY.labelFormat: "%d W"
            axisZ.labelFormat: "%d mV"
            axisX.min: 0
            axisY.min: 0
            axisZ.min: 0
            axisX.max: 1000
            axisY.max: 100
            axisZ.max: 800
            axisX.segmentCount: 4
            axisY.segmentCount: 4
            axisZ.segmentCount: 4
            measureFps: true
            renderingMode: AbstractGraph3D.RenderDirectToBackground

            onCurrentFpsChanged: (fps)=> {
                if (fps > 10)
                    fpsText.text = "FPS: " + Math.round(surfaceGraph.currentFps)
                else
                    fpsText.text = "FPS: " + Math.round(surfaceGraph.currentFps * 10.0) / 10.0
            }

            //! [0]
            Surface3DSeries {
                id: surfaceSeries
                drawMode: Surface3DSeries.DrawSurface;
                flatShadingEnabled: false;
                meshSmooth: true
                itemLabelFormat: "@xLabel, @zLabel: @yLabel"
                itemLabelVisible: false

                onItemLabelChanged: {
                    if (surfaceSeries.selectedPoint === surfaceSeries.invalidSelectionPosition)
                        selectionText.text = "No selection"
                    else
                        selectionText.text = surfaceSeries.itemLabel
                }
            }
            //! [0]

            //! [2]
            Component.onCompleted: mainView.generateData()
            //! [2]
        }
    }

    //! [3]
    Timer {
        id: refreshTimer
        interval: 1000 / frequencySlider.value
        running: true
        repeat: true
        onTriggered: dataSource.update(surfaceSeries)
    }
    //! [3]

    Rectangle {
        id: controlArea
        height: portraitMode ? flatShadingToggle.implicitHeight * 7
                             : flatShadingToggle.implicitHeight * 2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        color: surfaceGraph.theme.backgroundColor

        // Samples
        Rectangle {
            id: samples
            width: portraitMode ? mainView.width : mainView.width / 4
            height: flatShadingToggle.implicitHeight
            anchors.left: parent.left
            anchors.top: parent.top

            border.color: "gray"
            border.width: 1
            radius: 4

            Row {
                anchors.centerIn: parent
                spacing: 10
                padding: 5

                Slider {
                    id: sampleSlider
                    from: mainView.sampleCache * 2
                    to: from * 10
                    stepSize: mainView.sampleCache
                    Component.onCompleted: value = from * 2
                }

                Text {
                    id: samplesText
                    text: "Samples: " + (mainView.sampleRows * mainView.sampleColumns)
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        // Frequency
        Rectangle {
            id: frequency
            width: portraitMode ? mainView.width : mainView.width / 4
            height: flatShadingToggle.implicitHeight
            anchors.left: portraitMode ? parent.left : samples.right
            anchors.top: portraitMode ? samples.bottom : parent.top

            border.color: "gray"
            border.width: 1
            radius: 4

            Row {
                anchors.centerIn: parent
                spacing: 10
                padding: 5

                Slider {
                    id: frequencySlider
                    from: 2
                    to: 60
                    stepSize: 2
                    value: 30
                }

                Text {
                    id: frequencyText
                    text: "Freq: " + frequencySlider.value + " Hz"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        // FPS
        Rectangle {
            id: fpsindicator
            width: portraitMode ? mainView.width : mainView.width / 4
            height: flatShadingToggle.implicitHeight
            anchors.left: portraitMode ? parent.left : frequency.right
            anchors.top: portraitMode ? frequency.bottom : parent.top

            border.color: "gray"
            border.width: 1
            radius: 4

            Text {
                id: fpsText
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // Selection
        Rectangle {
            id: selection
            width: portraitMode ? mainView.width : mainView.width / 4
            height: flatShadingToggle.implicitHeight
            anchors.left: portraitMode ? parent.left : fpsindicator.right
            anchors.top: portraitMode ? fpsindicator.bottom : parent.top

            border.color: "gray"
            border.width: 1
            radius: 4

            Text {
                id: selectionText
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "No selection"
            }
        }

        // Flat shading
        Button {
            id: flatShadingToggle
            width: portraitMode ? mainView.width : mainView.width / 3
            anchors.left: parent.left
            anchors.top: selection.bottom

            text: surfaceSeries.flatShadingSupported ? "Show Flat" : "Flat not supported"
            enabled: surfaceSeries.flatShadingSupported

            onClicked: {
                if (surfaceSeries.flatShadingEnabled === true) {
                    surfaceSeries.flatShadingEnabled = false;
                    text = "Show Flat"
                } else {
                    surfaceSeries.flatShadingEnabled = true;
                    text = "Show Smooth"
                }
            }
        }

        // Surface grid
        Button {
            id: surfaceGridToggle
            width: portraitMode ? mainView.width : mainView.width / 3
            anchors.left: portraitMode ? parent.left : flatShadingToggle.right
            anchors.top: portraitMode ? flatShadingToggle.bottom : selection.bottom

            text: "Show Surface Grid"

            onClicked: {
                if (surfaceSeries.drawMode & Surface3DSeries.DrawWireframe) {
                    surfaceSeries.drawMode &= ~Surface3DSeries.DrawWireframe;
                    text = "Show Surface Grid"
                } else {
                    surfaceSeries.drawMode |= Surface3DSeries.DrawWireframe;
                    text = "Hide Surface Grid"
                }
            }
        }

        // Exit
        Button {
            id: exitButton
            width: portraitMode ? mainView.width : mainView.width / 3
            anchors.left: portraitMode ? parent.left : surfaceGridToggle.right
            anchors.top: portraitMode ? surfaceGridToggle.bottom : selection.bottom

            text: "Quit"

            onClicked: Qt.quit();
        }
    }

    //! [4]
    function generateData() {
        dataSource.generateData(mainView.sampleCache, mainView.sampleRows,
                                mainView.sampleColumns, surfaceGraph.axisX.min,
                                surfaceGraph.axisX.max, surfaceGraph.axisY.min,
                                surfaceGraph.axisY.max, surfaceGraph.axisZ.min,
                                surfaceGraph.axisZ.max)
    }
    //! [4]
}

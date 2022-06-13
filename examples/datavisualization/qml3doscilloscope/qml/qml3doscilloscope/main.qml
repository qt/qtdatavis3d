// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtDataVisualization 1.2
import "."

Item {
    id: mainView
    width: 1280
    height: 1024

    property int sampleColumns: sampleSlider.value
    property int sampleRows: sampleColumns / 2
    property int sampleCache: 24

    onSampleRowsChanged: {
        surfaceSeries.selectedPoint = surfaceSeries.invalidSelectionPosition
        generateData()
    }

    Item {
        id: dataView
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height - buttonLayout.height

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
        width: parent.width
        height: flatShadingToggle.implicitHeight * 2
        anchors.left: parent.left
        anchors.top: parent.top
        color: surfaceGraph.theme.backgroundColor

        ColumnLayout {
            anchors.fill: parent
            RowLayout {
                id: sliderLayout
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumHeight: flatShadingToggle.implicitHeight
                spacing: 0

                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: samplesText.implicitWidth + 120
                    Layout.maximumWidth: samplesText.implicitWidth + 120
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    border.color: "gray"
                    border.width: 1
                    radius: 4

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: parent.border.width + 1

                        Slider {
                            id: sampleSlider
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            Layout.minimumWidth: 80
                            from: mainView.sampleCache * 2
                            to: from * 10
                            stepSize: mainView.sampleCache
                            Component.onCompleted: value = from * 2
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.minimumWidth: samplesText.implicitWidth + 10
                            Layout.maximumWidth: samplesText.implicitWidth + 10
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                            Text {
                                id: samplesText
                                text: "Samples: " + (mainView.sampleRows * mainView.sampleColumns)
                                anchors.fill: parent
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: frequencyText.implicitWidth + 120
                    Layout.maximumWidth: frequencyText.implicitWidth + 120
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    border.color: "gray"
                    border.width: 1
                    radius: 4

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: parent.border.width + 1

                        Slider {
                            id: frequencySlider
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            Layout.minimumWidth: 80
                            from: 2
                            to: 60
                            stepSize: 2
                            value: 30
                        }

                        Rectangle {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Layout.minimumWidth: frequencyText.implicitWidth + 10
                            Layout.maximumWidth: frequencyText.implicitWidth + 10
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                            Text {
                                id: frequencyText
                                text: "Freq: " + frequencySlider.value + " Hz"
                                anchors.fill: parent
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: fpsText.implicitWidth + 10
                    Layout.maximumWidth: fpsText.implicitWidth + 10
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

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

                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: selectionText.implicitWidth + 10
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

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
            }

            RowLayout {
                id: buttonLayout
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumHeight: flatShadingToggle.implicitHeight
                spacing: 0

                Button {
                    id: flatShadingToggle
                    Layout.fillHeight: true
                    Layout.fillWidth: true

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

                Button {
                    id: surfaceGridToggle
                    Layout.fillHeight: true
                    Layout.fillWidth: true

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

                Button {
                    id: exitButton
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    text: "Quit"

                    onClicked: Qt.quit();
                }
            }
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

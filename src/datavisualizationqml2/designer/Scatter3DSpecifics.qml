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

import QtQuick 2.0
import HelperWidgets 2.0
import QtQuick.Layouts 1.0

Column {
    anchors.left: parent.left
    anchors.right: parent.right

    Section {
        anchors.left: parent.left
        anchors.right: parent.right
        caption: qsTr("Scatter3D")

        SectionLayout {
            Label {
                text: qsTr("renderingMode")
                toolTip: qsTr("Rendering Mode")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                ComboBox {
                    backendValue: backendValues.renderingMode
                    model: ["RenderIndirect", "RenderDirectToBackground",
                        "RenderDirectToBackground_NoClear"]
                    Layout.fillWidth: true
                    scope: "AbstractGraph3D"
                }
            }
            Label {
                text: qsTr("msaaSamples")
                toolTip: qsTr("MSAA Sample Count")
                Layout.fillWidth: true
            }
            SpinBox {
                suffix: " x MSAA"
                backendValue: backendValues.msaaSamples
                minimumValue: 0
                maximumValue: 16
                Layout.fillWidth: true
            }
            Label {
                text: qsTr("shadowQuality")
                toolTip: qsTr("Shadow Quality")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                ComboBox {
                    backendValue: backendValues.shadowQuality
                    model: ["ShadowQualityNone", "ShadowQualityLow", "ShadowQualityMedium",
                        "ShadowQualityHigh", "ShadowQualitySoftLow", "ShadowQualitySoftMedium",
                        "ShadowQualitySoftHigh"]
                    Layout.fillWidth: true
                    scope: "AbstractGraph3D"
                }
            }
            Label {
                text: qsTr("selectionMode")
                toolTip: qsTr("Selection Mode")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                ComboBox {
                    backendValue: backendValues.selectionMode
                    model: ["SelectionNone", "SelectionItem"]
                    Layout.fillWidth: true
                    scope: "AbstractGraph3D"
                }
            }
            Label {
                text: qsTr("measureFps")
                toolTip: qsTr("Measure Frames Per Second")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                CheckBox {
                    backendValue: backendValues.measureFps
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("orthoProjection")
                toolTip: qsTr("Use Orthographic Projection")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                CheckBox {
                    backendValue: backendValues.orthoProjection
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("aspectRatio")
                toolTip: qsTr("Horizontal to Vertical Aspect Ratio")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                SpinBox {
                    backendValue: backendValues.aspectRatio
                    minimumValue: 0.1
                    maximumValue: 10.0
                    stepSize: 0.1
                    decimals: 1
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("optimizationHints")
                toolTip: qsTr("Optimization Hints")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                ComboBox {
                    backendValue: backendValues.optimizationHints
                    model: ["OptimizationDefault", "OptimizationStatic"]
                    Layout.fillWidth: true
                    scope: "AbstractGraph3D"
                }
            }
            Label {
                text: qsTr("polar")
                toolTip: qsTr("Use Polar Coordinates")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                CheckBox {
                    id: polarCheckbox
                    backendValue: backendValues.polar
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("radialLabelOffset")
                toolTip: qsTr("Radial Label Offset")
                Layout.fillWidth: true
                visible: polarCheckbox.checked
            }
            SecondColumnLayout {
                visible: polarCheckbox.checked
                SpinBox {
                    backendValue: backendValues.radialLabelOffset
                    minimumValue: 0.0
                    maximumValue: 1.0
                    stepSize: 0.01
                    decimals: 1
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("horizontalAspectRatio")
                toolTip: qsTr("Horizontal Aspect Ratio")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                SpinBox {
                    backendValue: backendValues.horizontalAspectRatio
                    minimumValue: 0.0
                    maximumValue: 100.0
                    stepSize: 0.01
                    decimals: 2
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("margin")
                toolTip: qsTr("Graph Margin")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                SpinBox {
                    backendValue: backendValues.margin
                    minimumValue: -1.0
                    maximumValue: 100.0
                    stepSize: 0.1
                    decimals: 1
                    Layout.fillWidth: true
                }
            }
        }
    }
}

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtQuick Enterprise Controls Add-on.
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
import HelperWidgets 2.0
import QtQuick.Layouts 1.0

Column {
    anchors.left: parent.left
    anchors.right: parent.right

    Section {
        anchors.left: parent.left
        anchors.right: parent.right
        caption: qsTr("Bars3D")

        SectionLayout {
            Label {
                text: qsTr("multiSeriesUniform")
                toolTip: qsTr("Multiseries Uniform")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                CheckBox {
                    backendValue: backendValues.multiSeriesUniform
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("barThickness")
                toolTip: qsTr("Bar Thickness Ratio")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                SpinBox {
                    backendValue: backendValues.barThickness // Why does this export value as string?
                    minimumValue: 0.001
                    maximumValue: 10.0
                    stepSize: 0.001
                    decimals: 3
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("barSpacing")
                toolTip: qsTr("Bar Spacing")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                SpinBox {
                    prefix: "col: "
                    backendValue: backendValues.barSpacing.width // Does not work
                    minimumValue: 0.0
                    maximumValue: 10.0
                    stepSize: 0.01
                    decimals: 2
                    Layout.fillWidth: true
                }
                SpinBox {
                    prefix: "row: "
                    backendValue: backendValues.barSpacing.height // Does not work
                    minimumValue: 0.0
                    maximumValue: 10.0
                    stepSize: 0.01
                    decimals: 2
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("barSpacingRelative")
                toolTip: qsTr("Bar Spacing Relative")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                CheckBox {
                    backendValue: backendValues.barSpacingRelative
                    Layout.fillWidth: true
                }
            }
            Label {
                text: qsTr("renderingMode")
                toolTip: qsTr("Rendering Mode")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                ComboBox {
                    backendValue: backendValues.renderingMode
                    model: ["RenderIndirect", "RenderDirectoToBackground",
                        "RenderDirectoToBackground_NoClear"]
                    Layout.fillWidth: true
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
                        "ShadowQualityHigh", "ShadowQualityLowSoft", "ShadowQualityMediumSoft",
                        "ShadowQualityHighSoft"]
                    Layout.fillWidth: true
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
                    model: [
                        "SelectionNone",
                        "SelectionItem",
                        "SelectionRow",
                        "SelectionItemAndRow",
                        "SelectionColumn",
                        "SelectionItemAndColumn",
                        "SelectionRowAndColumn",
                        "SelectionItemRowAndColumn",
                        "SelectionRow | SelectionSlice",
                        "SelectionItemAndRow | SelectionSlice",
                        "SelectionColumn | SelectionSlice",
                        "SelectionItemAndColumn | SelectionSlice",
                        "SelectionRow | SelectionMultiSeries",
                        "SelectionItemAndRow | SelectionMultiSeries",
                        "SelectionColumn | SelectionMultiSeries",
                        "SelectionItemAndColumn | SelectionMultiSeries",
                        "SelectionRow | SelectionSlice | SelectionMultiSeries",
                        "SelectionItemAndRow | SelectionSlice | SelectionMultiSeries",
                        "SelectionColumn | SelectionSlice | SelectionMultiSeries",
                        "SelectionItemAndColumn | SelectionSlice | SelectionMultiSeries"
                    ]
                    Layout.fillWidth: true
                }
            }
        }
    }
}

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
        caption: qsTr("Surface3D")

        SectionLayout {
            Label {
                text: qsTr("clearWindowBeforeRendering")
                toolTip: qsTr("Clear Window Before Rendering")
                Layout.fillWidth: true
            }
            SecondColumnLayout {
                CheckBox {
                    backendValue: backendValues.clearWindowBeforeRendering
                    Layout.fillWidth: true
                }
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
                        "SelectionRow | SelectionSlice",
                        "SelectionItemAndRow | SelectionSlice",
                        "SelectionColumn | SelectionSlice",
                        "SelectionItemAndColumn | SelectionSlice",
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

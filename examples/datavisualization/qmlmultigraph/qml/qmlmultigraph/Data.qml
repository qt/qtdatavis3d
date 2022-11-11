// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick

Item {
    property alias sharedPrimaryData: dataModel1
    property alias sharedSecondaryData: dataModel2

    ListModel {
        id: dataModel1
        ListElement{ row: "0"; col: "0"; longitude: "20.0"; latitude: "10.0"; pop_density: "4.75"; }
        ListElement{ row: "1"; col: "0"; longitude: "21.1"; latitude: "10.3"; pop_density: "3.00"; }
        ListElement{ row: "2"; col: "0"; longitude: "22.5"; latitude: "10.7"; pop_density: "1.24"; }
        ListElement{ row: "3"; col: "0"; longitude: "24.0"; latitude: "10.5"; pop_density: "2.53"; }
        ListElement{ row: "0"; col: "1"; longitude: "20.2"; latitude: "11.2"; pop_density: "3.55"; }
        ListElement{ row: "1"; col: "1"; longitude: "21.3"; latitude: "11.5"; pop_density: "3.03"; }
        ListElement{ row: "2"; col: "1"; longitude: "22.6"; latitude: "11.7"; pop_density: "3.46"; }
        ListElement{ row: "3"; col: "1"; longitude: "23.4"; latitude: "11.5"; pop_density: "4.12"; }
        ListElement{ row: "0"; col: "2"; longitude: "20.2"; latitude: "12.3"; pop_density: "3.37"; }
        ListElement{ row: "1"; col: "2"; longitude: "21.1"; latitude: "12.4"; pop_density: "2.98"; }
        ListElement{ row: "2"; col: "2"; longitude: "22.5"; latitude: "12.1"; pop_density: "3.33"; }
        ListElement{ row: "3"; col: "2"; longitude: "23.3"; latitude: "12.7"; pop_density: "3.23"; }
        ListElement{ row: "0"; col: "3"; longitude: "20.7"; latitude: "13.3"; pop_density: "5.34"; }
        ListElement{ row: "1"; col: "3"; longitude: "21.5"; latitude: "13.2"; pop_density: "4.54"; }
        ListElement{ row: "2"; col: "3"; longitude: "22.4"; latitude: "13.6"; pop_density: "4.65"; }
        ListElement{ row: "3"; col: "3"; longitude: "23.2"; latitude: "13.4"; pop_density: "6.67"; }
        ListElement{ row: "0"; col: "4"; longitude: "20.6"; latitude: "15.0"; pop_density: "6.01"; }
        ListElement{ row: "1"; col: "4"; longitude: "21.3"; latitude: "14.6"; pop_density: "5.83"; }
        ListElement{ row: "2"; col: "4"; longitude: "22.5"; latitude: "14.8"; pop_density: "7.32"; }
        ListElement{ row: "3"; col: "4"; longitude: "23.7"; latitude: "14.3"; pop_density: "6.90"; }
    }

    ListModel {
        id: dataModel2
        ListElement{ row: "0"; col: "0"; longitude: "40.0"; latitude: "0.0"; pop_density: "47.5"; }
        ListElement{ row: "1"; col: "0"; longitude: "41.1"; latitude: "0.3"; pop_density: "30.0"; }
        ListElement{ row: "2"; col: "0"; longitude: "42.5"; latitude: "0.7"; pop_density: "12.4"; }
        ListElement{ row: "0"; col: "1"; longitude: "40.2"; latitude: "1.2"; pop_density: "35.5"; }
        ListElement{ row: "1"; col: "1"; longitude: "41.3"; latitude: "1.5"; pop_density: "30.3"; }
        ListElement{ row: "2"; col: "1"; longitude: "42.6"; latitude: "1.7"; pop_density: "34.6"; }
        ListElement{ row: "0"; col: "2"; longitude: "40.2"; latitude: "2.3"; pop_density: "33.7"; }
        ListElement{ row: "1"; col: "2"; longitude: "41.1"; latitude: "2.4"; pop_density: "29.8"; }
        ListElement{ row: "2"; col: "2"; longitude: "42.5"; latitude: "2.1"; pop_density: "33.3"; }
        ListElement{ row: "0"; col: "3"; longitude: "40.7"; latitude: "3.3"; pop_density: "53.4"; }
        ListElement{ row: "1"; col: "3"; longitude: "41.5"; latitude: "3.2"; pop_density: "45.4"; }
        ListElement{ row: "2"; col: "3"; longitude: "42.4"; latitude: "3.6"; pop_density: "46.5"; }
    }
}


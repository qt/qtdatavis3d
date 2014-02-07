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

import QtQuick 2.1
import QtDataVisualization 1.0

Item {
    property alias model: dataModel
    property alias proxy: modelProxy
    property alias series: barSeries

    ItemModelBarDataProxy {
        id: modelProxy
        itemModel: dataModel
        rowRole: "year"
        columnRole: "month"
        valueRole: "expenses"
        rotationRole: "angle"
    }

    Bar3DSeries {
        id: barSeries
        dataProxy: modelProxy
        itemLabelFormat: "@valueTitle for @colLabel, @rowLabel: @valueLabel"

        onMeshAngleChanged: console.log("angle changed:", angle)
    }

    ListModel {
        id: dataModel
        ListElement{ year: "2006"; month: "Jan"; expenses: "4"; angle: "1";  income: "5" }
        ListElement{ year: "2006"; month: "Feb"; expenses: "5"; angle: "2";  income: "6" }
        ListElement{ year: "2006"; month: "Mar"; expenses: "7"; angle: "3";  income: "4" }
        ListElement{ year: "2006"; month: "Apr"; expenses: "3"; angle: "4";  income: "2" }
        ListElement{ year: "2006"; month: "May"; expenses: "4"; angle: "5";  income: "1" }
        ListElement{ year: "2006"; month: "Jun"; expenses: "2"; angle: "6";  income: "2" }
        ListElement{ year: "2006"; month: "Jul"; expenses: "1"; angle: "7";  income: "3" }
        ListElement{ year: "2006"; month: "Aug"; expenses: "5"; angle: "8";  income: "1" }
        ListElement{ year: "2006"; month: "Sep"; expenses: "2"; angle: "9";  income: "3" }
        ListElement{ year: "2006"; month: "Oct"; expenses: "5"; angle: "10";  income: "2" }
        ListElement{ year: "2006"; month: "Nov"; expenses: "8"; angle: "11";  income: "5" }
        ListElement{ year: "2006"; month: "Dec"; expenses: "3"; angle: "12";  income: "3" }

        ListElement{ year: "2007"; month: "Jan"; expenses: "3"; angle: "13";  income: "1" }
        ListElement{ year: "2007"; month: "Feb"; expenses: "4"; angle: "14";  income: "2" }
        ListElement{ year: "2007"; month: "Mar"; expenses: "12";angle: "15";  income: "4" }
        ListElement{ year: "2007"; month: "Apr"; expenses: "13";angle: "16";  income: "6" }
        ListElement{ year: "2007"; month: "May"; expenses: "14";angle: "17";  income: "11" }
        ListElement{ year: "2007"; month: "Jun"; expenses: "7"; angle: "18";  income: "7" }
        ListElement{ year: "2007"; month: "Jul"; expenses: "6"; angle: "19";  income: "4" }
        ListElement{ year: "2007"; month: "Aug"; expenses: "4"; angle: "20";  income: "15" }
        ListElement{ year: "2007"; month: "Sep"; expenses: "2"; angle: "21";  income: "18" }
        ListElement{ year: "2007"; month: "Oct"; expenses: "29";angle: "22";  income: "25" }
        ListElement{ year: "2007"; month: "Nov"; expenses: "23";angle: "23";  income: "29" }
        ListElement{ year: "2007"; month: "Dec"; expenses: "5"; angle: "24";  income: "9" }

        ListElement{ year: "2008"; month: "Jan"; expenses: "3";  income: "8" }
        ListElement{ year: "2008"; month: "Feb"; expenses: "8";  income: "14" }
        ListElement{ year: "2008"; month: "Mar"; expenses: "10"; income: "20" }
        ListElement{ year: "2008"; month: "Apr"; expenses: "12"; income: "24" }
        ListElement{ year: "2008"; month: "May"; expenses: "10"; income: "19" }
        ListElement{ year: "2008"; month: "Jun"; expenses: "5";  income: "8" }
        ListElement{ year: "2008"; month: "Jul"; expenses: "1";  income: "4" }
        ListElement{ year: "2008"; month: "Aug"; expenses: "7";  income: "12" }
        ListElement{ year: "2008"; month: "Sep"; expenses: "4";  income: "16" }
        ListElement{ year: "2008"; month: "Oct"; expenses: "22"; income: "33" }
        ListElement{ year: "2008"; month: "Nov"; expenses: "16"; income: "25" }
        ListElement{ year: "2008"; month: "Dec"; expenses: "2";  income: "7" }

        ListElement{ year: "2009"; month: "Jan"; expenses: "4"; angle: "37";  income: "5"  }
        ListElement{ year: "2009"; month: "Feb"; expenses: "4"; angle: "38";  income: "7"  }
        ListElement{ year: "2009"; month: "Mar"; expenses: "11";angle: "39";  income: "14"  }
        ListElement{ year: "2009"; month: "Apr"; expenses: "16";angle: "40";  income: "22"  }
        ListElement{ year: "2009"; month: "May"; expenses: "3"; angle: "41";  income: "5"  }
        ListElement{ year: "2009"; month: "Jun"; expenses: "4"; angle: "42";  income: "8"  }
        ListElement{ year: "2009"; month: "Jul"; expenses: "7"; angle: "43";  income: "9"  }
        ListElement{ year: "2009"; month: "Aug"; expenses: "9"; angle: "44";  income: "13"  }
        ListElement{ year: "2009"; month: "Sep"; expenses: "1"; angle: "45";  income: "6"  }
        ListElement{ year: "2009"; month: "Oct"; expenses: "14";angle: "46";  income: "25"  }
        ListElement{ year: "2009"; month: "Nov"; expenses: "19";angle: "47";  income: "29"  }
        ListElement{ year: "2009"; month: "Dec"; expenses: "5"; angle: "48";  income: "7"  }

        ListElement{ year: "2010"; month: "Jan"; expenses: "14";angle: "49";  income: "22"  }
        ListElement{ year: "2010"; month: "Feb"; expenses: "5"; angle: "50";  income: "7"  }
        ListElement{ year: "2010"; month: "Mar"; expenses: "1"; angle: "51";  income: "9"  }
        ListElement{ year: "2010"; month: "Apr"; expenses: "1"; angle: "52";  income: "12"  }
        ListElement{ year: "2010"; month: "May"; expenses: "5"; angle: "53";  income: "9"  }
        ListElement{ year: "2010"; month: "Jun"; expenses: "5"; angle: "54";  income: "8"  }
        ListElement{ year: "2010"; month: "Jul"; expenses: "3"; angle: "55";  income: "7"  }
        ListElement{ year: "2010"; month: "Aug"; expenses: "1"; angle: "56";  income: "5"  }
        ListElement{ year: "2010"; month: "Sep"; expenses: "2"; angle: "57";  income: "4"  }
        ListElement{ year: "2010"; month: "Oct"; expenses: "10";angle: "58";  income: "13"  }
        ListElement{ year: "2010"; month: "Nov"; expenses: "12";angle: "59";  income: "17"  }
        ListElement{ year: "2010"; month: "Dec"; expenses: "6"; angle: "60";  income: "9"  }

        ListElement{ year: "2011"; month: "Jan"; expenses: "2"; angle: "61";  income: "6"  }
        ListElement{ year: "2011"; month: "Feb"; expenses: "4"; angle: "62";  income: "8"  }
        ListElement{ year: "2011"; month: "Mar"; expenses: "7"; angle: "63";  income: "12"  }
        ListElement{ year: "2011"; month: "Apr"; expenses: "9"; angle: "64";  income: "15"  }
        ListElement{ year: "2011"; month: "May"; expenses: "7"; angle: "65";  income: "19"  }
        ListElement{ year: "2011"; month: "Jun"; expenses: "9"; angle: "66";  income: "18"  }
        ListElement{ year: "2011"; month: "Jul"; expenses: "13";angle: "67";  income: "17"  }
        ListElement{ year: "2011"; month: "Aug"; expenses: "5"; angle: "68";  income: "9"  }
        ListElement{ year: "2011"; month: "Sep"; expenses: "3"; angle: "69";  income: "8"  }
        ListElement{ year: "2011"; month: "Oct"; expenses: "13";angle: "70";  income: "15"  }
        ListElement{ year: "2011"; month: "Nov"; expenses: "8"; angle: "71";  income: "17"  }
        ListElement{ year: "2011"; month: "Dec"; expenses: "7"; angle: "72";  income: "10"  }

        ListElement{ year: "2012"; month: "Jan"; expenses: "12";angle: "73";  income: "16"  }
        ListElement{ year: "2012"; month: "Feb"; expenses: "24";angle: "74";  income: "28"  }
        ListElement{ year: "2012"; month: "Mar"; expenses: "27";angle: "75";  income: "22"  }
        ListElement{ year: "2012"; month: "Apr"; expenses: "29";angle: "76";  income: "25"  }
        ListElement{ year: "2012"; month: "May"; expenses: "27";angle: "77";  income: "29"  }
        ListElement{ year: "2012"; month: "Jun"; expenses: "19";angle: "78";  income: "18"  }
        ListElement{ year: "2012"; month: "Jul"; expenses: "13";angle: "79";  income: "17"  }
        ListElement{ year: "2012"; month: "Aug"; expenses: "15";angle: "80";  income: "19"  }
        ListElement{ year: "2012"; month: "Sep"; expenses: "3"; angle: "81";  income: "8"  }
        ListElement{ year: "2012"; month: "Oct"; expenses: "3"; angle: "82";  income: "6"  }
        ListElement{ year: "2012"; month: "Nov"; expenses: "4"; angle: "83";  income: "8"  }
        ListElement{ year: "2012"; month: "Dec"; expenses: "5"; angle: "84";  income: "9"  }
    }
}

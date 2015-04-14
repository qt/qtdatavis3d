/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

import QtQuick 2.1
import QtDataVisualization 1.1

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
        valueRolePattern: /@*(\d*)t*/
        rotationRolePattern: /jjj/
        valueRoleReplace: "\\1"
        rotationRoleReplace: "1"
    }

    Bar3DSeries {
        id: barSeries
        dataProxy: modelProxy
        itemLabelFormat: "@valueTitle for @colLabel, @rowLabel: @valueLabel"

        onMeshAngleChanged: console.log("angle changed:", angle)
    }

    ListModel {
        id: dataModel
        ListElement{ year: "2006"; month: "Jan"; expenses: "@@4ttt"; angle: "jjj1";  income: "5" }
        ListElement{ year: "2006"; month: "Feb"; expenses: "@@5ttt"; angle: "jjj2";  income: "6" }
        ListElement{ year: "2006"; month: "Mar"; expenses: "@@7ttt"; angle: "jjj3";  income: "4" }
        ListElement{ year: "2006"; month: "Apr"; expenses: "@@3ttt"; angle: "jjj4";  income: "2" }
        ListElement{ year: "2006"; month: "May"; expenses: "@@4ttt"; angle: "jjj5";  income: "1" }
        ListElement{ year: "2006"; month: "Jun"; expenses: "@@2ttt"; angle: "jjj6";  income: "2" }
        ListElement{ year: "2006"; month: "Jul"; expenses: "@@1ttt"; angle: "jjj7";  income: "3" }
        ListElement{ year: "2006"; month: "Aug"; expenses: "@@5ttt"; angle: "jjj8";  income: "1" }
        ListElement{ year: "2006"; month: "Sep"; expenses: "@@2ttt"; angle: "jjj9";  income: "3" }
        ListElement{ year: "2006"; month: "Oct"; expenses: "@@5ttt"; angle: "jjj10";  income: "2" }
        ListElement{ year: "2006"; month: "Nov"; expenses: "@@8ttt"; angle: "jjj11";  income: "5" }
        ListElement{ year: "2006"; month: "Dec"; expenses: "@@3ttt"; angle: "jjj12";  income: "3" }

        ListElement{ year: "2007"; month: "Jan"; expenses: "@@3ttt"; angle: "jjj13";  income: "1" }
        ListElement{ year: "2007"; month: "Feb"; expenses: "@@4ttt"; angle: "jjj14";  income: "2" }
        ListElement{ year: "2007"; month: "Mar"; expenses: "@@12ttt"; angle: "jjj15";  income: "4" }
        ListElement{ year: "2007"; month: "Apr"; expenses: "@@13ttt"; angle: "jjj16";  income: "6" }
        ListElement{ year: "2007"; month: "May"; expenses: "@@14ttt"; angle: "jjj17";  income: "11" }
        ListElement{ year: "2007"; month: "Jun"; expenses: "@@7ttt"; angle: "jjj18";  income: "7" }
        ListElement{ year: "2007"; month: "Jul"; expenses: "@@6ttt"; angle: "jjj19";  income: "4" }
        ListElement{ year: "2007"; month: "Aug"; expenses: "@@4ttt"; angle: "jjj20";  income: "15" }
        ListElement{ year: "2007"; month: "Sep"; expenses: "@@2ttt"; angle: "jjj21";  income: "18" }
        ListElement{ year: "2007"; month: "Oct"; expenses: "@@29ttt"; angle: "jjj22";  income: "25" }
        ListElement{ year: "2007"; month: "Nov"; expenses: "@@23ttt"; angle: "jjj23";  income: "29" }
        ListElement{ year: "2007"; month: "Dec"; expenses: "@@5ttt"; angle: "jjj24";  income: "9" }

        ListElement{ year: "2008"; month: "Jan"; expenses: "@@3";  income: "8" }
        ListElement{ year: "2008"; month: "Feb"; expenses: "@@8";  income: "14" }
        ListElement{ year: "2008"; month: "Mar"; expenses: "@@10"; income: "20" }
        ListElement{ year: "2008"; month: "Apr"; expenses: "@@12"; income: "24" }
        ListElement{ year: "2008"; month: "May"; expenses: "@@10"; income: "19" }
        ListElement{ year: "2008"; month: "Jun"; expenses: "@@5";  income: "8" }
        ListElement{ year: "2008"; month: "Jul"; expenses: "@@1";  income: "4" }
        ListElement{ year: "2008"; month: "Aug"; expenses: "@@7";  income: "12" }
        ListElement{ year: "2008"; month: "Sep"; expenses: "@@4";  income: "16" }
        ListElement{ year: "2008"; month: "Oct"; expenses: "@@22"; income: "33" }
        ListElement{ year: "2008"; month: "Nov"; expenses: "@@16"; income: "25" }
        ListElement{ year: "2008"; month: "Dec"; expenses: "@@2";  income: "7" }

        ListElement{ year: "2009"; month: "Jan"; expenses: "@@4ttt"; angle: "jjj37";  income: "5"  }
        ListElement{ year: "2009"; month: "Feb"; expenses: "@@4ttt"; angle: "jjj38";  income: "7"  }
        ListElement{ year: "2009"; month: "Mar"; expenses: "@@11ttt"; angle: "jjj39";  income: "14"  }
        ListElement{ year: "2009"; month: "Apr"; expenses: "@@16ttt"; angle: "jjj40";  income: "22"  }
        ListElement{ year: "2009"; month: "May"; expenses: "@@3ttt"; angle: "jjj41";  income: "5"  }
        ListElement{ year: "2009"; month: "Jun"; expenses: "@@4ttt"; angle: "jjj42";  income: "8"  }
        ListElement{ year: "2009"; month: "Jul"; expenses: "@@7ttt"; angle: "jjj43";  income: "9"  }
        ListElement{ year: "2009"; month: "Aug"; expenses: "@@9ttt"; angle: "jjj44";  income: "13"  }
        ListElement{ year: "2009"; month: "Sep"; expenses: "@@1ttt"; angle: "jjj45";  income: "6"  }
        ListElement{ year: "2009"; month: "Oct"; expenses: "@@14ttt"; angle: "jjj46";  income: "25"  }
        ListElement{ year: "2009"; month: "Nov"; expenses: "@@19ttt"; angle: "jjj47";  income: "29"  }
        ListElement{ year: "2009"; month: "Dec"; expenses: "@@5ttt"; angle: "jjj48";  income: "7"  }

        ListElement{ year: "2010"; month: "Jan"; expenses: "@@14ttt"; angle: "jjj49";  income: "22"  }
        ListElement{ year: "2010"; month: "Feb"; expenses: "@@5ttt"; angle: "jjj50";  income: "7"  }
        ListElement{ year: "2010"; month: "Mar"; expenses: "@@1ttt"; angle: "jjj51";  income: "9"  }
        ListElement{ year: "2010"; month: "Apr"; expenses: "@@1ttt"; angle: "jjj52";  income: "12"  }
        ListElement{ year: "2010"; month: "May"; expenses: "@@5ttt"; angle: "jjj53";  income: "9"  }
        ListElement{ year: "2010"; month: "Jun"; expenses: "@@5ttt"; angle: "jjj54";  income: "8"  }
        ListElement{ year: "2010"; month: "Jul"; expenses: "@@3ttt"; angle: "jjj55";  income: "7"  }
        ListElement{ year: "2010"; month: "Aug"; expenses: "@@1ttt"; angle: "jjj56";  income: "5"  }
        ListElement{ year: "2010"; month: "Sep"; expenses: "@@2ttt"; angle: "jjj57";  income: "4"  }
        ListElement{ year: "2010"; month: "Oct"; expenses: "@@10ttt"; angle: "jjj58";  income: "13"  }
        ListElement{ year: "2010"; month: "Nov"; expenses: "@@12ttt"; angle: "jjj59";  income: "17"  }
        ListElement{ year: "2010"; month: "Dec"; expenses: "@@6ttt"; angle: "jjj60";  income: "9"  }

        ListElement{ year: "2011"; month: "Jan"; expenses: "@@2ttt"; angle: "jjj61";  income: "6"  }
        ListElement{ year: "2011"; month: "Feb"; expenses: "@@4ttt"; angle: "jjj62";  income: "8"  }
        ListElement{ year: "2011"; month: "Mar"; expenses: "@@7ttt"; angle: "jjj63";  income: "12"  }
        ListElement{ year: "2011"; month: "Apr"; expenses: "@@9ttt"; angle: "jjj64";  income: "15"  }
        ListElement{ year: "2011"; month: "May"; expenses: "@@7ttt"; angle: "jjj65";  income: "19"  }
        ListElement{ year: "2011"; month: "Jun"; expenses: "@@9ttt"; angle: "jjj66";  income: "18"  }
        ListElement{ year: "2011"; month: "Jul"; expenses: "@@13ttt"; angle: "jjj67";  income: "17"  }
        ListElement{ year: "2011"; month: "Aug"; expenses: "@@5ttt"; angle: "jjj68";  income: "9"  }
        ListElement{ year: "2011"; month: "Sep"; expenses: "@@3ttt"; angle: "jjj69";  income: "8"  }
        ListElement{ year: "2011"; month: "Oct"; expenses: "@@13ttt"; angle: "jjj70";  income: "15"  }
        ListElement{ year: "2011"; month: "Nov"; expenses: "@@8ttt"; angle: "jjj71";  income: "17"  }
        ListElement{ year: "2011"; month: "Dec"; expenses: "@@7ttt"; angle: "jjj72";  income: "10"  }

        ListElement{ year: "2012"; month: "Jan"; expenses: "@@12ttt"; angle: "jjj73";  income: "16"  }
        ListElement{ year: "2012"; month: "Feb"; expenses: "@@24ttt"; angle: "jjj74";  income: "28"  }
        ListElement{ year: "2012"; month: "Mar"; expenses: "@@27ttt"; angle: "jjj75";  income: "22"  }
        ListElement{ year: "2012"; month: "Apr"; expenses: "@@29ttt"; angle: "jjj76";  income: "25"  }
        ListElement{ year: "2012"; month: "May"; expenses: "@@27ttt"; angle: "jjj77";  income: "29"  }
        ListElement{ year: "2012"; month: "Jun"; expenses: "@@19ttt"; angle: "jjj78";  income: "18"  }
        ListElement{ year: "2012"; month: "Jul"; expenses: "@@13ttt"; angle: "jjj79";  income: "17"  }
        ListElement{ year: "2012"; month: "Aug"; expenses: "@@15ttt"; angle: "jjj80";  income: "19"  }
        ListElement{ year: "2012"; month: "Sep"; expenses: "@@3ttt"; angle: "jjj81";  income: "8"  }
        ListElement{ year: "2012"; month: "Oct"; expenses: "@@3ttt"; angle: "jjj82";  income: "6"  }
        ListElement{ year: "2012"; month: "Nov"; expenses: "@@4ttt"; angle: "jjj83";  income: "8"  }
        ListElement{ year: "2012"; month: "Dec"; expenses: "@@5ttt"; angle: "jjj84";  income: "9"  }
    }
}

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
import com.digia.QtDataVis3D 1.0

Item {
    property alias mapping: valueMapping
    property alias model: dataModel
    property alias proxy: modelProxy

    BarDataMapping {
        id: valueMapping
        rowRole: "year"
        columnRole: "month"
        valueRole: "expenses"
        rowCategories: ["2000", "2001", "2002", "2003"]
        columnCategories: ["Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    }

    ItemModelBarDataProxy {
        id: modelProxy
        activeMapping: valueMapping
        itemModel: dataModel
    }

    ListModel {
        id: dataModel
        ListElement{ year: "2000"; month: "Jan"; expenses: "3";  income: "8" }
        ListElement{ year: "2000"; month: "Feb"; expenses: "8";  income: "14" }
        ListElement{ year: "2000"; month: "Mar"; expenses: "10"; income: "20" }
        ListElement{ year: "2000"; month: "Apr"; expenses: "12"; income: "24" }
        ListElement{ year: "2000"; month: "May"; expenses: "10"; income: "19" }
        ListElement{ year: "2000"; month: "Jun"; expenses: "5";  income: "8" }
        ListElement{ year: "2000"; month: "Jul"; expenses: "1";  income: "4" }
        ListElement{ year: "2000"; month: "Aug"; expenses: "7";  income: "12" }
        ListElement{ year: "2000"; month: "Sep"; expenses: "4";  income: "16" }
        ListElement{ year: "2000"; month: "Oct"; expenses: "22"; income: "33" }
        ListElement{ year: "2000"; month: "Nov"; expenses: "16"; income: "25" }
        ListElement{ year: "2000"; month: "Dec"; expenses: "2";  income: "7" }

        ListElement{ year: "2001"; month: "Jan"; expenses: "4";  income: "5"  }
        ListElement{ year: "2001"; month: "Feb"; expenses: "4";  income: "7"  }
        ListElement{ year: "2001"; month: "Mar"; expenses: "11"; income: "14"  }
        ListElement{ year: "2001"; month: "Apr"; expenses: "16"; income: "22"  }
        ListElement{ year: "2001"; month: "May"; expenses: "3";  income: "5"  }
        ListElement{ year: "2001"; month: "Jun"; expenses: "4";  income: "8"  }
        ListElement{ year: "2001"; month: "Jul"; expenses: "7";  income: "9"  }
        ListElement{ year: "2001"; month: "Aug"; expenses: "9";  income: "13"  }
        ListElement{ year: "2001"; month: "Sep"; expenses: "1";  income: "6"  }
        ListElement{ year: "2001"; month: "Oct"; expenses: "14"; income: "25"  }
        ListElement{ year: "2001"; month: "Nov"; expenses: "19"; income: "29"  }
        ListElement{ year: "2001"; month: "Dec"; expenses: "5";  income: "7"  }

        ListElement{ year: "2002"; month: "Jan"; expenses: "14"; income: "22"  }
        ListElement{ year: "2002"; month: "Feb"; expenses: "5";  income: "7"  }
        ListElement{ year: "2002"; month: "Mar"; expenses: "1";  income: "9"  }
        ListElement{ year: "2002"; month: "Apr"; expenses: "1";  income: "12"  }
        ListElement{ year: "2002"; month: "May"; expenses: "5";  income: "9"  }
        ListElement{ year: "2002"; month: "Jun"; expenses: "5";  income: "8"  }
        ListElement{ year: "2002"; month: "Jul"; expenses: "3";  income: "7"  }
        ListElement{ year: "2002"; month: "Aug"; expenses: "1";  income: "5"  }
        ListElement{ year: "2002"; month: "Sep"; expenses: "2";  income: "4"  }
        ListElement{ year: "2002"; month: "Oct"; expenses: "10"; income: "13"  }
        ListElement{ year: "2002"; month: "Nov"; expenses: "12"; income: "17"  }
        ListElement{ year: "2002"; month: "Dec"; expenses: "6";  income: "9"  }

        ListElement{ year: "2003"; month: "Jan"; expenses: "2";  income: "6"  }
        ListElement{ year: "2003"; month: "Feb"; expenses: "4";  income: "8"  }
        ListElement{ year: "2003"; month: "Mar"; expenses: "7";  income: "12"  }
        ListElement{ year: "2003"; month: "Apr"; expenses: "9";  income: "15"  }
        ListElement{ year: "2003"; month: "May"; expenses: "7";  income: "19"  }
        ListElement{ year: "2003"; month: "Jun"; expenses: "9";  income: "18"  }
        ListElement{ year: "2003"; month: "Jul"; expenses: "13"; income: "17"  }
        ListElement{ year: "2003"; month: "Aug"; expenses: "5";  income: "9"  }
        ListElement{ year: "2003"; month: "Sep"; expenses: "3";  income: "8"  }
        ListElement{ year: "2003"; month: "Oct"; expenses: "13"; income: "15"  }
        ListElement{ year: "2003"; month: "Nov"; expenses: "8";  income: "17"  }
        ListElement{ year: "2003"; month: "Dec"; expenses: "7";  income: "10"  }
    }
}

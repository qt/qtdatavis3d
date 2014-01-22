/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QSCATTERDATAITEM_P_H
#define QSCATTERDATAITEM_P_H

#include "datavisualizationglobal_p.h"
#include "qscatterdataitem.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QScatterDataItemPrivate
{
public:
    QScatterDataItemPrivate();
    virtual ~QScatterDataItemPrivate();

protected:
    friend class QScatterDataItem;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

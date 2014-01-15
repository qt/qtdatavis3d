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

#ifndef Q3DLIGHT_H
#define Q3DLIGHT_H

#include <QtDataVisualization/q3dobject.h>

namespace QtDataVisualization {

class Q3DLightPrivate;
class Q3DScene;

class QT_DATAVISUALIZATION_EXPORT Q3DLight : public Q3DObject
{
    Q_OBJECT

public:
    Q3DLight(QObject *parent = 0);
    virtual ~Q3DLight();

private:
    QScopedPointer<Q3DLightPrivate> d_ptr;

    Q_DISABLE_COPY(Q3DLight)

    friend class Q3DLightPrivate;
    friend class Q3DScenePrivate;
};

}

#endif // Q3DLIGHT_H

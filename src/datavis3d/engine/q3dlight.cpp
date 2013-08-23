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

#include "q3dlight.h"
#include "q3dscene.h"
#include "q3dlight_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

Q3DLight::Q3DLight(QObject *parent) :
    Q3DObject(parent),
    d_ptr(new Q3DLightPrivate(this))
{
}

void Q3DLight::copyValuesFrom(const Q3DLight &source)
{
    Q3DObject::copyValuesFrom(source);
}

Q3DLight::~Q3DLight()
{
}

Q3DLightPrivate::Q3DLightPrivate(Q3DLight *q) :
    q_ptr(q)
{
}

Q3DLightPrivate::~Q3DLightPrivate()
{
}

QT_DATAVIS3D_END_NAMESPACE

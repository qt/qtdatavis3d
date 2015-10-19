/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include "q3dlight_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class Q3DLight
 * \inmodule QtDataVisualization
 * \brief Representation of a light source in 3D space.
 * \since QtDataVisualization 1.0
 *
 * Q3DLight represents a monochrome non variable light source in 3D space.
 */

/*!
 * \qmltype Light3D
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates Q3DLight
 * \brief Representation of a light source in 3D space.
 *
 * Light3D represents a monochrome non variable light source in 3D space.
 */

/*!
 * Constructs a new 3D light located at origin. An optional \a parent parameter can be given
 * and is then passed to QObject constructor.
 */
Q3DLight::Q3DLight(QObject *parent) :
    Q3DObject(parent),
    d_ptr(new Q3DLightPrivate(this))
{
}

/*!
 * Destroys the light object.
 */
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

void Q3DLightPrivate::sync(Q3DLight &other)
{
    Q_UNUSED(other);
    // Do nothing. Only value light has to sync is the position, which we handle internally.
}

QT_END_NAMESPACE_DATAVISUALIZATION

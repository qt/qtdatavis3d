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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef ABSTRACTRENDERITEM_P_H
#define ABSTRACTRENDERITEM_P_H

#include "datavisualizationglobal_p.h"
#include "labelitem_p.h"

#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class AbstractRenderItem
{
public:
    AbstractRenderItem();
    AbstractRenderItem(const AbstractRenderItem &other);
    virtual ~AbstractRenderItem();

    // Position in 3D scene
    inline void setTranslation(const QVector3D &translation) { m_translation = translation; }
    inline const QVector3D &translation() const {return m_translation; }

    inline const QQuaternion &rotation() const { return m_rotation; }
    inline void setRotation(const QQuaternion &rotation)
    {
        if (rotation.isNull())
            m_rotation = identityQuaternion;
        else
            m_rotation = rotation;
    }

protected:
    QVector3D m_translation;
    QQuaternion m_rotation;

    friend class QAbstractDataItem;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

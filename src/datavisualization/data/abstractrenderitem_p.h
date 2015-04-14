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

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

#ifndef QCUSTOM3DITEM_P_H
#define QCUSTOM3DITEM_P_H

#include "datavisualizationglobal_p.h"
#include "qcustom3ditem.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

struct QCustomItemDirtyBitField {
    bool textureDirty               : 1;
    bool meshDirty                  : 1;
    bool positionDirty              : 1;
    bool scalingDirty               : 1;
    bool rotationDirty              : 1;
    bool visibleDirty               : 1;
    bool shadowCastingDirty         : 1;

    QCustomItemDirtyBitField()
        : textureDirty(false),
          meshDirty(false),
          positionDirty(false),
          scalingDirty(false),
          rotationDirty(false),
          visibleDirty(false),
          shadowCastingDirty(false)
    {
    }
};

class QCustom3DItemPrivate : public QObject
{
    Q_OBJECT
public:
    QCustom3DItemPrivate(QCustom3DItem *q);
    QCustom3DItemPrivate(QCustom3DItem *q, const QString &meshFile, const QVector3D &position,
                         const QVector3D &scaling, const QQuaternion &rotation);
    virtual ~QCustom3DItemPrivate();

    QImage textureImage();
    void clearTextureImage();
    void resetDirtyBits();

public:
    QCustom3DItem *q_ptr;
    QImage m_textureImage;
    QString m_textureFile;
    QString m_meshFile;
    QVector3D m_position;
    bool m_positionAbsolute;
    QVector3D m_scaling;
    bool m_scalingAbsolute;
    QQuaternion m_rotation;
    bool m_visible;
    bool m_shadowCasting;

    bool m_isLabelItem;
    bool m_isVolumeItem;

    QCustomItemDirtyBitField m_dirtyBits;

Q_SIGNALS:
    void needUpdate();

private:
    QCustom3DItemPrivate(QCustom3DItemPrivate *d);

    friend class QCustom3DItem;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

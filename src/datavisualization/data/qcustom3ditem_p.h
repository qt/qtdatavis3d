/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
******************************************************************************/

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

QT_BEGIN_NAMESPACE

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

QT_END_NAMESPACE

#endif

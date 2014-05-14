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

#ifndef QCUSTOM3DITEM_P_H
#define QCUSTOM3DITEM_P_H

#include "qcustom3ditem.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QCustom3DItemPrivate : public QObject
{
    Q_OBJECT
public:
    QCustom3DItemPrivate(QCustom3DItem *q, QObject *parent);
    QCustom3DItemPrivate(QCustom3DItem *q, const QString &meshFile, const QVector3D &position,
                         const QVector3D &scaling, const QQuaternion &rotation, QObject *parent);
    virtual ~QCustom3DItemPrivate();

    QImage textureImage();
    void clearTextureImage();

public:
    QCustom3DItem *q_ptr;
    QImage m_textureImage;
    QString m_textureFile;
    QString m_meshFile;
    QVector3D m_position;
    QVector3D m_scaling;
    QQuaternion m_rotation;
    bool m_visible;

private:
    friend class QCustom3DItem;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

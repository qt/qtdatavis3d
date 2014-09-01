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

#ifndef QCUSTOM3DVOLUME_P_H
#define QCUSTOM3DVOLUME_P_H

#include "qcustom3dvolume.h"
#include "qcustom3ditem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

struct QCustomVolumeDirtyBitField {
    bool textureDimensionsDirty : 1;
    bool sliceIndicesDirty      : 1;
    bool colorTableDirty        : 1;
    bool textureDataDirty       : 1;
    bool textureFormatDirty     : 1;
    bool alphaDirty             : 1;
    bool shaderDirty            : 1;

    QCustomVolumeDirtyBitField()
        : textureDimensionsDirty(false),
          sliceIndicesDirty(false),
          colorTableDirty(false),
          textureDataDirty(false),
          textureFormatDirty(false),
          alphaDirty(false),
          shaderDirty(false)
    {
    }
};

class QCustom3DVolumePrivate : public QCustom3DItemPrivate
{
    Q_OBJECT

public:
    QCustom3DVolumePrivate(QCustom3DVolume *q);
    QCustom3DVolumePrivate(QCustom3DVolume *q, const QVector3D &position, const QVector3D &scaling,
                           const QQuaternion &rotation, int textureWidth,
                           int textureHeight, int textureDepth, QVector<uchar> *textureData,
                           QImage::Format textureFormat, const QVector<QRgb> &colorTable);
    virtual ~QCustom3DVolumePrivate();

    void resetDirtyBits();
    QImage renderSlice(Qt::Axis axis, int index);

    QCustom3DVolume *qptr();

public:
    int m_textureWidth;
    int m_textureHeight;
    int m_textureDepth;
    int m_sliceIndexX;
    int m_sliceIndexY;
    int m_sliceIndexZ;

    QImage::Format m_textureFormat;
    QVector<QRgb> m_colorTable;
    QVector<uchar> *m_textureData;

    float m_alphaMultiplier;
    bool m_preserveOpacity;
    bool m_useHighDefShader;

    QCustomVolumeDirtyBitField m_dirtyBitsVolume;

private:
    int multipliedAlphaValue(int alpha);

    friend class QCustom3DVolume;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

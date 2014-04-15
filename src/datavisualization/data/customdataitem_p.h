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

#ifndef CUSTOMDATAITEM_P_H
#define CUSTOMDATAITEM_P_H

#include "datavisualizationglobal_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class TextureHelper;

class QT_DATAVISUALIZATION_EXPORT CustomDataItem
{
public:
    CustomDataItem();
    virtual ~CustomDataItem();

    inline void setMeshFile(const QString &meshFile) { m_meshFile = meshFile; }
    void setTextureImage(const QImage &textureImage);
    inline void setPosition(const QVector3D &position) { m_position = position; }
    inline void setScaling(const QVector3D &scaling) { m_scaling = scaling; }
    inline void setRotation(const QQuaternion &rotation) { m_rotation = rotation; }

private:
    TextureHelper *m_textureHelper;
    GLuint m_texture;
    QString m_meshFile;
    QVector3D m_position;
    QVector3D m_scaling;
    QQuaternion m_rotation;

    friend class Bars3DRenderer;
    friend class Scatter3DRenderer;
    friend class Surface3DRenderer;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

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

#ifndef SERIESRENDERCACHE_P_H
#define SERIESRENDERCACHE_P_H

#include "datavisualizationglobal_p.h"
#include "qabstract3dseries_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Abstract3DRenderer;
class ObjectHelper;
class TextureHelper;

class SeriesRenderCache
{
public:
    SeriesRenderCache();
    virtual ~SeriesRenderCache();

    void populate(QAbstract3DSeries *series, Abstract3DRenderer *renderer);
    virtual void cleanup(TextureHelper *texHelper);

    // NOTE: Series pointer can only be used to access the series when syncing with controller.
    // It is not guaranteed to be valid while rendering and should only be used as an identifier.
    inline QAbstract3DSeries *series() const { return m_series; }

    inline const QString &itemLabelFormat() const { return m_itemLabelFormat; }
    inline const QAbstract3DSeries::Mesh &mesh() const { return m_mesh; }
    inline const QQuaternion &meshRotation() const { return m_meshRotation; }
    inline void setMeshRotation(const QQuaternion &rotation) { m_meshRotation = rotation; }
    inline ObjectHelper *object() const { return m_object; }
    inline const Q3DTheme::ColorStyle &colorStyle() const { return m_colorStyle; }
    inline const QVector3D &baseColor() const { return m_baseColor; }
    inline const GLuint &baseUniformTexture() const { return m_baseUniformTexture; }
    inline const GLuint &baseGradientTexture() const { return m_baseGradientTexture; }
    inline const QVector3D &singleHighlightColor() const { return m_singleHighlightColor; }
    inline const GLuint &singleHighlightGradientTexture() const { return m_singleHighlightGradientTexture; }
    inline const QVector3D &multiHighlightColor() const { return m_multiHighlightColor; }
    inline const GLuint &multiHighlightGradientTexture() const { return m_multiHighlightGradientTexture; }
    inline const QString &name() const { return m_name; }

protected:
    QAbstract3DSeries *m_series;
    QString m_itemLabelFormat;
    ObjectHelper *m_object;
    QAbstract3DSeries::Mesh m_mesh;
    QQuaternion m_meshRotation;

    Q3DTheme::ColorStyle m_colorStyle;
    QVector3D m_baseColor;
    GLuint m_baseUniformTexture;
    GLuint m_baseGradientTexture;
    QVector3D m_singleHighlightColor;
    GLuint m_singleHighlightGradientTexture;
    QVector3D m_multiHighlightColor;
    GLuint m_multiHighlightGradientTexture;

    QString m_name;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif


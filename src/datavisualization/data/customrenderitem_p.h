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

#ifndef CUSTOMRENDERITEM_P_H
#define CUSTOMRENDERITEM_P_H

#include "abstractrenderitem_p.h"
#include "objecthelper_p.h"
#include <QtGui/QRgb>
#include <QtGui/QImage>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QCustom3DItem;
class Abstract3DRenderer;

class CustomRenderItem : public AbstractRenderItem
{
public:
    CustomRenderItem();
    virtual ~CustomRenderItem();

    inline void setTexture(GLuint texture) { m_texture = texture; }
    inline GLuint texture() const { return m_texture; }
    void setMesh(const QString &meshFile);
    inline ObjectHelper *mesh() const { return m_object; }
    inline void setScaling(const QVector3D &scaling) { m_scaling = scaling; }
    inline QVector3D scaling() const { return m_scaling; }
    inline void setPosition(const QVector3D &position) { m_position = position; }
    inline QVector3D position() const { return m_position; }
    inline void setPositionAbsolute(bool absolute) { m_absolute = absolute; }
    inline bool isPositionAbsolute() const { return m_absolute; }
    inline void setBlendNeeded(bool blend) { m_needBlend = blend; }
    inline bool isBlendNeeded() const { return m_needBlend; }
    inline void setVisible(bool visible) { m_visible = visible; }
    inline bool isVisible() const { return m_visible; }
    inline void setItemPointer(QCustom3DItem *item) { m_item = item; }
    inline QCustom3DItem *itemPointer() const { return m_item; }
    inline void setValid(bool valid) { m_valid = valid; }
    inline bool isValid() const { return m_valid; }
    inline void setIndex(int index) { m_index = index; }
    inline int index() const { return m_index; }
    inline void setShadowCasting(bool shadowCasting) { m_shadowCasting = shadowCasting; }
    inline bool isShadowCasting() const { return m_shadowCasting; }
    inline void setFacingCamera(bool facing) { m_isFacingCamera = facing; }
    inline bool isFacingCamera() const { return m_isFacingCamera; }
    inline void setRenderer(Abstract3DRenderer *renderer) { m_renderer = renderer; }

    // Volume specific
    inline void setTextureWidth(int width) { m_textureWidth = width; }
    inline int textureWidth() const { return m_textureWidth; }
    inline void setTextureHeight(int height) { m_textureHeight = height; }
    inline int textureHeight() const { return m_textureHeight; }
    inline void setTextureDepth(int depth) { m_textureDepth = depth; }
    inline int textureDepth() const { return m_textureDepth; }
    inline int textureSize() const { return m_textureWidth * m_textureHeight * m_textureDepth; }
    inline void setColorTable(const QVector<QVector4D> &colors) { m_colorTable = colors; }
    void setColorTable(const QVector<QRgb> &colors);
    inline const QVector<QVector4D> &colorTable() const { return m_colorTable; }
    inline void setVolume(bool volume) { m_isVolume = volume; }
    inline bool isVolume() const { return m_isVolume; }
    inline void setTextureFormat(QImage::Format format) { m_textureFormat = format; }
    inline QImage::Format textureFormat() const { return m_textureFormat; }
    inline void setSliceIndexX(int index) { m_sliceIndexX = index; }
    inline void setSliceIndexY(int index) { m_sliceIndexY = index; }
    inline void setSliceIndexZ(int index) { m_sliceIndexZ = index; }
    inline int sliceIndexX() const { return m_sliceIndexX; }
    inline int sliceIndexY() const { return m_sliceIndexY; }
    inline int sliceIndexZ() const { return m_sliceIndexZ; }
    inline void setAlphaMultiplier(float mult) { m_alphaMultiplier = mult; }
    inline float alphaMultiplier() const { return m_alphaMultiplier; }
    inline void setPreserveOpacity(bool enable) { m_preserveOpacity = enable; }
    inline bool preserveOpacity() const { return m_preserveOpacity; }

private:
    Q_DISABLE_COPY(CustomRenderItem)

    GLuint m_texture;
    QVector3D m_scaling;
    QVector3D m_position;
    bool m_absolute;
    ObjectHelper *m_object; // shared reference
    bool m_needBlend;
    bool m_visible;
    bool m_valid;
    int m_index;
    bool m_shadowCasting;
    bool m_isFacingCamera;
    QCustom3DItem *m_item;
    Abstract3DRenderer *m_renderer;

    // Volume specific
    int m_textureWidth;
    int m_textureHeight;
    int m_textureDepth;
    QVector<QVector4D> m_colorTable;
    bool m_isVolume;
    QImage::Format m_textureFormat;
    int m_sliceIndexX;
    int m_sliceIndexY;
    int m_sliceIndexZ;
    float m_alphaMultiplier;
    bool m_preserveOpacity;
};
typedef QHash<QCustom3DItem *, CustomRenderItem *> CustomRenderItemArray;

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef ABSTRACT3DRENDERER_P_H
#define ABSTRACT3DRENDERER_P_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QFont>

#include "datavis3dglobal_p.h"
#include "abstract3dcontroller_p.h"
#include "axisrendercache_p.h"
#include "qabstractdataproxy.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class TextureHelper;
class Theme;
class Drawer;

class Abstract3DRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    Abstract3DController *m_controller;
    bool m_isInitialized;

protected:
    bool m_hasNegativeValues;
    Theme m_cachedTheme;
    QFont m_cachedFont;
    QDataVis::LabelTransparency m_cachedLabelTransparency;
    Drawer *m_drawer;
    QRect m_cachedBoundingRect;
    QDataVis::ShadowQuality m_cachedShadowQuality;
    GLfloat m_autoScaleAdjustment;
    int m_cachedZoomLevel;

    QString m_cachedItemLabelFormat;
    QString m_cachedObjFile;
    QDataVis::SelectionMode m_cachedSelectionMode;
    bool m_cachedIsGridEnabled;
    bool m_cachedIsBackgroundEnabled;


    AxisRenderCache m_axisCacheX;
    AxisRenderCache m_axisCacheY;
    AxisRenderCache m_axisCacheZ;
    TextureHelper *m_textureHelper;

public:
    ~Abstract3DRenderer();

    inline bool isInitialized() { return m_isInitialized; }
    void updateDataModel(QAbstractDataProxy *dataProxy);

    virtual void render(CameraHelper *camera, const GLuint defaultFboHandle);

    virtual void updateBoundingRect(const QRect boundingRect);
    virtual void updatePosition(const QRect boundingRect);

    virtual void updateZoomLevel(int newZoomLevel);
    virtual void updateTheme(Theme theme);
    virtual void updateFont(const QFont &font);
    virtual void updateLabelTransparency(QDataVis::LabelTransparency transparency);
    virtual void updateSelectionMode(QDataVis::SelectionMode newMode);
    virtual void updateGridEnabled(bool enable);
    virtual void updateBackgroundEnabled(bool enable);
    virtual void updateMeshFileName(const QString &objFileName);

    virtual QString itemLabelFormat() const;
    virtual void requestSelectionAtPoint(const QPoint &point) = 0;
    virtual void updateTextures() = 0;
    virtual void initSelectionBuffer() = 0;
#if !defined(QT_OPENGL_ES_2)
    virtual void updateDepthBuffer() = 0;
#endif
    virtual void updateShadowQuality(QDataVis::ShadowQuality quality) = 0;
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader) = 0;
    virtual void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader) = 0;
    virtual void updateAxisType(QAbstractAxis::AxisOrientation orientation, QAbstractAxis::AxisType type);
    virtual void updateAxisTitle(QAbstractAxis::AxisOrientation orientation, const QString &title);
    virtual void updateAxisLabels(QAbstractAxis::AxisOrientation orientation, const QStringList &labels);
    virtual void updateAxisRange(QAbstractAxis::AxisOrientation orientation, qreal min, qreal max);
    virtual void updateAxisSegmentCount(QAbstractAxis::AxisOrientation orientation, int count);
    virtual void updateAxisSubSegmentCount(QAbstractAxis::AxisOrientation orientation, int count);

protected:
    Abstract3DRenderer(Abstract3DController *controller);

    virtual void initializeOpenGL();

    virtual void handleShadowQualityChange();
    virtual void handleResize();
    virtual void loadMeshFile() = 0;

    AxisRenderCache &axisCacheForOrientation(QAbstractAxis::AxisOrientation orientation);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // ABSTRACT3DRENDERER_P_H

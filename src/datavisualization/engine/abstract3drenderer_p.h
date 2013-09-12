/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

#ifndef ABSTRACT3DRENDERER_P_H
#define ABSTRACT3DRENDERER_P_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QFont>
#include <QTime>

#include "datavisualizationglobal_p.h"
#include "abstract3dcontroller_p.h"
#include "axisrendercache_p.h"
#include "qabstractdataproxy.h"

//#define DISPLAY_RENDER_SPEED

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class TextureHelper;
class Theme;
class Drawer;

class Abstract3DRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    Abstract3DController *m_controller;

protected:
    bool m_hasNegativeValues;
    Theme m_cachedTheme;
    QFont m_cachedFont;
    QDataVis::LabelTransparency m_cachedLabelTransparency;
    Drawer *m_drawer;
    QRect m_cachedBoundingRect;
    QDataVis::ShadowQuality m_cachedShadowQuality;
    GLfloat m_autoScaleAdjustment;

    QString m_cachedItemLabelFormat;
    QString m_cachedObjFile;
    QDataVis::SelectionMode m_cachedSelectionMode;
    bool m_cachedIsGridEnabled;
    bool m_cachedIsBackgroundEnabled;

    AxisRenderCache m_axisCacheX;
    AxisRenderCache m_axisCacheY;
    AxisRenderCache m_axisCacheZ;
    TextureHelper *m_textureHelper;
    Q3DBox m_boundingBox;

    Q3DScene *m_cachedScene;

#ifdef DISPLAY_RENDER_SPEED
    bool m_isFirstFrame;
    QTime m_lastFrameTime;
    GLint m_numFrames;
#endif

    QString generateValueLabel(const QString &format, qreal value);

public:
    virtual ~Abstract3DRenderer();

    void updateDataModel(QAbstractDataProxy *dataProxy);

    virtual void render(GLuint defaultFboHandle);

    virtual void updateBoundingRect(const QRect &boundingRect);
    virtual void updatePosition(const QRect &boundingRect);

    virtual void updateTheme(Theme theme);
    virtual void updateFont(const QFont &font);
    virtual void updateLabelTransparency(QDataVis::LabelTransparency transparency);
    virtual void updateSelectionMode(QDataVis::SelectionMode newMode);
    virtual void updateGridEnabled(bool enable);
    virtual void updateBackgroundEnabled(bool enable);
    virtual void updateMeshFileName(const QString &objFileName);
    virtual void updateScene(Q3DScene *scene);
    virtual QString itemLabelFormat() const;
    virtual void updateTextures() = 0;
    virtual void initSelectionBuffer() = 0;

#if !defined(QT_OPENGL_ES_2)
    virtual void updateDepthBuffer() = 0;
#endif
    virtual void updateShadowQuality(QDataVis::ShadowQuality quality) = 0;
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader) = 0;
    virtual void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader) = 0;
    virtual void updateAxisType(Q3DAbstractAxis::AxisOrientation orientation, Q3DAbstractAxis::AxisType type);
    virtual void updateAxisTitle(Q3DAbstractAxis::AxisOrientation orientation, const QString &title);
    virtual void updateAxisLabels(Q3DAbstractAxis::AxisOrientation orientation, const QStringList &labels);
    virtual void updateAxisRange(Q3DAbstractAxis::AxisOrientation orientation, qreal min, qreal max);
    virtual void updateAxisSegmentCount(Q3DAbstractAxis::AxisOrientation orientation, int count);
    virtual void updateAxisSubSegmentCount(Q3DAbstractAxis::AxisOrientation orientation, int count);
    virtual void updateAxisLabelFormat(Q3DAbstractAxis::AxisOrientation orientation, const QString &format);

signals:
    void needRender(); // Emit this if something in renderer causes need for another render pass.

protected:
    Abstract3DRenderer(Abstract3DController *controller);

    virtual void initializeOpenGL();

    virtual void handleShadowQualityChange();
    virtual void handleResize();
    virtual void loadMeshFile() = 0;

    AxisRenderCache &axisCacheForOrientation(Q3DAbstractAxis::AxisOrientation orientation);
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // ABSTRACT3DRENDERER_P_H

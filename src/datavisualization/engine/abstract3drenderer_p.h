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
#include "seriesrendercache_p.h"

//#define DISPLAY_RENDER_SPEED

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class TextureHelper;
class Theme;
class Drawer;

class Abstract3DRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

protected:
    enum SelectionState {
        SelectNone = 0,
        SelectOnScene,
        SelectOnOverview,
        SelectOnSlice
    };

    QString generateValueLabel(const QString &format, float value);

public:
    virtual ~Abstract3DRenderer();

    virtual void updateData() = 0;
    virtual void updateSeries(const QList<QAbstract3DSeries *> &seriesList, bool updateVisibility);

    virtual void render(GLuint defaultFboHandle);

    virtual void updateTheme(Q3DTheme *theme);
    virtual void updateSelectionMode(QDataVis::SelectionFlags newMode);
    virtual void updateScene(Q3DScene *scene);
    virtual void updateTextures() = 0;
    virtual void initSelectionBuffer() = 0;
    virtual void updateSelectionState(SelectionState state);
    virtual void updateInputPosition(const QPoint &position);

#if !defined(QT_OPENGL_ES_2)
    virtual void updateDepthBuffer() = 0;
#endif
    virtual void updateShadowQuality(QDataVis::ShadowQuality quality) = 0;
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader) = 0;
    virtual void initGradientShaders(const QString &vertexShader, const QString &fragmentShader);
    virtual void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader) = 0;
    virtual void updateAxisType(Q3DAbstractAxis::AxisOrientation orientation, Q3DAbstractAxis::AxisType type);
    virtual void updateAxisTitle(Q3DAbstractAxis::AxisOrientation orientation, const QString &title);
    virtual void updateAxisLabels(Q3DAbstractAxis::AxisOrientation orientation, const QStringList &labels);
    virtual void updateAxisRange(Q3DAbstractAxis::AxisOrientation orientation, float min, float max);
    virtual void updateAxisSegmentCount(Q3DAbstractAxis::AxisOrientation orientation, int count);
    virtual void updateAxisSubSegmentCount(Q3DAbstractAxis::AxisOrientation orientation, int count);
    virtual void updateAxisLabelFormat(Q3DAbstractAxis::AxisOrientation orientation, const QString &format);

    virtual void fixMeshFileName(QString &fileName, QAbstract3DSeries::Mesh mesh);
    void fixGradientAndGenerateTexture(QLinearGradient *gradient, GLuint *gradientTexture);

signals:
    void needRender(); // Emit this if something in renderer causes need for another render pass.
    void requestShadowQuality(QDataVis::ShadowQuality quality); // For automatic quality adjustments

protected:
    Abstract3DRenderer(Abstract3DController *controller);

    virtual void initializeOpenGL();

    void reInitShaders();
    virtual void handleShadowQualityChange();
    virtual void handleResize();

    AxisRenderCache &axisCacheForOrientation(Q3DAbstractAxis::AxisOrientation orientation);

    virtual void lowerShadowQuality();

    void fixGradient(QLinearGradient *gradient, GLuint *gradientTexture);

    bool m_hasNegativeValues;
    Q3DTheme *m_cachedTheme;
    Drawer *m_drawer;
    QRect m_viewport;
    QDataVis::ShadowQuality m_cachedShadowQuality;
    GLfloat m_autoScaleAdjustment;

    QDataVis::SelectionFlags m_cachedSelectionMode;

    AxisRenderCache m_axisCacheX;
    AxisRenderCache m_axisCacheY;
    AxisRenderCache m_axisCacheZ;
    TextureHelper *m_textureHelper;
    Q3DBox m_boundingBox;

    Q3DScene *m_cachedScene;
    bool m_selectionDirty;
    SelectionState m_selectionState;
    QPoint m_inputPosition;
    QVector<SeriesRenderCache> m_visibleSeriesList;
    QRect m_primarySubViewport;
    QRect m_secondarySubViewport;

#ifdef DISPLAY_RENDER_SPEED
    bool m_isFirstFrame;
    QTime m_lastFrameTime;
    GLint m_numFrames;
#endif
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // ABSTRACT3DRENDERER_P_H

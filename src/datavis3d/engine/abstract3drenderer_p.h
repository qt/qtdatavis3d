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

QT_DATAVIS3D_BEGIN_NAMESPACE

class Abstract3DRenderer : public QObject, protected QOpenGLFunctions
{
protected:
    Abstract3DController *m_controller;

    bool m_hasNegativeValues;
    QRect m_cachedBoundingRect;
    QDataVis::ShadowQuality m_cachedShadowQuality;
    Theme m_cachedTheme;
    QFont m_cachedFont;
    QDataVis::LabelTransparency m_cachedLabelTransparency;
    Drawer *m_drawer;
    GLfloat m_autoScaleAdjustment;

    AxisRenderCache m_axisCacheX;
    AxisRenderCache m_axisCacheY;
    AxisRenderCache m_axisCacheZ;

    Abstract3DRenderer(Abstract3DController *controller);
    virtual void initializePreOpenGL();
    virtual void initializeOpenGL();

    virtual void updateBoundingRect(const QRect boundingRect);
    virtual void updatePosition(const QRect boundingRect);
    virtual void handleResize();

    virtual void updateTheme(Theme theme);
    virtual void updateFont(const QFont &font);
    virtual void updateLabelTransparency(QDataVis::LabelTransparency transparency);

    virtual void handleShadowQualityChange();

    virtual void requestSelectionAtPoint(const QPoint &point)=0;
    virtual void updateTextures()=0;
    virtual void initSelectionBuffer()=0;
    virtual void updateDepthBuffer()=0;
    virtual void updateShadowQuality(QDataVis::ShadowQuality quality)=0;
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader)=0;
    virtual void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader)=0;
    virtual void updateAxisType(QAbstractAxis::AxisOrientation orientation, QAbstractAxis::AxisType type);
    virtual void updateAxisTitle(QAbstractAxis::AxisOrientation orientation, const QString &title);
    virtual void updateAxisLabels(QAbstractAxis::AxisOrientation orientation, const QStringList &labels);
    virtual void updateAxisRange(QAbstractAxis::AxisOrientation orientation, qreal min, qreal max);
    virtual void updateAxisSegmentCount(QAbstractAxis::AxisOrientation orientation, int count);
    virtual void updateAxisSubSegmentCount(QAbstractAxis::AxisOrientation orientation, int count);

    void initializeAxisCache(QAbstractAxis::AxisOrientation orientation, const QAbstractAxis *axis);
    AxisRenderCache &axisCacheForOrientation(QAbstractAxis::AxisOrientation orientation);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // ABSTRACT3DRENDERER_P_H

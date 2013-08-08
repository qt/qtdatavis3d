/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
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
    ShadowQuality m_cachedShadowQuality;
    Theme m_cachedTheme;
    QFont m_cachedFont;
    LabelTransparency m_cachedLabelTransparency;
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
    virtual void updateLabelTransparency(LabelTransparency transparency);

    virtual void handleShadowQualityChange();

    virtual void requestSelectionAtPoint(const QPoint &point)=0;
    virtual void updateTextures()=0;
    virtual void initSelectionBuffer()=0;
    virtual void updateDepthBuffer()=0;
    virtual void updateShadowQuality(ShadowQuality quality)=0;
    virtual void initShaders(const QString &vertexShader, const QString &fragmentShader)=0;
    virtual void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader)=0;
    virtual void updateAxisType(QAbstractAxis::AxisOrientation orientation, QAbstractAxis::AxisType type);
    virtual void updateAxisTitle(QAbstractAxis::AxisOrientation orientation, const QString &title);
    virtual void updateAxisLabels(QAbstractAxis::AxisOrientation orientation, const QStringList &labels);
    virtual void updateAxisRange(QAbstractAxis::AxisOrientation orientation, qreal min, qreal max);
    virtual void updateAxisTickCount(QAbstractAxis::AxisOrientation orientation, int count);
    virtual void updateAxisSubTickCount(QAbstractAxis::AxisOrientation orientation, int count);

    void initializeAxisCache(QAbstractAxis::AxisOrientation orientation, const QAbstractAxis *axis);
    AxisRenderCache &axisCacheForOrientation(QAbstractAxis::AxisOrientation orientation);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // ABSTRACT3DRENDERER_P_H

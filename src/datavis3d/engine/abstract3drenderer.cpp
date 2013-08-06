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
#include "abstract3drenderer_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

Abstract3DRenderer::Abstract3DRenderer(Abstract3DController *controller)
    : QObject(controller),
      m_controller(controller),
      m_hasNegativeValues(false),
      m_drawer(new Drawer(m_cachedTheme, m_cachedFont, m_cachedLabelTransparency)),
      m_autoScaleAdjustment(1.0f)
{
}

void Abstract3DRenderer::initializePreOpenGL()
{
    QObject::connect(m_drawer, &Drawer::drawerChanged, this, &Abstract3DRenderer::updateTextures);

    QObject::connect(m_controller, &Abstract3DController::themeChanged, this,
                     &Abstract3DRenderer::updateTheme);
    QObject::connect(m_controller, &Abstract3DController::fontChanged, this,
                     &Abstract3DRenderer::updateFont);
    QObject::connect(m_controller, &Abstract3DController::labelTransparencyUpdated, this,
                     &Abstract3DRenderer::updateLabelTransparency);
    QObject::connect(m_controller, &Abstract3DController::boundingRectChanged, this,
                     &Abstract3DRenderer::updateBoundingRect);
    QObject::connect(m_controller, &Abstract3DController::sizeChanged, this,
                     &Abstract3DRenderer::updateBoundingRect);
    QObject::connect(m_controller, &Abstract3DController::shadowQualityChanged, this,
                     &Abstract3DRenderer::updateShadowQuality);

    updateTheme(m_controller->theme());
    updateFont(m_controller->font());
    updateLabelTransparency(m_controller->labelTransparency());
}

void Abstract3DRenderer::initializeOpenGL()
{
    // OpenGL is initialized, safe to call these.
    updateBoundingRect(m_controller->boundingRect());
    updateShadowQuality(m_controller->shadowQuality());
}

void Abstract3DRenderer::updateBoundingRect(const QRect boundingRect)
{
    m_cachedBoundingRect = boundingRect;
    handleResize();
}

void Abstract3DRenderer::updatePosition(const QRect boundingRect)
{
    m_cachedBoundingRect = boundingRect;
}

void Abstract3DRenderer::updateTheme(Theme theme)
{
    m_cachedTheme.setFromTheme(theme);

    m_drawer->setTheme(m_cachedTheme);
    // Re-initialize shaders
    handleShadowQualityChange();
}

void Abstract3DRenderer::handleShadowQualityChange()
{
#if !defined(QT_OPENGL_ES_2)
    if (m_cachedShadowQuality > ShadowNone) {
        if (!m_cachedTheme.m_uniformColor) {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTexColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertexShadow"),
                        QStringLiteral(":/shaders/fragmentShadowNoTex"));
        }
        initBackgroundShaders(QStringLiteral(":/shaders/vertexShadow"),
                              QStringLiteral(":/shaders/fragmentShadowNoTex"));
    } else {
        if (!m_cachedTheme.m_uniformColor) {
            initShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragmentColorOnY"));
        } else {
            initShaders(QStringLiteral(":/shaders/vertex"),
                        QStringLiteral(":/shaders/fragment"));
        }
        initBackgroundShaders(QStringLiteral(":/shaders/vertex"),
                              QStringLiteral(":/shaders/fragment"));
    }
#else
    if (!m_cachedTheme.m_uniformColor) {
        initShaders(QStringLiteral(":/shaders/vertexES2"),
                    QStringLiteral(":/shaders/fragmentColorOnYES2"));
    } else {
        initShaders(QStringLiteral(":/shaders/vertexES2"),
                    QStringLiteral(":/shaders/fragmentES2"));
    }
    initBackgroundShaders(QStringLiteral(":/shaders/vertexES2"),
                          QStringLiteral(":/shaders/fragmentES2"));
#endif
}

void Abstract3DRenderer::updateFont(const QFont &font)
{
    m_cachedFont = font;
    m_drawer->setFont(font);
}

void Abstract3DRenderer::updateLabelTransparency(LabelTransparency transparency)
{
    m_cachedLabelTransparency = transparency;
    m_drawer->setTransparency(transparency);
}

void Abstract3DRenderer::handleResize()
{
    if (m_cachedBoundingRect.width() == 0 || m_cachedBoundingRect.height() == 0)
        return;
    qDebug() << "Bars3dRenderer::resizeEvent " << m_cachedBoundingRect.width() << "x" <<m_cachedBoundingRect.height();
    // Calculate zoom level based on aspect ratio
    GLfloat div;
    GLfloat zoomAdjustment;
    div = qMin(m_cachedBoundingRect.width(), m_cachedBoundingRect.height());
    zoomAdjustment = defaultRatio * ((m_cachedBoundingRect.width() / div) / (m_cachedBoundingRect.height() / div));
    //qDebug() << "zoom adjustment" << zoomAdjustment;
    m_autoScaleAdjustment = qMin(zoomAdjustment, 1.0f); // clamp to 1.0f

    // Re-init selection buffer
    initSelectionBuffer();

#if !defined(QT_OPENGL_ES_2)
    // Re-init depth buffer
    updateDepthBuffer();
#endif
}

QT_DATAVIS3D_END_NAMESPACE

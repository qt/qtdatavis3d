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

#ifndef DECLARATIVESCATTER_H
#define DECLARATIVESCATTER_H

#include "datavis3dglobal_p.h"
#include "Scatter3Dcontroller_p.h"
#include "declarativescatter_p.h"
#include "qitemmodelscatterdatamapping.h"

#include <qsgsimpletexturenode.h>
#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>

class QOpenGLFramebufferObject;
class QSGTexture;
class QQuickWindow;

QT_DATAVIS3D_BEGIN_NAMESPACE

class DeclarativeScatter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *data READ data WRITE setData)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(float fontSize READ fontSize WRITE setFontSize)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_PROPERTY(QItemModelScatterDataMapping *mapping READ mapping WRITE setMapping)
    Q_ENUMS(SelectionMode)
    Q_ENUMS(ShadowQuality)
    Q_ENUMS(LabelTransparency)


protected:
    Scatter3DController *m_shared;
    DeclarativeScatterCachedStatePrivate *m_cachedState;
    QSize m_initialisedSize;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

public:
    // Duplicated here to be able to use the same enums
    enum SelectionMode {
        ModeNone = 0,
        ModeBar,
        ModeBarAndRow,
        ModeBarAndColumn,
        ModeBarRowAndColumn,
        ModeZoomRow,
        ModeZoomColumn
    };

    enum ShadowQuality {
        ShadowNone = 0,
        ShadowLow = 1,
        ShadowMedium = 3,
        ShadowHigh = 5
    };

    enum LabelTransparency {
        TransparencyNone = 0,       // Full solid, using colors from theme
        TransparencyFromTheme,      // Use colors and transparencies from theme
        TransparencyNoBackground    // Draw just text on transparent background
    };

public:
    explicit DeclarativeScatter(QQuickItem *parent = 0);
    ~DeclarativeScatter();

    void classBegin();
    void componentComplete();

    // TODO: We need to provide another way to pass row and column labels when using addData(QAbstractItemModel *data)

    // TODO: Map qml listitemmodel to qabstractitemmodel
    // Add whole data set.
    void setData(QAbstractItemModel *data);
    QAbstractItemModel *data();

    // dot type
    // TODO: Rename setBarType -> setItemType / setObjectType / setXXXType
    Q_INVOKABLE void setBarType(BarStyle style, bool smooth = false);

    // override bar type with own mesh
    Q_INVOKABLE void setMeshFileName(const QString &objFileName);

    // Select preset camera placement
    Q_INVOKABLE void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) (or (-90...90) if there are negative values) angles and distance in
    // percentage (10...500))
    Q_INVOKABLE void setCameraPosition(qreal horizontal, qreal vertical, int distance);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    Q_INVOKABLE void setTheme(ColorTheme theme);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height)
    // color to change from bottom to top
    Q_INVOKABLE void setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                                 bool uniform = true);

    // Set tick count and step. Note; tickCount * step should be the maximum possible value of data
    // set. Minimum is the absolute minimum possible value a bar can have. This is especially
    // important to set if values can be negative.
    Q_INVOKABLE void setTickCount(int tickCount, qreal step, qreal minimum);

    // TODO: light placement API

    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode();

    // Font size adjustment
    void setFontSize(float fontsize);
    float fontSize();

    // Set font
    void setFont(const QFont &font);
    QFont font();

    // Label transparency adjustment
    void setLabelTransparency(LabelTransparency transparency);
    LabelTransparency labelTransparency();

    // Enable or disable background grid
    void setGridVisible(bool visible);
    bool isGridVisible();

    // Enable or disable background mesh
    void setBackgroundVisible(bool visible);
    bool isBackgroundVisible();

    // Adjust shadow quality
    void setShadowQuality(ShadowQuality quality);
    ShadowQuality shadowQuality();

    // TODO: Create own item model data mapping for scatter
    QItemModelScatterDataMapping *mapping() const;
    void setMapping(QItemModelScatterDataMapping *mapping);

public slots:
    // Used to detect when shadow quality changes autonomously due to e.g. resizing.
    void handleShadowQualityUpdate(QtDataVis3D::ShadowQuality quality);

signals:
    // Signals shadow quality changes.
    void shadowQualityChanged(DeclarativeScatter::ShadowQuality quality);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

};

class DeclarativeScatterRenderer : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    DeclarativeScatterRenderer(QQuickWindow *window, Scatter3DController *shared);
    ~DeclarativeScatterRenderer();

public slots:
    void render();

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture *m_texture;
    QQuickWindow *m_window;
    Scatter3DController *m_scatterRenderer;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

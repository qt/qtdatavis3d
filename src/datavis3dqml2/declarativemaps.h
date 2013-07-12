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

#ifndef DECLARATIVEMAPS_H
#define DECLARATIVEMAPS_H

#include "datavis3dglobal_p.h"
#include "maps3dcontroller_p.h"
#include "declarativemaps_p.h"
#include "qitemmodelmapdatamapping.h"

#include <qsgsimpletexturenode.h>
#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>

class QOpenGLFramebufferObject;
class QSGTexture;
class QQuickWindow;

QT_DATAVIS3D_BEGIN_NAMESPACE

class DeclarativeMaps : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *data READ data WRITE setData)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(float fontSize READ fontSize WRITE setFontSize)
    Q_PROPERTY(QItemModelMapDataMapping *mapping READ mapping WRITE setMapping)
    Q_ENUMS(SelectionMode)
    Q_ENUMS(ShadowQuality)
    Q_ENUMS(LabelTransparency)

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
    explicit DeclarativeMaps(QQuickItem *parent = 0);
    ~DeclarativeMaps();

    void classBegin();
    void componentComplete();

    void setData(QAbstractItemModel *data);
    QAbstractItemModel *data();

    // bar specifications; base thickness in x, y and z, enum to indicate which direction is increased with value
    // TODO: Start using thickness also in adjustment direction; use it as a relative value.
    // For example, in AdjustAll mode setting thickness to (0.1f, 1.0f, 0.5f) would apply value to
    // x at 10%, y at 100% and z at 50%. If a dimension is not included, given thickness states its absolute value.
    Q_INVOKABLE void setBarSpecs(const QVector3D &thickness = QVector3D(1.0f, 1.0f, 1.0f),
                     Q3DMaps::AdjustmentDirection direction = Q3DMaps::AdjustHeight);

    // bar type; bars (=cubes), pyramids, cones, cylinders, balls, etc.
    Q_INVOKABLE void setBarType(BarStyle style, bool smooth = false);

    // override bar type with own mesh
    Q_INVOKABLE void setMeshFileName(const QString &objFileName);

    // Select preset camera placement
    Q_INVOKABLE void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) angles and distance in percentage (10...500))
    Q_INVOKABLE void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text colors are affected)
    Q_INVOKABLE void setTheme(ColorTheme theme);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height) color to change from bottom to top
    Q_INVOKABLE void setBarColor(QColor baseColor, QColor heightColor, bool uniform = true);

    // Set area specs
    Q_INVOKABLE void setAreaSpecs(const QRect &areaRect, const QImage &image);

    // Set area image
    Q_INVOKABLE void setImage(const QImage &image);
    Q_INVOKABLE void setImage(const QString &imageUrl);

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

    // Adjust shadow quality
    void setShadowQuality(ShadowQuality quality);
    ShadowQuality shadowQuality();

    QItemModelMapDataMapping *mapping() const;
    void setMapping(QItemModelMapDataMapping *mapping);

protected:
    Maps3DController *m_shared;
    DeclarativeMapsCachedStatePrivate *m_cachedState;
    QSize m_initializedSize;

    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

// TODO: If we use texture node, our rendering is done into a texture that is then drawn to the
// qquickwindow -> selection will not work
class DeclarativeMapsRenderer : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    DeclarativeMapsRenderer(QQuickWindow *window, Maps3DController *shared);
    ~DeclarativeMapsRenderer();

public slots:
    void render();

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture *m_texture;
    QQuickWindow *m_window;
    Maps3DController *m_mapsRenderer;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

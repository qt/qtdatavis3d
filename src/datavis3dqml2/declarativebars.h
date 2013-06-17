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

#ifndef DECLARATIVEBARS_H
#define DECLARATIVEBARS_H

#include "bars3dcontroller_p.h"
#include "qdatavis3dglobal.h"
#include "qdatavis3namespace.h"
#include "declarativebars_p.h"

#include <qsgsimpletexturenode.h>
#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>

class QOpenGLFramebufferObject;
class QSGTexture;
class QQuickWindow;

QT_DATAVIS3D_BEGIN_NAMESPACE

class DeclarativeBars : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *data READ data WRITE setData)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    //Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(float fontSize READ fontSize WRITE setFontSize)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(int height READ height WRITE setHeight)
    Q_ENUMS(SelectionMode)
    Q_ENUMS(ShadowQuality)
    Q_ENUMS(LabelTransparency)

protected:
    Bars3dController *m_shared;
    DeclarativeBarsCachedStatePrivate *m_cachedState;
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
    explicit DeclarativeBars(QQuickItem *parent = 0);
    ~DeclarativeBars();

    void classBegin();
    void componentComplete();

    // TODO: We need to provide another way to pass row and column labels when using addData(QAbstractItemModel *data)

    // TODO: Map qml listitemmodel to qabstractitemmodel
    // Add whole data set.
    void setData(QAbstractItemModel *data);
    QAbstractItemModel *data();

    // TODO: Are we going to offer only addData(QAbstractItemModel *data) for QML2?
    // If so, all these adds can be removed, as well as metatypes for dataset and datarow

    // Add a row of data. Each new row is added to the front of the sample space, moving previous
    // rows back (if sample space is more than one row deep)
    Q_INVOKABLE void addDataRow(const QVector<GLfloat> &dataRow,
                                const QString &labelRow = QString(),
                                const QVector<QString> &labelsColumn = QVector<QString>());
    // ownership of dataItems is transferred
    Q_INVOKABLE void addDataRow(const QVector<QDataItem*> &dataRow,
                                const QString &labelRow = QString(),
                                const QVector<QString> &labelsColumn = QVector<QString>());
    // ownership of dataRow is transferred
    Q_INVOKABLE void addDataRow(QDataRow *dataRow);

    // Add complete data set at a time, as a vector of data rows
    Q_INVOKABLE void addDataSet(const QVector< QVector<GLfloat> > &data,
                                const QVector<QString> &labelsRow = QVector<QString>(),
                                const QVector<QString> &labelsColumn = QVector<QString>());

    // ownership of dataItems is transferred
    Q_INVOKABLE void addDataSet(const QVector< QVector<QDataItem*> > &data,
                                const QVector<QString> &labelsRow = QVector<QString>(),
                                const QVector<QString> &labelsColumn = QVector<QString>());
    // ownership of dataSet is transferred
    Q_INVOKABLE void addDataSet(QDataSet* dataSet);

    // bar thickness, spacing between bars, and is spacing relative to thickness or absolute
    // y -component sets the thickness/spacing of z -direction
    // With relative 0.0f means side-to-side, 1.0f = one thickness in between
    Q_INVOKABLE void setBarSpecs(QSizeF thickness = QSizeF(1.0f, 1.0f),
                                 QSizeF spacing = QSizeF(1.0f, 1.0f),
                                 bool relative = true);

    // bar type; bars (=cubes), pyramids, cones, cylinders, etc.
    Q_INVOKABLE void setBarType(BarStyle style, bool smooth = false);

    // override bar type with own mesh
    Q_INVOKABLE void setMeshFileName(const QString &objFileName);

    // how many samples per row and column, and names for axes
    Q_INVOKABLE void setupSampleSpace(int samplesRow, int samplesColumn,
                                      const QString &labelRow = QString(),
                                      const QString &labelColumn = QString(),
                                      const QString &labelHeight = QString());

    // Select preset camera placement
    Q_INVOKABLE void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) (or (-90...90) if there are negative values) angles and distance in
    // percentage (10...500))
    Q_INVOKABLE void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

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
    Q_INVOKABLE void setTickCount(GLint tickCount, GLfloat step, GLfloat minimum = 0.0f);

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

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

};

// TODO: If we use texture node, our rendering is done into a texture that is then drawn to the
// qquickwindow -> selection will not work
class DeclarativeBarsRenderer : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    DeclarativeBarsRenderer(QQuickWindow *window, Bars3dController *shared);
    ~DeclarativeBarsRenderer();

public slots:
    void render();

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture *m_texture;
    QQuickWindow *m_window;
    Bars3dController *m_barsRenderer;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

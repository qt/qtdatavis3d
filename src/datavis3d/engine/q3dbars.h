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

#ifndef Q3DBARS_H
#define Q3DBARS_H

#include "QtDataVis3D/qdatavis3dglobal.h"
#include "QtDataVis3D/qdatavis3namespace.h"
#include "q3dwindow.h"

#include <QFont>

QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE

class Q3DBarsPrivate;
class QDataItem;
class QDataRow;
class QDataSet;
class LabelItem;

class QTENTERPRISE_DATAVIS3D_EXPORT Q3DBars : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(float fontSize READ fontSize WRITE setFontSize)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)

public:
    explicit Q3DBars();
    ~Q3DBars();

    // Add a row of data. Each new row is added to the front of the sample space, moving previous
    // rows back (if sample space is more than one row deep)
    void addDataRow(const QVector<float> &dataRow,
                    const QString &labelRow = QString(),
                    const QVector<QString> &labelsColumn = QVector<QString>());
    // ownership of dataItems is transferred
    void addDataRow(const QVector<QDataItem*> &dataRow,
                    const QString &labelRow = QString(),
                    const QVector<QString> &labelsColumn = QVector<QString>());
    // ownership of dataRow is transferred
    void addDataRow(QDataRow *dataRow);

    // Add complete data set at a time, as a vector of data rows
    void addDataSet(const QVector< QVector<float> > &data,
                    const QVector<QString> &labelsRow = QVector<QString>(),
                    const QVector<QString> &labelsColumn = QVector<QString>());

    // ownership of dataItems is transferred
    void addDataSet(const QVector< QVector<QDataItem*> > &data,
                    const QVector<QString> &labelsRow = QVector<QString>(),
                    const QVector<QString> &labelsColumn = QVector<QString>());
    // ownership of dataSet is transferred
    void addDataSet(QDataSet* dataSet);

    // bar thickness, spacing between bars, and is spacing relative to thickness or absolute
    // y -component sets the thickness/spacing of z -direction
    // With relative 0.0f means side-to-side, 1.0f = one thickness in between
    void setBarSpecs(QSizeF thickness = QSizeF(1.0f, 1.0f),
                     QSizeF spacing = QSizeF(1.0f, 1.0f),
                     bool relative = true);

    // bar type; bars (=cubes), pyramids, cones, cylinders, etc.
    void setBarType(BarStyle style, bool smooth = false);

    // how many samples per row and column, and names for axes
    void setupSampleSpace(int samplesRow, int samplesColumn,
                          const QString &labelRow = QString(),
                          const QString &labelColumn = QString(),
                          const QString &labelHeight = QString());

    // Select preset camera placement
    void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) (or (-90...90) if there are negative values) angles and distance in
    // percentage (10...500))
    void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    void setTheme(ColorTheme theme);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height)
    // color to change from bottom to top
    void setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                     bool uniform = true);

    // Set tick count and step. Note; tickCount * step should be the maximum possible value of data
    // set. Minimum is the absolute minimum possible value a bar can have. This is especially
    // important to set if values can be negative.
    void setTickCount(GLint tickCount, GLfloat step, GLfloat minimum = 0.0f);

    // override bar type with own mesh
    void setMeshFileName(const QString &objFileName);
    // TODO: light placement API

    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode();

    // Set window title
    void setWindowTitle(const QString &title);
    QString windowTitle();

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

    // TODO: Do these need to be public? Where are they called from?
    // Size
    void setWidth(const int width);
    void setHeight(const int height);

    // Enable or disable background mesh
    void setBackgroundVisible(bool visible);
    bool isBackgroundVisible();

    // Adjust shadow quality
    void setShadowQuality(ShadowQuality quality);
    ShadowQuality shadowQuality();

protected:
    void initialize();
    void render();

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawZoomScene();
    void drawScene();
    QScopedPointer<Q3DBarsPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DBars)
};

QTENTERPRISE_DATAVIS3D_END_NAMESPACE

#endif

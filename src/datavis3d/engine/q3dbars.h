/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
#include "q3dwindow.h"

class QOpenGLShaderProgram;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class Q3DBarsPrivate;
class QDataItem;

class QTCOMMERCIALDATAVIS3D_EXPORT Q3DBars : public Q3DWindow
{
    Q_OBJECT
public:
    enum BarStyle {
        Bars = 0,
        Pyramids,
        Cones,
        Cylinders
    };

    enum CameraPreset {
        PresetFrontLow = 0,
        PresetFront,
        PresetFrontHigh,
        PresetLeftLow,
        PresetLeft,
        PresetLeftHigh,
        PresetRightLow,
        PresetRight,
        PresetRightHigh,
        PresetBehindLow,
        PresetBehind,
        PresetBehindHigh,
        PresetIsometricLeft,
        PresetIsometricLeftHigh,
        PresetIsometricRight,
        PresetIsometricRightHigh,
        PresetDirectlyAbove,
        PresetDirectlyAboveCW45,
        PresetDirectlyAboveCCW45
    };

    enum ColorTheme {
        ThemeSystem = 0,
        ThemeBlueCerulean,
        ThemeBlueIcy,
        ThemeBlueNcs,
        ThemeBrownSand,
        ThemeDark,
        ThemeHighContrast,
        ThemeLight
    };

    enum SelectionMode {
        None = 0,
        Bar,
        BarAndRow,
        BarAndColumn,
        BarRowAndColumn
    };

public:
    explicit Q3DBars();
    ~Q3DBars();

    void initialize();
    void render();
    void render(QPainter *painter);

    // Add a row of data. Each new row is added to the front of the sample space, moving previous
    // rows back (if sample space is more than one row deep)
    // TODO: Replace QVector<..> with a data row class (QDataRow)? Move labels to class.
    void addDataRow(const QVector<float> &dataRow
                    , const QString &labelRow = QString()
                    , const QVector<QString> &labelsColumn = QVector<QString>());
    // TODO: Replace QVector<..> with a data row class (QDataRow)? Move labels to class.
    void addDataRow(const QVector<QDataItem*> &dataRow
                    , const QString &labelRow = QString()
                    , const QVector<QString> &labelsColumn = QVector<QString>());

    // Add complete data set at a time, as a vector of data rows
    // TODO: Replace QVector<QVector<..>> with a data set class (QDataSet)? Move labels to class.
    void addDataSet(const QVector< QVector<float> > &data
                    , const QVector<QString> &labelsRow = QVector<QString>()
                    , const QVector<QString> &labelsColumn = QVector<QString>());

    // TODO: Replace QVector<QVector<..>> with a data set class (QDataSet)? Move labels to class.
    void addDataSet(const QVector< QVector<QDataItem*> > &data
                    , const QVector<QString> &labelsRow = QVector<QString>()
                    , const QVector<QString> &labelsColumn = QVector<QString>());

    // bar thickness, spacing between bars, and is spacing relative to thickness or absolute
    // y -component sets the thickness/spacing of z -direction
    // With relative 0.0f means side-to-side, 1.0f = one thickness in between
    void setBarSpecs(QPointF thickness = QPointF(1.0f, 1.0f)
            , QPointF spacing = QPointF(1.0f, 1.0f)
            , bool relative = true);

    // bar type; bars (=cubes), pyramids, cones, cylinders, etc.
    void setBarType(BarStyle style, bool smooth = false);

    // override bar type with own mesh
    void setMeshFileName(const QString &objFileName);

    // how many samples per row and column, and names for axes
    void setupSampleSpace(QPoint sampleCount, const QString &labelRow = QString()
            , const QString &labelColumn = QString(), const QString &labelHeight = QString());

    // Select preset camera placement
    void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) angles and distance in percentage (10...500))
    void setCameraPosition(float horizontal, float vertical, int distance = 100);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text colors are affected)
    void setTheme(ColorTheme theme);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height) color to change from bottom to top
    void setBarColor(QColor baseColor, QColor heightColor, QColor depthColor, bool uniform = true);

    // TODO: valon siirto / asetus
    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(SelectionMode mode);

    // Set window title
    void setWindowTitle(const QString &title);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawScene();
    QScopedPointer<Q3DBarsPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DBars)
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif

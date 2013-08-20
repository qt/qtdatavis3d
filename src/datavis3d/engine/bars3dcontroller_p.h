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

#ifndef Q3DBARSCONTROLLER_p_H
#define Q3DBARSCONTROLLER_p_H

#include "datavis3dglobal_p.h"
#include "abstract3dcontroller_p.h"

//#define DISPLAY_RENDER_SPEED

QT_DATAVIS3D_BEGIN_NAMESPACE

class Bars3dRenderer;
class QBarDataProxy;

struct Bars3DChangeBitField {
    bool slicingActiveChanged     : 1;
    bool sampleSpaceChanged       : 1;
    bool barSpecsChanged          : 1;

    Bars3DChangeBitField() :
        slicingActiveChanged(true),
        sampleSpaceChanged(true),
        barSpecsChanged(true)
    {
    }
};

class QT_DATAVIS3D_EXPORT Bars3dController : public Abstract3DController
{
    Q_OBJECT

private:


    Bars3DChangeBitField m_changeTracker;

    // Data
    int m_rowCount;
    int m_columnCount;

    // Interaction
    MouseState m_mouseState;
    QPoint m_mousePos;
    bool m_isSlicingActivated;

    // Look'n'feel
    bool m_isBarSpecRelative;
    GLfloat m_barThicknessRatio;
    QSizeF m_barSpacing;

    // Rendering
    Bars3dRenderer *m_renderer;
    QBarDataProxy *m_data;

public:
    explicit Bars3dController(QRect rect);
    ~Bars3dController();

    void initializeOpenGL();
    virtual void synchDataToRenderer();

    int columnCount();
    int rowCount();

    MouseState mouseState();
    QPoint mousePosition();

    bool isSlicingActive();
    void setSlicingActive(bool isSlicing);

    QMatrix4x4 calculateViewMatrix(int zoom, int viewPortWidth, int viewPortHeight, bool showUnder = false);

    // bar thickness, spacing between bars, and is spacing relative to thickness or absolute
    // y -component sets the thickness/spacing of z -direction
    // With relative 0.0f means side-to-side, 1.0f = one thickness in between
    void setBarSpecs(GLfloat thicknessRatio = 1.0f,
                     QSizeF spacing = QSizeF(1.0, 1.0),
                     bool relative = true);
    GLfloat barThickness();
    QSizeF barSpacing();
    bool isBarSpecRelative();

    // bar type; bars (=cubes), pyramids, cones, cylinders, etc.
    void setBarType(QDataVis::MeshStyle style, bool smooth = false);

    // how many samples per row and column, and names for axes
    void setDataWindow(int samplesRow, int samplesColumn);

    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(QDataVis::SelectionMode mode);

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    void wheelEvent(QWheelEvent *event);

    // Sets the data proxy. Assumes ownership of the data proxy. Deletes old proxy.
    void setDataProxy(QBarDataProxy *proxy);
    QBarDataProxy *dataProxy();
    virtual void handleAxisAutoAdjustRangeChangedInOrientation(QAbstractAxis::AxisOrientation orientation, bool autoAdjust);

public slots:
    void handleArrayReset();
    void handleRowsAdded(int startIndex, int count);
    void handleRowsChanged(int startIndex, int count);
    void handleRowsRemoved(int startIndex, int count);
    void handleRowsInserted(int startIndex, int count);
    void handleItemChanged(int rowIndex, int columnIndex);


signals:
    void slicingActiveChanged(bool isSlicing);
    void sampleSpaceChanged(int samplesRow, int samplesColumn);
    void barSpecsChanged(GLfloat thicknessRatio, QSizeF spacing, bool relative);

private:
    void adjustValueAxisRange();

    Q_DISABLE_COPY(Bars3dController)

};


QT_DATAVIS3D_END_NAMESPACE

#endif

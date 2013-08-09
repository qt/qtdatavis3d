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

#ifndef Q3DSCATTERCONTROLLER_p_H
#define Q3DSCATTERCONTROLLER_p_H

#include <QtCore/QSize>
#include <QtCore/QObject>
#include <QWindow>

#include "abstract3dcontroller_p.h"
#include "datavis3dglobal_p.h"

//#define DISPLAY_RENDER_SPEED

class QFont;
class QPoint;
class QSizeF;

QT_DATAVIS3D_BEGIN_NAMESPACE

class Scatter3DRenderer;
class LabelItem;
class QScatterDataProxy;

struct Scatter3DChangeBitField {
    bool selectionModeChanged     : 1;
    bool slicingActiveChanged     : 1;
    bool objFileChanged           : 1;
    bool gridEnabledChanged       : 1;
    bool backgroundEnabledChanged : 1;
    bool zoomLevelChanged         : 1;

    Scatter3DChangeBitField() :
        selectionModeChanged(true),
        slicingActiveChanged(true),
        objFileChanged(true),
        gridEnabledChanged(true),
        backgroundEnabledChanged(true),
        zoomLevelChanged(true)
    {
    }
};

class QT_DATAVIS3D_EXPORT Scatter3DController : public Abstract3DController
{
    Q_OBJECT

public:
    enum SelectionType {
        SelectionNone = 0,
        SelectionBar,
        SelectionRow,
        SelectionColumn
    };

private:
    Scatter3DChangeBitField m_changeTracker;

    // Interaction
    MouseState m_mouseState;
    QPoint m_mousePos;
    QDataVis::SelectionMode m_selectionMode;
    bool m_isSlicingActivated;

    // Look'n'Feel
    QString m_objFile;
    bool m_isGridEnabled;
    bool m_isBackgroundEnabled;

    Scatter3DRenderer *m_renderer;
    QScatterDataProxy *m_data;

public:
    explicit Scatter3DController(QRect rect);
    ~Scatter3DController();

    void initializeOpenGL();

    MouseState mouseState();
    QPoint mousePosition();

    bool isSlicingActive();
    void setSlicingActive(bool isSlicing);

    QMatrix4x4 calculateViewMatrix(int zoom, int viewPortWidth, int viewPortHeight,
                                   bool showUnder = false);

    // bar type; bars (=cubes), pyramids, cones, cylinders, etc.
    void setObjectType(QDataVis::MeshStyle style, bool smooth = false);
    QString objFile();

    // override bar type with own mesh
    void setMeshFileName(const QString &objFileName);

    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(QDataVis::SelectionMode mode);
    QDataVis::SelectionMode selectionMode();

    // Enable or disable background grid
    void setGridEnabled(bool enable);
    bool gridEnabled();

    // Enable or disable background mesh
    void setBackgroundEnabled(bool enable);
    bool backgroundEnabled();

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    void wheelEvent(QWheelEvent *event);

    // Sets the data proxy. Assumes ownership of the data proxy. Deletes old proxy.
    void setDataProxy(QScatterDataProxy *proxy);
    QScatterDataProxy *dataProxy();

    void synchDataToRenderer();

    virtual void handleAxisAutoAdjustRangeChangedInOrientation(QAbstractAxis::AxisOrientation orientation, bool autoAdjust);

public slots:
    void handleArrayReset();
    void handleItemsAdded(int startIndex, int count);
    void handleItemsChanged(int startIndex, int count);
    void handleItemsRemoved(int startIndex, int count);
    void handleItemsInserted(int startIndex, int count);


signals:
    void selectionModeChanged(QDataVis::SelectionMode mode);
    void slicingActiveChanged(bool isSlicing);
    void objFileChanged(QString fileName);
    void fontChanged(QFont font);
    void gridEnabledChanged(bool enable);
    void backgroundEnabledChanged(bool enable);

private:
    void adjustValueAxisRange();

    Q_DISABLE_COPY(Scatter3DController)
};


QT_DATAVIS3D_END_NAMESPACE

#endif

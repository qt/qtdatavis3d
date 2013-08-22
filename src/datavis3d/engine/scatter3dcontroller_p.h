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

#include "datavis3dglobal_p.h"
#include "abstract3dcontroller_p.h"

//#define DISPLAY_RENDER_SPEED

QT_DATAVIS3D_BEGIN_NAMESPACE

class Scatter3DRenderer;
class QScatterDataProxy;

struct Scatter3DChangeBitField {
    bool slicingActiveChanged     : 1;
    bool selectedItemIndexChanged : 1;

    Scatter3DChangeBitField() :
        slicingActiveChanged(true),
        selectedItemIndexChanged(true)
    {
    }
};

class QT_DATAVIS3D_EXPORT Scatter3DController : public Abstract3DController
{
    Q_OBJECT

private:
    Scatter3DChangeBitField m_changeTracker;

    // Interaction
    MouseState m_mouseState;
    QPoint m_mousePos;
    bool m_isSlicingActivated;

    // Rendering
    Scatter3DRenderer *m_renderer;
    QScatterDataProxy *m_data;
    int m_selectedItemIndex;

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

    // Object type
    void setObjectType(QDataVis::MeshStyle style, bool smooth = false);

    // Change selection mode
    void setSelectionMode(QDataVis::SelectionMode mode);

    void setSelectedItemIndex(int index);
    int selectedItemIndex() const;
    static inline int noSelectionIndex() { return -1; }

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
    void handleSelectedItemIndexChanged(int index);

signals:
    void slicingActiveChanged(bool isSlicing);
    void selectedItemIndexChanged(int index);

private:
    void adjustValueAxisRange();

    Q_DISABLE_COPY(Scatter3DController)
};


QT_DATAVIS3D_END_NAMESPACE

#endif

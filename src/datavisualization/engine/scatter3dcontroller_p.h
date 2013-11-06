/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
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
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q3DSCATTERCONTROLLER_p_H
#define Q3DSCATTERCONTROLLER_p_H

#include "datavisualizationglobal_p.h"
#include "abstract3dcontroller_p.h"

//#define DISPLAY_RENDER_SPEED

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Scatter3DRenderer;
class QScatterDataProxy;

struct Scatter3DChangeBitField {
    bool selectedItemIndexChanged : 1;

    Scatter3DChangeBitField() :
        selectedItemIndexChanged(true)
    {
    }
};

class QT_DATAVISUALIZATION_EXPORT Scatter3DController : public Abstract3DController
{
    Q_OBJECT

private:
    Scatter3DChangeBitField m_changeTracker;

    // Rendering
    Scatter3DRenderer *m_renderer;
    int m_selectedItemIndex;

public:
    explicit Scatter3DController(QRect rect);
    ~Scatter3DController();

    void initializeOpenGL();

    // Object type
    void setObjectType(QDataVis::MeshStyle style, bool smooth = false);

    // Change selection mode
    void setSelectionMode(QDataVis::SelectionFlags mode);

    void setSelectedItemIndex(int index);
    int selectedItemIndex() const;
    static inline int noSelectionIndex() { return -1; }

    virtual void setActiveDataProxy(QAbstractDataProxy *proxy);

    void synchDataToRenderer();

    virtual void handleAxisAutoAdjustRangeChangedInOrientation(Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust);
    virtual void handleAxisRangeChangedBySender(QObject *sender);

public slots:
    void handleArrayReset();
    void handleItemsAdded(int startIndex, int count);
    void handleItemsChanged(int startIndex, int count);
    void handleItemsRemoved(int startIndex, int count);
    void handleItemsInserted(int startIndex, int count);

    // Renderer callback handlers
    void handleItemClicked(int index);

signals:
    void selectedItemIndexChanged(int index);

private:
    void adjustValueAxisRange();

    Q_DISABLE_COPY(Scatter3DController)
};


QT_DATAVISUALIZATION_END_NAMESPACE

#endif

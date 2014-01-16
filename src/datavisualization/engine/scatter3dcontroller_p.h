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

namespace QtDataVisualization {

class Scatter3DRenderer;
class QScatterDataProxy;
class QScatter3DSeries;

struct Scatter3DChangeBitField {
    bool selectedItemChanged : 1;

    Scatter3DChangeBitField() :
        selectedItemChanged(true)
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
    int m_selectedItem;
    QScatter3DSeries *m_selectedItemSeries; // Points to the series for which the bar is selected
                                            // in single series selection cases.

public:
    explicit Scatter3DController(QRect rect, Q3DScene *scene = 0);
    ~Scatter3DController();

    virtual void initializeOpenGL();

    // Change selection mode
    void setSelectionMode(QAbstract3DGraph::SelectionFlags mode);

    void setSelectedItem(int index, QScatter3DSeries *series);
    static inline int invalidSelectionIndex() { return -1; }
    virtual void clearSelection();

    void synchDataToRenderer();

    virtual void addSeries(QAbstract3DSeries *series);
    virtual void removeSeries(QAbstract3DSeries *series);
    virtual QList<QScatter3DSeries *> scatterSeriesList();

    virtual void handleAxisAutoAdjustRangeChangedInOrientation(QAbstract3DAxis::AxisOrientation orientation, bool autoAdjust);
    virtual void handleAxisRangeChangedBySender(QObject *sender);
    virtual void handleSeriesVisibilityChangedBySender(QObject *sender);
    virtual void handlePendingClick();

public slots:
    void handleArrayReset();
    void handleItemsAdded(int startIndex, int count);
    void handleItemsChanged(int startIndex, int count);
    void handleItemsRemoved(int startIndex, int count);
    void handleItemsInserted(int startIndex, int count);

private:
    void adjustValueAxisRange();

    Q_DISABLE_COPY(Scatter3DController)
};


}

#endif

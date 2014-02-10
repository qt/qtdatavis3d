/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#ifndef SURFACE3DCONTROLLER_P_H
#define SURFACE3DCONTROLLER_P_H

#include "abstract3dcontroller_p.h"
#include "datavisualizationglobal_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Surface3DRenderer;
class QSurface3DSeries;

struct Surface3DChangeBitField {
    bool smoothStatusChanged      : 1;
    bool surfaceGridChanged       : 1;
    bool selectedPointChanged     : 1;
    bool rowsChanged              : 1;
    bool itemChanged              : 1;

    Surface3DChangeBitField() :
        smoothStatusChanged(true),
        surfaceGridChanged(true),
        selectedPointChanged(true),
        rowsChanged(false),
        itemChanged(false)
    {
    }
};

class QT_DATAVISUALIZATION_EXPORT Surface3DController : public Abstract3DController
{
    Q_OBJECT

private:
    Surface3DChangeBitField m_changeTracker;
    Surface3DRenderer *m_renderer;
    QPoint m_selectedPoint;
    QSurface3DSeries *m_selectedSeries; // Points to the series for which the point is selected in
                                        // single series selection cases.
    bool m_flatShadingSupported;
    QVector<QPoint> m_changedItems;
    QVector<int> m_changedRows;
    QVector<QSurface3DSeries *> m_changedSeriesList;

public:
    explicit Surface3DController(QRect rect, Q3DScene *scene = 0);
    ~Surface3DController();

    virtual void initializeOpenGL();
    virtual void synchDataToRenderer();

    void setSelectionMode(QAbstract3DGraph::SelectionFlags mode);
    void setSelectedPoint(const QPoint &position, QSurface3DSeries *series, bool enterSlice);
    virtual void clearSelection();

    inline QSurface3DSeries *selectedSeries() const { return m_selectedSeries; }

    virtual void handleAxisAutoAdjustRangeChangedInOrientation(
            QAbstract3DAxis::AxisOrientation orientation, bool autoAdjust);
    virtual void handleAxisRangeChangedBySender(QObject *sender);
    virtual void handleSeriesVisibilityChangedBySender(QObject *sender);
    virtual void handlePendingClick();

    static QPoint invalidSelectionPosition();
    bool isFlatShadingSupported();

    virtual void addSeries(QAbstract3DSeries *series);
    virtual void removeSeries(QAbstract3DSeries *series);
    virtual QList<QSurface3DSeries *> surfaceSeriesList();

public slots:
    void handleArrayReset();
    void handleRowsAdded(int startIndex, int count);
    void handleRowsChanged(int startIndex, int count);
    void handleRowsRemoved(int startIndex, int count);
    void handleRowsInserted(int startIndex, int count);
    void handleItemChanged(int rowIndex, int columnIndex);

    void handleFlatShadingSupportedChange(bool supported);

signals:
    void selectedSeriesChanged(QSurface3DSeries *series);

private:
    void adjustValueAxisRange();

    Q_DISABLE_COPY(Surface3DController)
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

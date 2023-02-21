// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtGraphs API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SURFACE3DCONTROLLER_P_H
#define SURFACE3DCONTROLLER_P_H

#include <private/abstract3dcontroller_p.h>
#include <private/graphsglobal_p.h>

QT_BEGIN_NAMESPACE

class Surface3DRenderer;
class QSurface3DSeries;

struct Surface3DChangeBitField {
    bool selectedPointChanged      : 1;
    bool rowsChanged               : 1;
    bool itemChanged               : 1;
    bool flipHorizontalGridChanged : 1;
    bool surfaceTextureChanged     : 1;

    Surface3DChangeBitField() :
        selectedPointChanged(true),
        rowsChanged(false),
        itemChanged(false),
        flipHorizontalGridChanged(true),
        surfaceTextureChanged(true)
    {
    }
};

class Q_GRAPHS_EXPORT Surface3DController : public Abstract3DController
{
    Q_OBJECT

public:
    struct ChangeItem {
        QSurface3DSeries *series;
        QPoint point;
    };
    struct ChangeRow {
        QSurface3DSeries *series;
        int row;
    };
    enum DataDimension {
        BothAscending = 0,
        XDescending = 1,
        ZDescending = 2,
        BothDescending = XDescending | ZDescending
    };
    Q_DECLARE_FLAGS(DataDimensions, DataDimension)

    explicit Surface3DController(QRect rect, Q3DScene *scene = 0);
    ~Surface3DController();

    void initializeOpenGL() override;
    void synchDataToRenderer() override;

    void setSelectionMode(QAbstract3DGraph::SelectionFlags mode) override;
    void setSelectedPoint(const QPoint &position, QSurface3DSeries *series, bool enterSlice);
    void clearSelection() override;

    inline QSurface3DSeries *selectedSeries() const { return m_selectedSeries; }

    void handleAxisAutoAdjustRangeChangedInOrientation(
            QAbstract3DAxis::AxisOrientation orientation, bool autoAdjust) override;
    void handleAxisRangeChangedBySender(QObject *sender) override;
    void handleSeriesVisibilityChangedBySender(QObject *sender) override;
    void handlePendingClick() override;
    void adjustAxisRanges() override;

    static QPoint invalidSelectionPosition();
    bool isFlatShadingSupported();

    void addSeries(QAbstract3DSeries *series) override;
    void removeSeries(QAbstract3DSeries *series) override;
    virtual QList<QSurface3DSeries *> surfaceSeriesList();

    void setFlipHorizontalGrid(bool flip);
    bool flipHorizontalGrid() const;

    void updateSurfaceTexture(QSurface3DSeries *series);

    void setDataDimensions(DataDimensions dimension) { m_dataDimensions = dimension; }
    DataDimensions dataDimensions() { return m_dataDimensions; }

    bool hasChangedSeriesList() { return !m_changedSeriesList.isEmpty(); }
    bool isSeriesVisibilityDirty() { return m_isSeriesVisualsDirty; }

    bool isDataDirty() { return m_isDataDirty; }

    QList<QAbstract3DSeries *> changedSeriesList() { return m_changedSeriesList; }

    bool isSelectedPointChanged() const { return m_changeTracker.selectedPointChanged; }
    void setSelectedPointChanged(bool changed) { m_changeTracker.selectedPointChanged = changed; }

public Q_SLOTS:
    void handleArrayReset();
    void handleRowsAdded(int startIndex, int count);
    void handleRowsChanged(int startIndex, int count);
    void handleRowsRemoved(int startIndex, int count);
    void handleRowsInserted(int startIndex, int count);
    void handleItemChanged(int rowIndex, int columnIndex);

    void handleFlatShadingSupportedChange(bool supported);

Q_SIGNALS:
    void selectedSeriesChanged(QSurface3DSeries *series);
    void flipHorizontalGridChanged(bool flip);

private:
    Surface3DChangeBitField m_changeTracker;
    Surface3DRenderer *m_renderer;
    QPoint m_selectedPoint;
    QSurface3DSeries *m_selectedSeries; // Points to the series for which the point is selected in
                                        // single series selection cases.
    bool m_flatShadingSupported;
    QList<ChangeItem> m_changedItems;
    QList<ChangeRow> m_changedRows;
    bool m_flipHorizontalGrid;
    QList<QSurface3DSeries *> m_changedTextures;

    Surface3DController::DataDimensions m_dataDimensions;

    Q_DISABLE_COPY(Surface3DController)
};

QT_END_NAMESPACE

#endif

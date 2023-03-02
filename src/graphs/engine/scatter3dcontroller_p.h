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

#ifndef Q3DSCATTERCONTROLLER_p_H
#define Q3DSCATTERCONTROLLER_p_H

#include <private/graphsglobal_p.h>
#include <private/abstract3dcontroller_p.h>

QT_BEGIN_NAMESPACE

class QScatterDataProxy;
class QScatter3DSeries;

struct Scatter3DChangeBitField {
    bool selectedItemChanged : 1;
    bool itemChanged         : 1;

    Scatter3DChangeBitField() :
        selectedItemChanged(true),
        itemChanged(false)
    {
    }
};

class Q_GRAPHS_EXPORT Scatter3DController : public Abstract3DController
{
    Q_OBJECT

public:
    struct ChangeItem {
        QScatter3DSeries *series;
        int index;
    };
private:
    Scatter3DChangeBitField m_changeTracker;
    QList<ChangeItem> m_changedItems;

    // Rendering
    int m_selectedItem;
    QScatter3DSeries *m_selectedItemSeries; // Points to the series for which the bar is selected
                                            // in single series selection cases.

    struct InsertRemoveRecord {
        bool m_isInsert;
        int m_startIndex;
        int m_count;
        QAbstract3DSeries *m_series;

        InsertRemoveRecord() :
            m_isInsert(false),
            m_startIndex(0),
            m_count(0),
            m_series(0)
        {}

        InsertRemoveRecord(bool isInsert, int startIndex, int count, QAbstract3DSeries *series) :
            m_isInsert(isInsert),
            m_startIndex(startIndex),
            m_count(count),
            m_series(series)
        {}
    };

    QList<InsertRemoveRecord> m_insertRemoveRecords;
    bool m_recordInsertsAndRemoves;

public:
    explicit Scatter3DController(QRect rect, Q3DScene *scene = 0);
    ~Scatter3DController();

    // Change selection mode
    void setSelectionMode(QAbstract3DGraph::SelectionFlags mode) override;

    inline QScatter3DSeries *selectedSeries() const { return m_selectedItemSeries; }

    void setSelectedItem(int index, QScatter3DSeries *series);
    static inline int invalidSelectionIndex() { return -1; }
    void clearSelection() override;

    void addSeries(QAbstract3DSeries *series) override;
    void removeSeries(QAbstract3DSeries *series) override;
    virtual QList<QScatter3DSeries *> scatterSeriesList();

    inline bool hasSelectedItemChanged() const { return m_changeTracker.selectedItemChanged; }
    inline void setSelectedItemChanged(bool changed) { m_changeTracker.selectedItemChanged = changed; }
    inline bool hasItemChanged() const { return m_changeTracker.itemChanged; }
    inline void setItemChanged(bool changed) { m_changeTracker.itemChanged = changed; }

    void clearChangedItems();

    void handleAxisAutoAdjustRangeChangedInOrientation(
            QAbstract3DAxis::AxisOrientation orientation, bool autoAdjust) override;
    void handleAxisRangeChangedBySender(QObject *sender) override;
    void adjustAxisRanges() override;
    inline bool hasChangedSeriesList() const { return !m_changedSeriesList.empty(); }
    inline bool isSeriesVisualsDirty() const { return m_isSeriesVisualsDirty; }
    inline void setSeriesVisualsDirty() { m_isSeriesVisualsDirty = true; }
    inline bool isDataDirty() const { return m_isDataDirty; }

public Q_SLOTS:
    void handleArrayReset();
    void handleItemsAdded(int startIndex, int count);
    void handleItemsChanged(int startIndex, int count);
    void handleItemsRemoved(int startIndex, int count);
    void handleItemsInserted(int startIndex, int count);

Q_SIGNALS:
    void selectedSeriesChanged(QScatter3DSeries *series);

protected:
    void startRecordingRemovesAndInserts() override;

private:

    friend class QQuickGraphsScatter;
    Q_DISABLE_COPY(Scatter3DController)
};

QT_END_NAMESPACE

#endif

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

#ifndef Q3DBARSCONTROLLER_p_H
#define Q3DBARSCONTROLLER_p_H

#include "datavisualizationglobal_p.h"
#include "abstract3dcontroller_p.h"

//#define DISPLAY_RENDER_SPEED

namespace QtDataVisualization {

class Bars3DRenderer;
class QBar3DSeries;

struct Bars3DChangeBitField {
    bool slicingActiveChanged     : 1;
    bool barSpecsChanged          : 1;
    bool selectedBarChanged       : 1;

    Bars3DChangeBitField() :
        slicingActiveChanged(true),
        barSpecsChanged(true),
        selectedBarChanged(true)
    {
    }
};

class QT_DATAVISUALIZATION_EXPORT Bars3DController : public Abstract3DController
{
    Q_OBJECT

private:
    Bars3DChangeBitField m_changeTracker;

    // Interaction
    QPoint m_selectedBar;     // Points to row & column in data window.
    QBar3DSeries *m_selectedBarSeries; // Points to the series for which the bar is selected in
                                       // single series selection cases.
    QBar3DSeries *m_primarySeries; // Category axis labels are taken from the primary series

    // Look'n'feel
    bool m_isBarSpecRelative;
    GLfloat m_barThicknessRatio;
    QSizeF m_barSpacing;

    // Rendering
    Bars3DRenderer *m_renderer;

public:
    explicit Bars3DController(QRect rect, Q3DScene *scene = 0);
    ~Bars3DController();

    virtual void initializeOpenGL();
    virtual void synchDataToRenderer();

    // bar thickness, spacing between bars, and is spacing relative to thickness or absolute
    // y -component sets the thickness/spacing of z -direction
    // With relative 0.0f means side-to-side, 1.0f = one thickness in between
    void setBarSpecs(GLfloat thicknessRatio = 1.0f,
                     const QSizeF &spacing = QSizeF(1.0, 1.0),
                     bool relative = true);
    GLfloat barThickness();
    QSizeF barSpacing();
    bool isBarSpecRelative();

    void setSelectionMode(QAbstract3DGraph::SelectionFlags mode);
    void setSelectedBar(const QPoint &position, QBar3DSeries *series);
    virtual void clearSelection();

    virtual void handleAxisAutoAdjustRangeChangedInOrientation(QAbstract3DAxis::AxisOrientation orientation, bool autoAdjust);
    virtual void handleSeriesVisibilityChangedBySender(QObject *sender);

    static QPoint invalidSelectionPosition();

    virtual void setAxisX(QAbstract3DAxis *axis);
    virtual void setAxisZ(QAbstract3DAxis *axis);

    virtual void setPrimarySeries(QBar3DSeries *series);
    virtual QBar3DSeries *primarySeries() const;
    virtual void addSeries(QAbstract3DSeries *series);
    virtual void removeSeries(QAbstract3DSeries *series);
    virtual void insertSeries(int index, QAbstract3DSeries *series);
    virtual QList<QBar3DSeries *> barSeriesList();

    virtual void handleAxisRangeChangedBySender(QObject *sender);

public slots:
    void handleArrayReset();
    void handleRowsAdded(int startIndex, int count);
    void handleRowsChanged(int startIndex, int count);
    void handleRowsRemoved(int startIndex, int count);
    void handleRowsInserted(int startIndex, int count);
    void handleItemChanged(int rowIndex, int columnIndex);
    void handleDataRowLabelsChanged();
    void handleDataColumnLabelsChanged();

    // Renderer callback handlers
    void handleBarClicked(const QPoint &position, QBar3DSeries *series);

signals:
    void primarySeriesChanged(QBar3DSeries *series);

protected:
    virtual QAbstract3DAxis *createDefaultAxis(QAbstract3DAxis::AxisOrientation orientation);

private:
    void adjustAxisRanges();
    void adjustSelectionPosition(QPoint &pos, const QBar3DSeries *series);

    Q_DISABLE_COPY(Bars3DController)

};


}

#endif

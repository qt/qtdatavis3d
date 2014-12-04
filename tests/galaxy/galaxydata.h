/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef SCATTERDATAMODIFIER_H
#define SCATTERDATAMODIFIER_H

#include "star.h"

#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qabstract3dseries.h>
#include <QtGui/QFont>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>

using namespace QtDataVisualization;

class GalaxyData : public QObject
{
    Q_OBJECT
public:
    explicit GalaxyData(Q3DScatter *scatter,
                        qreal rad = 13000,
                        qreal radCore = 4000.0,
                        qreal deltaAng = 0.0004,
                        qreal ex1 = 0.9,
                        qreal ex2 = 0.9);
    ~GalaxyData();

    qreal getExcentricity(qreal r) const;
    qreal getAngularOffset(qreal rad) const;

    void radiusGalaxyChanged(int value);
    void radiusCoreChanged(int value);
    void angleOffsetChanged(int value);
    void eccentricityInnerChanged(int value);
    void eccentricityOuterChanged(int value);
    void resetValues();
    void setFilteredEnabled(bool enabled);
    inline void setSliders(QSlider *rg,
                           QSlider *rc,
                           QSlider *ao,
                           QSlider *ei,
                           QSlider *eo) {
        m_radiusGalaxySlider = rg;
        m_radiusCoreSlider = rc;
        m_angleOffsetSlider = ao;
        m_eccentricityInnerSlider = ei;
        m_eccentricityOuterSlider = eo;
    }
    void setFpsLabel(QLabel *fpsLabel) { m_fpsLabel = fpsLabel; }
    void handleFpsChange(qreal fps);

private:
    void createGalaxy();
    void checkMinMax(const Star &star);
    void createNormalDataView();
    void createFilteredView();
    void createNormalSeries();
    qreal value;

private:
    Q3DScatter *m_graph;
    QScatterDataArray *m_dataArray;
    Star *m_pStars;

    qreal m_elEx1;          // Excentricity of the innermost ellipse
    qreal m_elEx2;          // Excentricity of the outermost ellipse

    qreal m_angleOffset;    // Angular offset per parsec

    qreal m_radCore;        // Radius of the inner core
    qreal m_radGalaxy;      // Radius of the galaxy
    qreal m_radFarField;    // The radius after which all density waves must have circular shape

    QSlider *m_radiusGalaxySlider;
    QSlider *m_radiusCoreSlider;
    QSlider *m_angleOffsetSlider;
    QSlider *m_eccentricityInnerSlider;
    QSlider *m_eccentricityOuterSlider;
    QLabel *m_fpsLabel;

    qreal m_minx = 9999.9;
    qreal m_maxx = -9999.0;
    qreal m_miny = 9999.9;
    qreal m_maxy = -9999.0;
    int m_range;
    bool m_filtered;
};

#endif

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

/*
 * Galaxy creation code obtained from http://beltoforion.de/galaxy/galaxy_en.html
 * Thanks to Ingo Berg, great work.
 * Licensed under a  Creative Commons Attribution 3.0 License
 * http://creativecommons.org/licenses/by/3.0/
 */

#ifndef CUMULATIVEDISTRIBUTOR_H
#define CUMULATIVEDISTRIBUTOR_H

#include <QtCore/qglobal.h>
#include <QtGui/QVector2D>
#include <QtCore/QVector>

class CumulativeDistributor
{
public:
    typedef qreal (CumulativeDistributor::*dist_fun_t)(qreal x);

    CumulativeDistributor();
    virtual ~CumulativeDistributor();

    qreal PropFromVal(qreal fVal);
    qreal ValFromProp(qreal fVal);

    void setupRealistic(qreal I0, qreal k, qreal a, qreal RBulge, qreal min,
                        qreal max, int nSteps);
    qreal valFromProp(qreal fVal);

private:
    dist_fun_t m_pDistFun;
    qreal m_fMin;
    qreal m_fMax;
    qreal m_fWidth;
    int m_nSteps;

    // parameters for realistic star distribution
    qreal m_I0;
    qreal m_k;
    qreal m_a;
    qreal m_RBulge;

    QVector<qreal> m_vM1;
    QVector<qreal> m_vY1;
    QVector<qreal> m_vX1;

    QVector<qreal> m_vM2;
    QVector<qreal> m_vY2;
    QVector<qreal> m_vX2;

    void buildCDF(int nSteps);

    qreal IntensityBulge(qreal R, qreal I0, qreal k);
    qreal IntensityDisc(qreal R, qreal I0, qreal a);
    qreal Intensity(qreal x);
};

#endif // CUMULATIVEDISTRIBUTOR_H

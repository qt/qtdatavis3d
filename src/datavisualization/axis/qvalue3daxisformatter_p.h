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

#include "datavisualizationglobal_p.h"
#include "qvalue3daxisformatter.h"
#include "utils_p.h"
#include <QtCore/QVector>

#ifndef QVALUE3DAXISFORMATTER_P_H
#define QVALUE3DAXISFORMATTER_P_H

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QValue3DAxis;

class QValue3DAxisFormatterPrivate : public QObject
{
    Q_OBJECT

public:
    QValue3DAxisFormatterPrivate(QValue3DAxisFormatter *q);
    virtual ~QValue3DAxisFormatterPrivate();

    void recalculate();
    void doRecalculate();
    void populateCopy(QValue3DAxisFormatter &copy);
    void doPopulateCopy(QValue3DAxisFormatterPrivate &copy);

    QString labelForIndex(int index) const;
    QString stringForValue(qreal value, const QString &format);
    float positionAt(float value) const;
    float valueAt(float position) const;

    void setAxis(QValue3DAxis *axis);
    void resetArrays();
    void markDirty(bool labelsChange);

public slots:
    void markDirtyNoLabelChange();

protected:
    QValue3DAxisFormatter *q_ptr;

    bool m_needsRecalculate;

    float m_min;
    float m_max;
    float m_rangeNormalizer;

    QVector<float> m_gridPositions;
    QVector<float> m_subGridPositions;
    QVector<float> m_labelPositions;
    QVector<qreal> m_labelValues;

    QValue3DAxis *m_axis;

    QString m_previousLabelFormat;
    QByteArray m_labelFormatArray;
    Utils::ParamType m_preparsedParamType;

    friend class QValue3DAxisFormatter;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

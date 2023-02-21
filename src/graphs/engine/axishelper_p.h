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

#ifndef AXISHELPER_P_H
#define AXISHELPER_P_H

#include "graphsglobal_p.h"
#include <qvalue3daxisformatter_p.h>
#include <QList>

QT_BEGIN_NAMESPACE
class AxisHelper : public QObject
{
    Q_OBJECT
public:
    AxisHelper();
    virtual ~AxisHelper();
    void updatePositions();

    inline float scale() { return m_scale; }
    inline void setScale(float scale) { m_scale = scale; }

    inline float translate() { return m_translate; }
    inline void setTranslate(float translate) { m_translate = translate; }

    inline QValue3DAxisFormatter *formatter() { return m_formatter; }
    inline void setFormatter(QValue3DAxisFormatter *formatter) { m_formatter = formatter; }

    inline float itemPositionAt(float value)
    {
        return m_formatter->positionAt(value) * m_scale + m_translate;
    }

    inline float labelPositionAt(int index)
    {
        return m_formatter->labelPositions().at(index) * m_scale + m_translate;
    }
    inline float gridPositionAt(int gridLine)
    {
        return m_formatter->gridPositions().at(gridLine) * m_scale + m_translate;
    }

    inline float subGridPositionAt(int gridLine)
    {
        return m_formatter->subGridPositions().at(gridLine) * m_scale + m_translate;
    }

    inline void setMin(float min) { m_min = min; }
    inline float min() const { return m_min; }
    inline void setMax(float max) { m_max = max; }
    inline float max() const { return m_max; }

    inline void setReversed(bool enable) { m_reversed = enable; }
    inline bool isReversed() const { return m_reversed; }

private:
    QList<float> m_adjustedGridPositions;
    QList<float> m_adjustedSubGridPositions;

    float m_scale;
    float m_translate;
    QValue3DAxisFormatter *m_formatter;

    float m_min;
    float m_max;

    bool m_reversed = false;
};

QT_END_NAMESPACE

#endif // AXISHELPER_P_H

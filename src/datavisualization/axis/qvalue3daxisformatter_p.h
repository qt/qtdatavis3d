/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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

#ifndef QVALUE3DAXISFORMATTER_P_H
#define QVALUE3DAXISFORMATTER_P_H

#include "datavisualizationglobal_p.h"
#include "qvalue3daxisformatter.h"
#include "utils_p.h"
#include <QtCore/QLocale>

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

    QString stringForValue(qreal value, const QString &format);
    float positionAt(float value) const;
    float valueAt(float position) const;

    void setAxis(QValue3DAxis *axis);
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
    QStringList m_labelStrings;

    QValue3DAxis *m_axis;

    QString m_previousLabelFormat;
    QByteArray m_labelFormatArray;
    Utils::ParamType m_preparsedParamType;

    bool m_allowNegatives;
    bool m_allowZero;

    QLocale m_locale;
    QString m_formatPreStr;
    QString m_formatPostStr;
    int m_formatPrecision;
    char m_formatSpec;
    bool m_cLocaleInUse;

    friend class QValue3DAxisFormatter;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

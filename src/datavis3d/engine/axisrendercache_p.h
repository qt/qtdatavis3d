/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef AXISRENDERCACHE_P_H
#define AXISRENDERCACHE_P_H

#include "datavis3dglobal_p.h"
#include "labelitem_p.h"
#include "q3dabstractaxis_p.h"
#include "drawer_p.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class AxisRenderCache : public QObject
{
    Q_OBJECT
public:
    AxisRenderCache();
    virtual ~AxisRenderCache();

    void setDrawer(Drawer *drawer);

    void setType(Q3DAbstractAxis::AxisType type);
    inline Q3DAbstractAxis::AxisType type() const { return m_type; }
    void setTitle(const QString &title);
    inline const QString &title() { return m_title; }
    void setLabels(const QStringList &labels);
    inline const QStringList &labels() { return m_labels; }
    void setMin(qreal min);
    inline qreal min() { return m_min; }
    void setMax(qreal max);
    inline qreal max() { return m_max; }
    void setSegmentCount(int count);
    inline int segmentCount() const { return m_segmentCount; }
    void setSubSegmentCount(int count);
    inline int subSegmentCount() const { return m_subSegmentCount; }
    inline void setLabelFormat(const QString &format) { m_labelFormat = format; }
    inline const QString &labelFormat() { return m_labelFormat; }

    inline LabelItem &titleItem() { return m_titleItem; }
    inline QList<LabelItem *> &labelItems() { return m_labelItems; }
    inline GLfloat segmentStep() const { return m_segmentStep; }
    inline GLfloat subSegmentStep() const { return m_subSegmentStep; }

public slots:
    void updateTextures();

private:
    void updateSegmentStep();
    void updateSubSegmentStep();

    // Cached axis values
    Q3DAbstractAxis::AxisType m_type;
    QString m_title;
    QStringList m_labels;
    qreal m_min;
    qreal m_max;
    int m_segmentCount;
    int m_subSegmentCount;
    QString m_labelFormat;

    // Renderer items
    Drawer *m_drawer; // Not owned
    LabelItem m_titleItem;
    QList<LabelItem *> m_labelItems;
    GLfloat m_segmentStep;
    GLfloat m_subSegmentStep;

    Q_DISABLE_COPY(AxisRenderCache)
};

QT_DATAVIS3D_END_NAMESPACE

#endif

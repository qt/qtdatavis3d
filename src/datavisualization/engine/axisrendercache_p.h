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

#ifndef AXISRENDERCACHE_P_H
#define AXISRENDERCACHE_P_H

#include "datavisualizationglobal_p.h"
#include "labelitem_p.h"
#include "qabstract3daxis_p.h"
#include "drawer_p.h"
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class AxisRenderCache : public QObject
{
    Q_OBJECT
public:
    AxisRenderCache();
    virtual ~AxisRenderCache();

    void setDrawer(Drawer *drawer);

    void setType(QAbstract3DAxis::AxisType type);
    inline QAbstract3DAxis::AxisType type() const { return m_type; }
    void setTitle(const QString &title);
    inline const QString &title() const { return m_title; }
    void setLabels(const QStringList &labels);
    inline const QStringList &labels() const { return m_labels; }
    inline void setMin(float min) { m_min = min; }
    inline float min() const { return m_min; }
    inline void setMax(float max) { m_max = max; }
    inline float max() const { return m_max; }
    inline void setSegmentCount(int count) { m_segmentCount = count; m_positionsDirty = true; }
    inline int segmentCount() const { return m_segmentCount; }
    inline void setSubSegmentCount(int count) { m_subSegmentCount = count; m_positionsDirty = true; }
    inline int subSegmentCount() const { return m_subSegmentCount; }
    inline void setLabelFormat(const QString &format) { m_labelFormat = format; }
    inline const QString &labelFormat() const { return m_labelFormat; }
    inline void setFormatter(QValue3DAxisFormatter *formatter)
    {
        m_formatter = formatter; m_positionsDirty = true;
    }
    inline QValue3DAxisFormatter *formatter() const { return m_formatter; }
    inline void setCtrlFormatter(QValue3DAxisFormatter *formatter)
    {
        m_ctrlFormatter = formatter;
    }
    inline QValue3DAxisFormatter *ctrlFormatter() const { return m_ctrlFormatter; }

    inline LabelItem &titleItem() { return m_titleItem; }
    inline QList<LabelItem *> &labelItems() { return m_labelItems; }
    inline float gridLinePosition(int index) { return m_adjustedGridLinePositions.at(index); }
    inline int gridLineCount() { return m_adjustedGridLinePositions.size(); }
    inline float labelPosition(int index) { return m_adjustedLabelPositions.at(index); }
    inline int labelCount() { return m_adjustedLabelPositions.size(); }
    void updateAllPositions();
    inline bool positionsDirty() const { return m_positionsDirty; }
    inline void markPositionsDirty() { m_positionsDirty = true; }
    inline void setTranslate(float translate) { m_translate = translate; m_positionsDirty = true; }
    inline float translate() { return m_translate; }
    inline void setScale(float scale) { m_scale = scale; m_positionsDirty = true; }
    inline float scale() { return m_scale; }
    inline float positionAt(float value)
    {
        return m_formatter->positionAt(value) * m_scale + m_translate;
    }

public slots:
    void updateTextures();

private:
    int maxLabelWidth(const QStringList &labels) const;

    // Cached axis values
    QAbstract3DAxis::AxisType m_type;
    QString m_title;
    QStringList m_labels;
    float m_min;
    float m_max;
    int m_segmentCount;
    int m_subSegmentCount;
    QString m_labelFormat;
    QFont m_font;
    QValue3DAxisFormatter *m_formatter;
    QPointer<QValue3DAxisFormatter> m_ctrlFormatter;

    // Renderer items
    Drawer *m_drawer; // Not owned
    LabelItem m_titleItem;
    QList<LabelItem *> m_labelItems;
    QVector<float> m_adjustedGridLinePositions;
    QVector<float> m_adjustedLabelPositions;
    bool m_positionsDirty;
    float m_translate;
    float m_scale;

    Q_DISABLE_COPY(AxisRenderCache)
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

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

#ifndef DECLARATIVEBARS_P_H
#define DECLARATIVEBARS_P_H

#include "datavisualizationglobal_p.h"
#include "abstractdeclarative_p.h"
#include "bars3dcontroller_p.h"
#include "declarativebars_p.h"
#include "q3dvalueaxis.h"
#include "q3dcategoryaxis.h"
#include "qbardataproxy.h"
#include "qbar3dseries.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>
#include <QQuickWindow>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class DeclarativeBars : public AbstractDeclarative
{
    Q_OBJECT
    Q_PROPERTY(Q3DCategoryAxis *rowAxis READ rowAxis WRITE setRowAxis)
    Q_PROPERTY(Q3DValueAxis *valueAxis READ valueAxis WRITE setValueAxis)
    Q_PROPERTY(Q3DCategoryAxis *columnAxis READ columnAxis WRITE setColumnAxis)
    Q_PROPERTY(QtDataVisualization::QDataVis::MeshStyle barType READ barType WRITE setBarType NOTIFY meshFileNameChanged)
    Q_PROPERTY(qreal barThickness READ barThickness WRITE setBarThickness NOTIFY barThicknessChanged)
    Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing NOTIFY barSpacingChanged)
    Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative NOTIFY barSpacingRelativeChanged)
    Q_PROPERTY(bool barSmoothingEnabled READ isBarSmoothingEnabled WRITE setBarSmoothingEnabled NOTIFY meshFileNameChanged)
    Q_PROPERTY(QString meshFileName READ meshFileName WRITE setMeshFileName NOTIFY meshFileNameChanged)
    Q_PROPERTY(QPointF selectedBar READ selectedBar WRITE setSelectedBar NOTIFY selectedBarChanged)
    Q_PROPERTY(QQmlListProperty<QBar3DSeries> seriesList READ seriesList)
    Q_CLASSINFO("DefaultProperty", "seriesList")

public:
    explicit DeclarativeBars(QQuickItem *parent = 0);
    ~DeclarativeBars();

    Q_INVOKABLE void setBarColor(const QColor &baseColor); // TODO property (or more likely as part of data set)

    Q3DCategoryAxis *rowAxis() const;
    void setRowAxis(Q3DCategoryAxis *axis);
    Q3DValueAxis *valueAxis() const;
    void setValueAxis(Q3DValueAxis *axis);
    Q3DCategoryAxis *columnAxis() const;
    void setColumnAxis(Q3DCategoryAxis *axis);

    void setBarThickness(qreal thicknessRatio);
    qreal barThickness() const;

    void setBarSpacing(QSizeF spacing);
    QSizeF barSpacing() const;

    void setBarSpacingRelative(bool relative);
    bool isBarSpacingRelative() const;

    void setBarType(QDataVis::MeshStyle style);
    QDataVis::MeshStyle barType() const;

    void setBarSmoothingEnabled(bool enabled);
    bool isBarSmoothingEnabled() const;

    void setMeshFileName(const QString &objFileName);
    QString meshFileName() const;

    void setSelectedBar(const QPointF &position);
    QPointF selectedBar() const;

    QQmlListProperty<QBar3DSeries> seriesList();
    static void appendSeriesFunc(QQmlListProperty<QBar3DSeries> *list, QBar3DSeries *series);
    static int countSeriesFunc(QQmlListProperty<QBar3DSeries> *list);
    static QBar3DSeries *atSeriesFunc(QQmlListProperty<QBar3DSeries> *list, int index);
    static void clearSeriesFunc(QQmlListProperty<QBar3DSeries> *list);
    Q_INVOKABLE void addSeries(QBar3DSeries *series);
    Q_INVOKABLE void removeSeries(QBar3DSeries *series);

signals:
    void selectedBarChanged(const QPointF &position);
    void barThicknessChanged(qreal thicknessRatio);
    void barSpacingChanged(QSizeF spacing);
    void barSpacingRelativeChanged(bool relative);
    void meshFileNameChanged(QString filename);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

private:
    Bars3DController *m_shared;
    QSize m_initialisedSize;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

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

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>
#include <QQuickWindow>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class DeclarativeBars : public AbstractDeclarative
{
    Q_OBJECT
    Q_PROPERTY(QBarDataProxy *dataProxy READ dataProxy WRITE setDataProxy)
    Q_PROPERTY(Q3DCategoryAxis *rowAxis READ rowAxis WRITE setRowAxis)
    Q_PROPERTY(Q3DValueAxis *valueAxis READ valueAxis WRITE setValueAxis)
    Q_PROPERTY(Q3DCategoryAxis *columnAxis READ columnAxis WRITE setColumnAxis)
    Q_PROPERTY(QtDataVisualization::QDataVis::MeshStyle barType READ barType WRITE setBarType)
    Q_PROPERTY(qreal barThickness READ barThickness WRITE setBarThickness)
    Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing)
    Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative)
    Q_PROPERTY(bool barSmoothingEnabled READ isBarSmoothingEnabled WRITE setBarSmoothingEnabled)
    Q_PROPERTY(QString meshFileName READ meshFileName WRITE setMeshFileName)
    Q_PROPERTY(QPointF selectedBar READ selectedBar WRITE setSelectedBar NOTIFY selectedBarChanged)

public:
    explicit DeclarativeBars(QQuickItem *parent = 0);
    ~DeclarativeBars();

    Q_INVOKABLE void setBarColor(const QColor &baseColor, bool uniform = true);

    QBarDataProxy *dataProxy() const;
    void setDataProxy(QBarDataProxy *dataProxy);

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

signals:
    void selectedBarChanged(const QPointF &position);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

private:
    Bars3DController *m_shared;
    QSize m_initialisedSize;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

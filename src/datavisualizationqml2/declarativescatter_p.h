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

#ifndef DECLARATIVESCATTER_P_H
#define DECLARATIVESCATTER_P_H

#include "datavisualizationglobal_p.h"
#include "abstractdeclarative_p.h"
#include "scatter3dcontroller_p.h"
#include "declarativescatter_p.h"
#include "q3dvalueaxis.h"
#include "qscatterdataproxy.h"
#include "qscatter3dseries.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class DeclarativeScatter : public AbstractDeclarative
{
    Q_OBJECT
    Q_PROPERTY(Q3DValueAxis *axisX READ axisX WRITE setAxisX)
    Q_PROPERTY(Q3DValueAxis *axisY READ axisY WRITE setAxisY)
    Q_PROPERTY(Q3DValueAxis *axisZ READ axisZ WRITE setAxisZ)
    Q_PROPERTY(QtDataVisualization::QDataVis::MeshStyle objectType READ objectType WRITE setObjectType NOTIFY meshFileNameChanged)
    Q_PROPERTY(bool objectSmoothingEnabled READ isObjectSmoothingEnabled WRITE setObjectSmoothingEnabled NOTIFY meshFileNameChanged)
    Q_PROPERTY(QString meshFileName READ meshFileName WRITE setMeshFileName NOTIFY meshFileNameChanged)
    Q_PROPERTY(int selectedItemIndex READ selectedItemIndex WRITE setSelectedItemIndex NOTIFY selectedItemIndexChanged)
    Q_PROPERTY(QQmlListProperty<QScatter3DSeries> seriesList READ seriesList)

public:
    explicit DeclarativeScatter(QQuickItem *parent = 0);
    ~DeclarativeScatter();

    Q_INVOKABLE void setObjectColor(const QColor &baseColor); // TODO property (or more likely as part of data set)

    Q3DValueAxis *axisX() const;
    void setAxisX(Q3DValueAxis *axis);
    Q3DValueAxis *axisY() const;
    void setAxisY(Q3DValueAxis *axis);
    Q3DValueAxis *axisZ() const;
    void setAxisZ(Q3DValueAxis *axis);

    void setObjectType(QDataVis::MeshStyle style);
    QDataVis::MeshStyle objectType() const;

    void setObjectSmoothingEnabled(bool enabled);
    bool isObjectSmoothingEnabled() const;

    void setMeshFileName(const QString &objFileName);
    QString meshFileName() const;

    void setSelectedItemIndex(int index);
    int selectedItemIndex() const;

    QQmlListProperty<QScatter3DSeries> seriesList();
    static void appendSeriesFunc(QQmlListProperty<QScatter3DSeries> *list, QScatter3DSeries *series);
    static int countSeriesFunc(QQmlListProperty<QScatter3DSeries> *list);
    static QScatter3DSeries *atSeriesFunc(QQmlListProperty<QScatter3DSeries> *list, int index);
    static void clearSeriesFunc(QQmlListProperty<QScatter3DSeries> *list);
    Q_INVOKABLE void addSeries(QScatter3DSeries *series);
    Q_INVOKABLE void removeSeries(QScatter3DSeries *series);

signals:
    void selectedItemIndexChanged(int index);
    void meshFileNameChanged(QString filename);

protected:
    Scatter3DController *m_shared;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

private:
    QSize m_initialisedSize;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

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

#ifndef QABSTRACT3DSERIES_H
#define QABSTRACT3DSERIES_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QObject>
#include <QScopedPointer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QAbstract3DSeriesPrivate;

class QT_DATAVISUALIZATION_EXPORT QAbstract3DSeries : public QObject
{
    Q_OBJECT
    Q_ENUMS(SeriesType)
    Q_ENUMS(Mesh)
    Q_PROPERTY(SeriesType type READ type CONSTANT)
    Q_PROPERTY(QString itemLabelFormat READ itemLabelFormat WRITE setItemLabelFormat NOTIFY itemLabelFormatChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
    Q_PROPERTY(Mesh mesh READ mesh WRITE setMesh NOTIFY meshChanged)
    Q_PROPERTY(bool meshSmooth READ isMeshSmooth WRITE setMeshSmooth NOTIFY meshSmoothChanged)
    Q_PROPERTY(QString userDefinedMesh READ userDefinedMesh WRITE setUserDefinedMesh NOTIFY userDefinedMeshChanged)

public:
    enum SeriesType {
        SeriesTypeNone = 0,
        SeriesTypeBar = 1,
        SeriesTypeScatter = 2,
        SeriesTypeSurface = 4
    };

    enum Mesh {
        MeshUserDefined = 0,
        MeshBar,
        MeshCube,
        MeshPyramid,
        MeshCone,
        MeshCylinder,
        MeshBevelBar,
        MeshBevelCube,
        MeshSphere,
        MeshMinimal,
        MeshPoint
    };

protected:
    explicit QAbstract3DSeries(QAbstract3DSeriesPrivate *d, QObject *parent = 0);

public:
    virtual ~QAbstract3DSeries();

    SeriesType type() const;

    void setItemLabelFormat(const QString &format);
    QString itemLabelFormat() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void setMesh(Mesh mesh);
    Mesh mesh() const;

    void setMeshSmooth(bool enable);
    bool isMeshSmooth() const;

    void setUserDefinedMesh(const QString &fileName);
    QString userDefinedMesh() const;

signals:
    void itemLabelFormatChanged(QString format);
    void visibilityChanged(bool visible);
    void meshChanged(Mesh mesh);
    void meshSmoothChanged(bool enabled);
    void userDefinedMeshChanged(QString fileName);

protected:
    QScopedPointer<QAbstract3DSeriesPrivate> d_ptr;

//    Q3DTheme::ColorStyle m_colorStyle;
//    QColor m_objectColor;
//    QLinearGradient m_objectGradient;
//    QColor m_singleHighlightColor;
//    QLinearGradient m_singleHighlightGradient;
//    QColor m_multiHighlightColor;
//    QLinearGradient m_multiHighlightGradient;

private:
    Q_DISABLE_COPY(QAbstract3DSeries)

    friend class Abstract3DController;
    friend class Bars3DController;
    friend class Surface3DController;
    friend class Scatter3DController;
    friend class QBar3DSeries;
    friend class SeriesRenderCache;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

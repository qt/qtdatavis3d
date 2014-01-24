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

#include <QtDataVisualization/q3dtheme.h>
#include <QObject>
#include <QScopedPointer>
#include <QLinearGradient>
#include <QQuaternion>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

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
    Q_PROPERTY(QQuaternion meshRotation READ meshRotation WRITE setMeshRotation NOTIFY meshRotationChanged)
    Q_PROPERTY(QString userDefinedMesh READ userDefinedMesh WRITE setUserDefinedMesh NOTIFY userDefinedMeshChanged)
    Q_PROPERTY(QtDataVisualization::Q3DTheme::ColorStyle colorStyle READ colorStyle WRITE setColorStyle NOTIFY colorStyleChanged)
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor NOTIFY baseColorChanged)
    Q_PROPERTY(QLinearGradient baseGradient READ baseGradient WRITE setBaseGradient NOTIFY baseGradientChanged)
    Q_PROPERTY(QColor singleHighlightColor READ singleHighlightColor WRITE setSingleHighlightColor NOTIFY singleHighlightColorChanged)
    Q_PROPERTY(QLinearGradient singleHighlightGradient READ singleHighlightGradient WRITE setSingleHighlightGradient NOTIFY singleHighlightGradientChanged)
    Q_PROPERTY(QColor multiHighlightColor READ multiHighlightColor WRITE setMultiHighlightColor NOTIFY multiHighlightColorChanged)
    Q_PROPERTY(QLinearGradient multiHighlightGradient READ multiHighlightGradient WRITE setMultiHighlightGradient NOTIFY multiHighlightGradientChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

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
        MeshArrow,
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

    void setMeshRotation(const QQuaternion &rotation);
    QQuaternion meshRotation() const;

    void setUserDefinedMesh(const QString &fileName);
    QString userDefinedMesh() const;

    void setColorStyle(Q3DTheme::ColorStyle style);
    Q3DTheme::ColorStyle colorStyle() const;
    void setBaseColor(const QColor &color);
    QColor baseColor() const;
    void setBaseGradient(const QLinearGradient &gradient);
    QLinearGradient baseGradient() const;
    void setSingleHighlightColor(const QColor &color);
    QColor singleHighlightColor() const;
    void setSingleHighlightGradient(const QLinearGradient &gradient);
    QLinearGradient singleHighlightGradient() const;
    void setMultiHighlightColor(const QColor &color);
    QColor multiHighlightColor() const;
    void setMultiHighlightGradient(const QLinearGradient &gradient);
    QLinearGradient multiHighlightGradient() const;

    void setName(const QString &name);
    QString name() const;

signals:
    void itemLabelFormatChanged(QString format);
    void visibilityChanged(bool visible);
    void meshChanged(Mesh mesh);
    void meshSmoothChanged(bool enabled);
    void meshRotationChanged(QQuaternion rotation);
    void userDefinedMeshChanged(QString fileName);
    void colorStyleChanged(Q3DTheme::ColorStyle style);
    void baseColorChanged(QColor color);
    void baseGradientChanged(QLinearGradient gradient);
    void singleHighlightColorChanged(QColor color);
    void singleHighlightGradientChanged(QLinearGradient gradient);
    void multiHighlightColorChanged(QColor color);
    void multiHighlightGradientChanged(QLinearGradient gradient);
    void nameChanged(QString name);

protected:
    QScopedPointer<QAbstract3DSeriesPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstract3DSeries)

    friend class Abstract3DController;
    friend class Bars3DController;
    friend class Surface3DController;
    friend class Scatter3DController;
    friend class QBar3DSeries;
    friend class SeriesRenderCache;
    friend class Abstract3DRenderer;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

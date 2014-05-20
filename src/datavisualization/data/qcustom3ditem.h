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

#ifndef QCUSTOM3DITEM_H
#define QCUSTOM3DITEM_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtGui/QImage>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QCustom3DItemPrivate;

class QT_DATAVISUALIZATION_EXPORT QCustom3DItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString meshFile READ meshFile WRITE setMeshFile NOTIFY meshFileChanged)
    Q_PROPERTY(QString textureFile READ textureFile WRITE setTextureFile NOTIFY textureFileChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(bool positionAbsolute READ isPositionAbsolute WRITE setPositionAbsolute NOTIFY positionAbsoluteChanged)
    Q_PROPERTY(QVector3D scaling READ scaling WRITE setScaling NOTIFY scalingChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool shadowCasting READ isShadowCasting WRITE setShadowCasting NOTIFY shadowCastingChanged)

public:
    explicit QCustom3DItem(QObject *parent = 0);
    explicit QCustom3DItem(const QString &meshFile, const QVector3D &position,
                           const QVector3D &scaling, const QQuaternion &rotation,
                           const QImage &texture, QObject *parent = 0);
    virtual ~QCustom3DItem();

    void setMeshFile(const QString &meshFile);
    QString meshFile() const;

    void setTextureFile(const QString &textureFile);
    QString textureFile() const;

    void setPosition(const QVector3D &position);
    QVector3D position() const;

    void setPositionAbsolute(bool positionAbsolute);
    bool isPositionAbsolute() const;

    void setScaling(const QVector3D &scaling);
    QVector3D scaling() const;

    void setRotation(const QQuaternion &rotation);
    QQuaternion rotation();

    void setVisible(bool visible);
    bool isVisible() const;

    void setShadowCasting(bool enabled);
    bool isShadowCasting() const;

    Q_INVOKABLE void setRotationAxisAndAngle(const QVector3D &axis, float angle);

    void setTextureImage(const QImage &textureImage);

signals:
    void meshFileChanged(const QString &meshFile);
    void textureFileChanged(const QString &textureFile);
    void positionChanged(const QVector3D &position);
    void positionAbsoluteChanged(bool positionAbsolute);
    void scalingChanged(const QVector3D &scaling);
    void rotationChanged(const QQuaternion &rotation);
    void visibleChanged(bool visible);
    void shadowCastingChanged(bool shadowCasting);

protected:
    QScopedPointer<QCustom3DItemPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QCustom3DItem)

    friend class Abstract3DRenderer;
    friend class Abstract3DController;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

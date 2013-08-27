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

#ifndef QITEMMODELSCATTERDATAMAPPING_H
#define QITEMMODELSCATTERDATAMAPPING_H

#include <QtDataVis3D/qdatavis3denums.h>
#include <QObject>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelScatterDataMappingPrivate;

class QT_DATAVIS3D_EXPORT QItemModelScatterDataMapping : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString xPosRole READ xPosRole WRITE setXPosRole NOTIFY mappingChanged)
    Q_PROPERTY(QString yPosRole READ yPosRole WRITE setYPosRole NOTIFY mappingChanged)
    Q_PROPERTY(QString zPosRole READ zPosRole WRITE setZPosRole NOTIFY mappingChanged)
    //Q_PROPERTY(QString valueRole READ valueRole WRITE setValueRole NOTIFY mappingChanged)

public:
    explicit QItemModelScatterDataMapping(QObject *parent = 0);
    QItemModelScatterDataMapping(const QString &xPosRole, const QString &yPosRole,
                                 const QString &zPosRole, const QString &valueRole,
                                 QObject *parent = 0);
    virtual ~QItemModelScatterDataMapping();

    void setXPosRole(const QString &role);
    QString xPosRole() const;
    void setYPosRole(const QString &role);
    QString yPosRole() const;
    void setZPosRole(const QString &role);
    QString zPosRole() const;
    // TODO: This is a placeholder for scatter item size
    //void setValueRole(const QString &role);
    //QString valueRole() const;

    void remap(const QString &xPosRole, const QString &yPosRole, const QString &zPosRole,
               const QString &valueRole);

signals:
    void mappingChanged();

private:
    QScopedPointer<QItemModelScatterDataMappingPrivate> d_ptr;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

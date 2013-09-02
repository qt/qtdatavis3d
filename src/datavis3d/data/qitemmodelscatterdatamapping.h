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
#include <QtDataVis3D/qabstractdatamapping.h>
#include <QObject>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelScatterDataMappingPrivate;

class QT_DATAVIS3D_EXPORT QItemModelScatterDataMapping : public QAbstractDataMapping
{
    Q_OBJECT
    Q_PROPERTY(QString xPosRole READ xPosRole WRITE setXPosRole)
    Q_PROPERTY(QString yPosRole READ yPosRole WRITE setYPosRole)
    Q_PROPERTY(QString zPosRole READ zPosRole WRITE setZPosRole)

public:
    explicit QItemModelScatterDataMapping(QObject *parent = 0);
    QItemModelScatterDataMapping(const QString &xPosRole, const QString &yPosRole,
                                 const QString &zPosRole, QObject *parent = 0);
    virtual ~QItemModelScatterDataMapping();

    void setXPosRole(const QString &role);
    QString xPosRole() const;
    void setYPosRole(const QString &role);
    QString yPosRole() const;
    void setZPosRole(const QString &role);
    QString zPosRole() const;

    void remap(const QString &xPosRole, const QString &yPosRole, const QString &zPosRole);

protected:
    QItemModelScatterDataMappingPrivate *dptr();
    const QItemModelScatterDataMappingPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QItemModelScatterDataMapping)
};

QT_DATAVIS3D_END_NAMESPACE

#endif

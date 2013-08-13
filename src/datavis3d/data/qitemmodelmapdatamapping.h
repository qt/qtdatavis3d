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

#ifndef QITEMMODELMAPDATAMAPPING_H
#define QITEMMODELMAPDATAMAPPING_H

#include <QtDataVis3D/qdatavis3dnamespace.h>
#include <QStringList>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelMapDataMappingPrivate;

class QT_DATAVIS3D_EXPORT QItemModelMapDataMapping : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString labelRole READ labelRole WRITE setLabelRole)
    Q_PROPERTY(QString xPosRole READ xPosRole WRITE setXPosRole)
    Q_PROPERTY(QString yPosRole READ yPosRole WRITE setYPosRole)
    Q_PROPERTY(QString valueRole READ valueRole WRITE setValueRole)
public:
    explicit QItemModelMapDataMapping();
    QItemModelMapDataMapping(const QItemModelMapDataMapping &other);
    QItemModelMapDataMapping(const QString &labelRole, const QString &xPosRole,
                             const QString &yPosRole, const QString &valueRole);
    virtual ~QItemModelMapDataMapping();

    QItemModelMapDataMapping &operator=(const QItemModelMapDataMapping &other);

    void setLabelRole(const QString &role);
    QString labelRole() const;
    void setXPosRole(const QString &role);
    QString xPosRole() const;
    void setYPosRole(const QString &role);
    QString yPosRole() const;
    void setValueRole(const QString &role);
    QString valueRole() const;

    void remap(const QString &labelRole, const QString &xPosRole,
               const QString &yPosRole, const QString &valueRole);
signals:
    void mappingChanged();

private:
    QScopedPointer<QItemModelMapDataMappingPrivate> d_ptr;
};


QT_DATAVIS3D_END_NAMESPACE

#endif

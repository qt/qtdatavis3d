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

#ifndef QCATEGORYAXIS_H
#define QCATEGORYAXIS_H

#include <QtDataVis3D/qabstractaxis.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QCategoryAxisPrivate;

class QT_DATAVIS3D_EXPORT QCategoryAxis : public QAbstractAxis
{
    Q_OBJECT
    // Note: categoryLabels actually reads/writes the labels property in abstract axis,
    //       which is read only there. Since subclass cannot have property with same name,
    //       this partially duplicate property is necessary.
    Q_PROPERTY(QStringList categoryLabels READ categoryLabels WRITE setCategoryLabels)

public:
    explicit QCategoryAxis(QObject *parent = 0);
    ~QCategoryAxis();

    QStringList categoryLabels() const;

public slots:
    void setCategoryLabels(const QStringList &labels);

protected:
    QCategoryAxisPrivate *dptr();

private:

    Q_DISABLE_COPY(QCategoryAxis)
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QCATEGORYAXIS_H

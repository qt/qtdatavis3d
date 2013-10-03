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

#ifndef Q3DCATEGORYAXIS_H
#define Q3DCATEGORYAXIS_H

#include <QtDataVisualization/q3dabstractaxis.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DCategoryAxisPrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DCategoryAxis : public Q3DAbstractAxis
{
    Q_OBJECT
    Q_PROPERTY(QStringList categoryLabels READ categoryLabels WRITE setCategoryLabels)

public:
    explicit Q3DCategoryAxis(QObject *parent = 0);
    virtual ~Q3DCategoryAxis();

    QStringList categoryLabels() const;
    void setCategoryLabels(const QStringList &labels);

protected:
    Q3DCategoryAxisPrivate *dptr();

private:
    Q_DISABLE_COPY(Q3DCategoryAxis)
    friend class Bars3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // QCATEGORYAXIS_H

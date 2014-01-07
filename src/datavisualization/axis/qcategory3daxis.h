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

#ifndef QCATEGORY3DAXIS_H
#define QCATEGORY3DAXIS_H

#include <QtDataVisualization/qabstract3daxis.h>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QCategory3DAxisPrivate;

class QT_DATAVISUALIZATION_EXPORT QCategory3DAxis : public QAbstract3DAxis
{
    Q_OBJECT
    Q_PROPERTY(QStringList labels READ labels WRITE setLabels NOTIFY labelsChanged)

public:
    explicit QCategory3DAxis(QObject *parent = 0);
    virtual ~QCategory3DAxis();

    void setLabels(const QStringList &labels);
    QStringList labels() const;

signals:
    void labelsChanged();

protected:
    QCategory3DAxisPrivate *dptr();

private:
    Q_DISABLE_COPY(QCategory3DAxis)
    friend class Bars3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

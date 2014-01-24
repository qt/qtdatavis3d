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

#ifndef QVALUE3DAXIS_H
#define QVALUE3DAXIS_H

#include <QtDataVisualization/qabstract3daxis.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QValue3DAxisPrivate;

class QT_DATAVISUALIZATION_EXPORT QValue3DAxis : public QAbstract3DAxis
{
    Q_OBJECT
    Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)
    Q_PROPERTY(int subSegmentCount READ subSegmentCount WRITE setSubSegmentCount NOTIFY subSegmentCountChanged)
    Q_PROPERTY(QString labelFormat READ labelFormat WRITE setLabelFormat NOTIFY labelFormatChanged)

public:
    explicit QValue3DAxis(QObject *parent = 0);
    virtual ~QValue3DAxis();

    void setSegmentCount(int count);
    int segmentCount() const;

    void setSubSegmentCount(int count);
    int subSegmentCount() const;

    void setLabelFormat(const QString &format);
    QString labelFormat() const;

signals:
    void segmentCountChanged(int count);
    void subSegmentCountChanged(int count);
    void labelFormatChanged(QString format);

protected:
    QValue3DAxisPrivate *dptr();
    const QValue3DAxisPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QValue3DAxis)
    friend class Bars3DController;
    friend class Scatter3DController;
    friend class Surface3DController;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

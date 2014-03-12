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

#ifndef QVALUE3DAXISFORMATTER_H
#define QVALUE3DAXISFORMATTER_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QValue3DAxisFormatterPrivate;
class QValue3DAxis;

class QT_DATAVISUALIZATION_EXPORT QValue3DAxisFormatter : public QObject
{
    Q_OBJECT

protected:
    explicit QValue3DAxisFormatter(QValue3DAxisFormatterPrivate *d, QObject *parent = 0);
public:
    explicit QValue3DAxisFormatter(QObject *parent = 0);
    virtual ~QValue3DAxisFormatter();

    virtual bool allowNegatives() const;
    virtual bool allowZero() const;

    // Getters not const as they can trigger recalculate
    float gridPosition(int index);
    float subGridPosition(int subGridIndex, int segmentIndex);
    float labelPosition(int index);
    QString stringForValue(float value, const QString &format);
    float positionAt(float value);
    float valueAt(float position);

    virtual QValue3DAxisFormatter *createNewInstance();
    void populateCopy(QValue3DAxisFormatter &copy);

protected:
    void markDirty();
    QValue3DAxis *axis() const;

    virtual void doRecalculate();
    virtual QString doStringForValue(float value, const QString &format);
    virtual float doPositionAt(float value);
    virtual float doValueAt(float position);
    virtual void doPopulateCopy(QValue3DAxisFormatter &copy);

    QVector<float> &gridPositions();
    QVector<QVector<float> > &subGridPositions();
    QVector<float> &labelPositions();

    QScopedPointer<QValue3DAxisFormatterPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QValue3DAxisFormatter)

    friend class Abstract3DController;
    friend class QValue3DAxisFormatterPrivate;
    friend class QValue3DAxis;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

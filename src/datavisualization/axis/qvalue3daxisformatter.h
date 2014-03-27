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

protected:
    virtual bool allowNegatives() const;
    virtual bool allowZero() const;
    virtual QValue3DAxisFormatter *createNewInstance() const;
    virtual void recalculate();
    virtual QString labelForIndex(int index) const;
    virtual QString stringForValue(qreal value, const QString &format) const;
    virtual float positionAt(float value) const;
    virtual float valueAt(float position) const;
    virtual void populateCopy(QValue3DAxisFormatter &copy) const;

    void markDirty(bool labelsChange = false);
    QValue3DAxis *axis() const;

    QVector<float> &gridPositions() const;
    QVector<float> &subGridPositions() const;
    QVector<float> &labelPositions() const;
    QVector<qreal> &labelValues() const;

    QScopedPointer<QValue3DAxisFormatterPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QValue3DAxisFormatter)

    friend class Abstract3DController;
    friend class Abstract3DRenderer;
    friend class Bars3DRenderer;
    friend class Scatter3DRenderer;
    friend class Surface3DRenderer;
    friend class SurfaceObject;
    friend class QValue3DAxisFormatterPrivate;
    friend class QLogValue3DAxisFormatter;
    friend class QValue3DAxis;
    friend class QValue3DAxisPrivate;
    friend class AxisRenderCache;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

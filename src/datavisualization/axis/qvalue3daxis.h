// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QVALUE3DAXIS_H
#define QVALUE3DAXIS_H

#include <QtGraphs/qabstract3daxis.h>
#include <QtGraphs/qvalue3daxisformatter.h>

QT_BEGIN_NAMESPACE

class QValue3DAxisPrivate;

class Q_GRAPHS_EXPORT QValue3DAxis : public QAbstract3DAxis
{
    Q_OBJECT
    Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)
    Q_PROPERTY(int subSegmentCount READ subSegmentCount WRITE setSubSegmentCount NOTIFY subSegmentCountChanged)
    Q_PROPERTY(QString labelFormat READ labelFormat WRITE setLabelFormat NOTIFY labelFormatChanged)
    Q_PROPERTY(QValue3DAxisFormatter* formatter READ formatter WRITE setFormatter NOTIFY formatterChanged)
    Q_PROPERTY(bool reversed READ reversed WRITE setReversed NOTIFY reversedChanged)

public:
    explicit QValue3DAxis(QObject *parent = nullptr);
    virtual ~QValue3DAxis();

    void setSegmentCount(int count);
    int segmentCount() const;

    void setSubSegmentCount(int count);
    int subSegmentCount() const;

    void setLabelFormat(const QString &format);
    QString labelFormat() const;

    void setFormatter(QValue3DAxisFormatter *formatter);
    QValue3DAxisFormatter *formatter() const;

    void setReversed(bool enable);
    bool reversed() const;

    void recalculate();
    int gridSize();
    int subGridSize();
    float gridPositionAt(int gridLine);
    float subGridPositionAt(int gridLine);
    float labelPositionAt(int index);
    float positionAt(float x);
    QString stringForValue(float x);

Q_SIGNALS:
    void segmentCountChanged(int count);
    void subSegmentCountChanged(int count);
    void labelFormatChanged(const QString &format);
    void formatterChanged(QValue3DAxisFormatter *formatter);
    void reversedChanged(bool enable);

protected:
    QValue3DAxisPrivate *dptr();
    const QValue3DAxisPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QValue3DAxis)
    friend class Abstract3DController;
    friend class Bars3DController;
    friend class Scatter3DController;
    friend class Surface3DController;
    friend class QValue3DAxisFormatterPrivate;
    friend class QQuickDataVisScatter;
    friend class QQuickDataVisBars;
};

QT_END_NAMESPACE

#endif

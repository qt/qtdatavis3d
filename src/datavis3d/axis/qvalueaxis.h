/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVALUEAXIS_H
#define QVALUEAXIS_H

#include "qabstractaxis.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

class QValueAxisPrivate;

class QT_DATAVIS3D_EXPORT QValueAxis : public QAbstractAxis
{
    Q_OBJECT
    Q_PROPERTY(qreal min READ min WRITE setMin NOTIFY rangeChanged)
    Q_PROPERTY(qreal max READ max WRITE setMax NOTIFY rangeChanged)
    Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)
    Q_PROPERTY(int subSegmentCount READ subSegmentCount WRITE setSubSegmentCount NOTIFY subSegmentCountChanged)
    Q_PROPERTY(bool autoAdjustRange READ isAutoAdjustRange WRITE setAutoAdjustRange NOTIFY autoAdjustRangeChanged)
    Q_PROPERTY(QString labelFormat READ labelFormat WRITE setLabelFormat NOTIFY labelFormatChanged)

public:
    explicit QValueAxis();
    ~QValueAxis();

    qreal min() const;
    qreal max() const;
    int segmentCount() const;
    int subSegmentCount() const;
    bool isAutoAdjustRange() const;
    QString labelFormat() const;

public slots:
    void setRange(qreal min, qreal max);
    void setMin(qreal min);
    void setMax(qreal max);
    void setSegmentCount(int count);
    void setSubSegmentCount(int count);
    void setAutoAdjustRange(bool autoAdjust);
    void setLabelFormat(const QString &format);

signals:
    void rangeChanged(qreal min, qreal max);
    void segmentCountChanged(int count);
    void subSegmentCountChanged(int count);
    void autoAdjustRangeChanged(bool autoAdjust);
    void labelFormatChanged(QString format);

protected:
    QValueAxisPrivate *dptr();
    const QValueAxisPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QValueAxis)
    friend class Bars3dController;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QVALUEAXIS_H

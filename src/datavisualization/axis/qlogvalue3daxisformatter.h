/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef QLOGVALUE3DAXISFORMATTER_H
#define QLOGVALUE3DAXISFORMATTER_H

#include <QtDataVisualization/qvalue3daxisformatter.h>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QLogValue3DAxisFormatterPrivate;

class QT_DATAVISUALIZATION_EXPORT QLogValue3DAxisFormatter : public QValue3DAxisFormatter
{
    Q_OBJECT

    Q_PROPERTY(qreal base READ base WRITE setBase NOTIFY baseChanged)
    Q_PROPERTY(bool autoSubGrid READ autoSubGrid WRITE setAutoSubGrid NOTIFY autoSubGridChanged)
    Q_PROPERTY(bool showEdgeLabels READ showEdgeLabels WRITE setShowEdgeLabels NOTIFY showEdgeLabelsChanged)

protected:
    explicit QLogValue3DAxisFormatter(QLogValue3DAxisFormatterPrivate *d, QObject *parent = 0);
public:
    explicit QLogValue3DAxisFormatter(QObject *parent = 0);
    virtual ~QLogValue3DAxisFormatter();

    void setBase(qreal base);
    qreal base() const;
    void setAutoSubGrid(bool enabled);
    bool autoSubGrid() const;
    void setShowEdgeLabels(bool enabled);
    bool showEdgeLabels() const;

Q_SIGNALS:
    void baseChanged(qreal base);
    void autoSubGridChanged(bool enabled);
    void showEdgeLabelsChanged(bool enabled);

protected:
    virtual QValue3DAxisFormatter *createNewInstance() const;
    virtual void recalculate();
    virtual float positionAt(float value) const;
    virtual float valueAt(float position) const;
    virtual void populateCopy(QValue3DAxisFormatter &copy) const;

    QLogValue3DAxisFormatterPrivate *dptr();
    const QLogValue3DAxisFormatterPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QLogValue3DAxisFormatter)

    friend class QLogValue3DAxisFormatterPrivate;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

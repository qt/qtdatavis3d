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
    Q_PROPERTY(bool showMaxLabel READ showMaxLabel WRITE setShowMaxLabel NOTIFY showMaxLabelChanged)

protected:
    explicit QLogValue3DAxisFormatter(QLogValue3DAxisFormatterPrivate *d, QObject *parent = 0);
public:
    explicit QLogValue3DAxisFormatter(QObject *parent = 0);
    virtual ~QLogValue3DAxisFormatter();

    void setBase(qreal base);
    qreal base() const;
    void setAutoSubGrid(bool enabled);
    bool autoSubGrid() const;
    void setShowMaxLabel(bool enabled);
    bool showMaxLabel() const;

signals:
    void baseChanged(qreal base);
    void autoSubGridChanged(bool enabled);
    void showMaxLabelChanged(bool enabled);

protected:
    virtual bool allowNegatives() const;
    virtual bool allowZero() const;
    virtual QValue3DAxisFormatter *createNewInstance() const;
    virtual void recalculate();
    virtual float positionAt(float value) const;
    virtual float valueAt(float position) const;
    virtual void populateCopy(QValue3DAxisFormatter &copy) const;
    virtual QString labelForIndex(int index) const;

    QLogValue3DAxisFormatterPrivate *dptr();
    const QLogValue3DAxisFormatterPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QLogValue3DAxisFormatter)

    friend class QLogValue3DAxisFormatterPrivate;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

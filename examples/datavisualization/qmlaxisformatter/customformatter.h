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

#ifndef CUSTOMFORMATTER_H
#define CUSTOMFORMATTER_H

#include <QtDataVisualization/QValue3DAxisFormatter>
#include <QtCore/QDateTime>
#include <QtCore/QVector>

using namespace QtDataVisualization;

//! [2]
class CustomFormatter : public QValue3DAxisFormatter
{
    //! [2]
    Q_OBJECT

    //! [1]
    Q_PROPERTY(QDate originDate READ originDate WRITE setOriginDate NOTIFY originDateChanged)
    //! [1]
    //! [3]
    Q_PROPERTY(QString selectionFormat READ selectionFormat WRITE setSelectionFormat NOTIFY selectionFormatChanged)
    //! [3]
public:
    explicit CustomFormatter(QObject *parent = 0);
    virtual ~CustomFormatter();

    //! [0]
    virtual QValue3DAxisFormatter *createNewInstance() const;
    virtual void populateCopy(QValue3DAxisFormatter &copy) const;
    virtual void recalculate();
    virtual QString stringForValue(qreal value, const QString &format) const;
    //! [0]

    QDate originDate() const;
    QString selectionFormat() const;

public Q_SLOTS:
    void setOriginDate(const QDate &date);
    void setSelectionFormat(const QString &format);

Q_SIGNALS:
    void originDateChanged(const QDate &date);
    void selectionFormatChanged(const QString &format);

private:
    Q_DISABLE_COPY(CustomFormatter)

    QDateTime valueToDateTime(qreal value) const;

    QDate m_originDate;
    QString m_selectionFormat;
};

#endif

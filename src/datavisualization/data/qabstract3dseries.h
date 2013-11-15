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

#ifndef QABSTRACT3DSERIES_H
#define QABSTRACT3DSERIES_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QObject>
#include <QScopedPointer>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QAbstract3DSeriesPrivate;

class QT_DATAVISUALIZATION_EXPORT QAbstract3DSeries : public QObject
{
    Q_OBJECT
    Q_ENUMS(SeriesType)
    Q_PROPERTY(SeriesType type READ type)
    Q_PROPERTY(QString itemLabelFormat READ itemLabelFormat WRITE setItemLabelFormat NOTIFY itemLabelFormatChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)

public:
    enum SeriesType {
        SeriesTypeNone = 0,
        SeriesTypeBar = 1,
        SeriesTypeScatter = 2,
        SeriesTypeSurface = 4
    };

protected:
    explicit QAbstract3DSeries(QAbstract3DSeriesPrivate *d, QObject *parent = 0);

public:
    virtual ~QAbstract3DSeries();

    SeriesType type() const;

    void setItemLabelFormat(const QString &format);
    QString itemLabelFormat() const;

    void setVisible(bool visible);
    bool isVisible() const;

signals:
    void itemLabelFormatChanged(QString format);
    void visibilityChanged(bool visible);

protected:
    QScopedPointer<QAbstract3DSeriesPrivate> d_ptr;

//    QDataVis::ColorStyle m_colorStyle;
//    QColor m_objectColor;
//    QLinearGradient m_objectGradient;
//    QColor m_singleHighlightColor;
//    QLinearGradient m_singleHighlightGradient;
//    QColor m_multiHighlightColor;
//    QLinearGradient m_multiHighlightGradient;

private:
    Q_DISABLE_COPY(QAbstract3DSeries)

    friend class Abstract3DController;
    friend class Bars3DController;
    friend class Surface3DController;
    friend class Scatter3DController;
    friend class QBar3DSeries;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

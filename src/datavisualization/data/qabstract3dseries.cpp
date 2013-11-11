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

#include "qabstract3dseries.h"
#include "qabstract3dseries_p.h"
#include "qabstractdataproxy_p.h"
#include "abstract3dcontroller_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QAbstract3DSeries
 * \inmodule QtDataVisualization
 * \brief Base class for all QtDataVisualization data proxies.
 * \since Qt Data Visualization 1.0
 *
 * You use the visualization type specific inherited classes instead of the base class.
 * \sa QBar3DSeries, QScatter3DSeries, QSurface3DSeries, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmltype Abstract3DSeries
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QAbstract3DSeries
 * \brief Base type for all QtDataVisualization data proxies.
 *
 * This type is uncreatable, but contains properties that are exposed via subtypes.
 * \sa Bar3DSeries, Scatter3DSeries, Surface3DSeries, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmlproperty Abstract3DSeries.SeriesType Abstract3DSeries::type
 * The type of the series.
 */

/*!
 * \qmlproperty string Abstract3DSeries::itemLabelFormat
 *
 * Label format for data items in this series. This format is used for single item labels,
 * for example, when an item is selected. How the format is interpreted depends on series type. See
 * each series class documentation for more information.
 */

/*!
 * \qmlproperty bool Abstract3DSeries::visible
 * Sets the visibility of the series. If false, the series is not rendered.
 */

/*!
 * \enum QAbstract3DSeries::SeriesType
 *
 * Type of the series.
 *
 * \value SeriesTypeNone
 *        No series type.
 * \value SeriesTypeBar
 *        Series type for Q3DBars.
 * \value SeriesTypeScatter
 *        Series type for Q3DScatter.
 * \value SeriesTypeSurface
 *        Series type for Q3DSurface.
 */

/*!
 * \internal
 */
QAbstract3DSeries::QAbstract3DSeries(QAbstract3DSeriesPrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/*!
 * Destroys QAbstract3DSeries.
 */
QAbstract3DSeries::~QAbstract3DSeries()
{
}

/*!
 * \property QAbstract3DSeries::type
 *
 * The type of the series.
 */
QAbstract3DSeries::SeriesType QAbstract3DSeries::type() const
{
    return d_ptr->m_type;
}

/*!
 * \property QAbstract3DSeries::itemLabelFormat
 *
 * Sets label \a format for data items in this series. This format is used for single item labels,
 * for example, when an item is selected. How the format is interpreted depends on series type. See
 * each series class documentation for more information.
 *
 * \sa QBar3DSeries, Q3DScatterSeries, Q3DSurfaceSeries
 */
void QAbstract3DSeries::setItemLabelFormat(const QString &format)
{
    if (format != itemLabelFormat()) {
        d_ptr->setItemLabelFormat(format);
        emit itemLabelFormatChanged(format);
    }
}

QString QAbstract3DSeries::itemLabelFormat() const
{
    return d_ptr->m_itemLabelFormat;
}

/*!
 * \property QAbstract3DSeries::visible
 *
 * Sets the visibility of the series. If \a visible is false, the series is not rendered.
 * Defaults to true.
 */
void QAbstract3DSeries::setVisible(bool visible)
{
    d_ptr->m_visible = visible;
    emit visibilityChanged(visible);
}

bool QAbstract3DSeries::isVisible() const
{
    return d_ptr->m_visible;
}

// QAbstract3DSeriesPrivate

QAbstract3DSeriesPrivate::QAbstract3DSeriesPrivate(QAbstract3DSeries *q, QAbstract3DSeries::SeriesType type)
    : QObject(0),
      q_ptr(q),
      m_type(type),
      m_dataProxy(0),
      m_visible(true),
      m_controller(0)
{
}

QAbstract3DSeriesPrivate::~QAbstract3DSeriesPrivate()
{
}

void QAbstract3DSeriesPrivate::setItemLabelFormat(const QString &format)
{
    m_itemLabelFormat = format;
}

QAbstractDataProxy *QAbstract3DSeriesPrivate::dataProxy() const
{
    return m_dataProxy;
}

void QAbstract3DSeriesPrivate::setDataProxy(QAbstractDataProxy *proxy)
{
    Q_ASSERT(proxy && proxy != m_dataProxy && !proxy->d_ptr->series());

    delete m_dataProxy;
    m_dataProxy = proxy;

    proxy->d_ptr->setSeries(q_ptr); // also sets parent

    if (m_controller)
        connectControllerAndProxy(m_controller);
}

void QAbstract3DSeriesPrivate::setController(Abstract3DController *controller)
{
    connectControllerAndProxy(controller);
    m_controller = controller;
    q_ptr->setParent(controller);
}

QT_DATAVISUALIZATION_END_NAMESPACE

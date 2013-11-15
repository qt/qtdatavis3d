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

#include "qscatter3dseries_p.h"
#include "scatter3dcontroller_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QScatter3DSeries
 * \inmodule QtDataVisualization
 * \brief Base series class for Q3DScatter.
 * \since Qt Data Visualization 1.0
 *
 * QScatter3DSeries manages the series specific visual elements, as well as series data
 * (via data proxy).
 *
 * If no data proxy is set explicitly for the series, QScatter3DSeries creates a default
 * proxy. If any other proxy is set as active data proxy later, the default proxy and all data
 * added to it are destroyed.
 *
 * QScatter3DSeries supports the following format tags for QAbstract3DSeries::setItemLabelFormat():
 * \table
 *   \row
 *     \li @xTitle    \li Title from X axis
 *   \row
 *     \li @yTitle    \li Title from Y axis
 *   \row
 *     \li @zTitle    \li Title from Z axis
 *   \row
 *     \li @xLabel    \li Item value formatted using the same format the X axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 *   \row
 *     \li @yLabel    \li Item value formatted using the same format the Y axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 *   \row
 *     \li @zLabel    \li Item value formatted using the same format the Z axis attached to the graph uses,
 *                            see \l{Q3DValueAxis::setLabelFormat()} for more information.
 * \endtable
 *
 * For example:
 * \snippet doc_src_qtdatavisualization.cpp 1
 *
 * \sa {Qt Data Visualization Data Handling}
 */

/*!
 * \qmltype Scatter3DSeries
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QScatter3DSeries
 * \inherits Abstract3DSeries
 * \brief Base series type for Scatter3D.
 *
 * This type  manages the series specific visual elements, as well as series data
 * (via data proxy).
 *
 * For more complete description, see QScatter3DSeries.
 *
 * \sa {Qt Data Visualization Data Handling}
 */


/*!
 * Constructs QScatter3DSeries with the given \a parent.
 */
QScatter3DSeries::QScatter3DSeries(QObject *parent) :
    QAbstract3DSeries(new QScatter3DSeriesPrivate(this), parent)
{
    // Default proxy
    dptr()->setDataProxy(new QScatterDataProxy);
}

QScatter3DSeries::QScatter3DSeries(QScatterDataProxy *dataProxy, QObject *parent) :
    QAbstract3DSeries(new QScatter3DSeriesPrivate(this), parent)
{
    dptr()->setDataProxy(dataProxy);
}

/*!
 * \internal
 */
QScatter3DSeries::QScatter3DSeries(QScatter3DSeriesPrivate *d, QObject *parent) :
    QAbstract3DSeries(d, parent)
{
}

/*!
 * Destroys QScatter3DSeries.
 */
QScatter3DSeries::~QScatter3DSeries()
{
}

/*!
 * \property QScatter3DSeries::dataProxy
 *
 * This property holds the active data \a proxy. The series assumes ownership of any proxy set to
 * it and deletes any previously set proxy when a new one is added. The \a proxy cannot be null or
 * set to another series.
 */
void QScatter3DSeries::setDataProxy(QScatterDataProxy *proxy)
{
    d_ptr->setDataProxy(proxy);
}

QScatterDataProxy *QScatter3DSeries::dataProxy() const
{
    return static_cast<QScatterDataProxy *>(d_ptr->dataProxy());
}

/*!
 * \internal
 */
QScatter3DSeriesPrivate *QScatter3DSeries::dptr()
{
    return static_cast<QScatter3DSeriesPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QScatter3DSeriesPrivate *QScatter3DSeries::dptrc() const
{
    return static_cast<const QScatter3DSeriesPrivate *>(d_ptr.data());
}

// QScatter3DSeriesPrivate

QScatter3DSeriesPrivate::QScatter3DSeriesPrivate(QScatter3DSeries *q)
    : QAbstract3DSeriesPrivate(q, QAbstract3DSeries::SeriesTypeScatter)
{
    m_itemLabelFormat = QStringLiteral("@valueTitle: @valueLabel");
}

QScatter3DSeriesPrivate::~QScatter3DSeriesPrivate()
{
}

QScatter3DSeries *QScatter3DSeriesPrivate::qptr()
{
    return static_cast<QScatter3DSeries *>(q_ptr);
}

void QScatter3DSeriesPrivate::setDataProxy(QAbstractDataProxy *proxy)
{
    Q_ASSERT(proxy->type() == QAbstractDataProxy::DataTypeScatter);

    QAbstract3DSeriesPrivate::setDataProxy(proxy);

    emit qptr()->dataProxyChanged(static_cast<QScatterDataProxy *>(proxy));
}

void QScatter3DSeriesPrivate::connectControllerAndProxy(Abstract3DController *newController)
{
    QScatterDataProxy *scatterDataProxy = static_cast<QScatterDataProxy *>(m_dataProxy);

    if (m_controller && scatterDataProxy) {
        //Disconnect old controller/old proxy
        QObject::disconnect(scatterDataProxy, 0, m_controller, 0);
        QObject::disconnect(q_ptr, 0, m_controller, 0);
    }

    if (newController && scatterDataProxy) {
        Scatter3DController *controller = static_cast<Scatter3DController *>(newController);

        QObject::connect(scatterDataProxy, &QScatterDataProxy::arrayReset,
                         controller, &Scatter3DController::handleArrayReset);
        QObject::connect(scatterDataProxy, &QScatterDataProxy::itemsAdded,
                         controller, &Scatter3DController::handleItemsAdded);
        QObject::connect(scatterDataProxy, &QScatterDataProxy::itemsChanged,
                         controller, &Scatter3DController::handleItemsChanged);
        QObject::connect(scatterDataProxy, &QScatterDataProxy::itemsRemoved,
                         controller, &Scatter3DController::handleItemsRemoved);
        QObject::connect(scatterDataProxy, &QScatterDataProxy::itemsInserted,
                         controller, &Scatter3DController::handleItemsInserted);

        QObject::connect(q_ptr, &QAbstract3DSeries::visibilityChanged, controller,
                         &Abstract3DController::handleSeriesVisibilityChanged);
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE

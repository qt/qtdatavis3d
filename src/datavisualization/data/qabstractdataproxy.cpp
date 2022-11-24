/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qabstractdataproxy_p.h"
#include "qabstract3dseries_p.h"

QT_BEGIN_NAMESPACE

/*!
 * \class QAbstractDataProxy
 * \inmodule QtDataVisualization
 * \brief The QAbstractDataProxy class is a base class for all data
 * visualization data proxies.
 * \since QtDataVisualization 1.0
 *
 * The following visualization type specific inherited classes are used instead
 * of the base class: QBarDataProxy, QScatterDataProxy, and QSurfaceDataProxy.
 *
 * For more information, see \l{Qt Data Visualization Data Handling}.
 */

/*!
 * \qmltype AbstractDataProxy
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QAbstractDataProxy
 * \brief Base type for all QtDataVisualization data proxies.
 *
 * This type is uncreatable, but contains properties that are exposed via the
 * following subtypes: BarDataProxy, ScatterDataProxy, SurfaceDataProxy.
 *
 * For more information, see \l {Qt Data Visualization Data Handling}.
 */

/*!
 * \qmlproperty AbstractDataProxy.DataType AbstractDataProxy::type
 * The type of the proxy. One of the QAbstractDataProxy::DataType values.
 */

/*!
 * \enum QAbstractDataProxy::DataType
 *
 * This enum type specifies the data type of the proxy.
 *
 * \value DataTypeNone
 *        No data type.
 * \value DataTypeBar
 *        Data type for Q3DBars.
 * \value DataTypeScatter
 *        Data type for Q3DScatter.
 * \value DataTypeSurface
 *        Data type for Q3DSurface.
 */

/*!
 * \internal
 */
QAbstractDataProxy::QAbstractDataProxy(QAbstractDataProxyPrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/*!
 * Deletes the abstract data proxy.
 */
QAbstractDataProxy::~QAbstractDataProxy()
{
}

/*!
 * \property QAbstractDataProxy::type
 *
 * \brief The data type of the proxy.
 */
QAbstractDataProxy::DataType QAbstractDataProxy::type() const
{
    return d_ptr->m_type;
}

// QAbstractDataProxyPrivate

QAbstractDataProxyPrivate::QAbstractDataProxyPrivate(QAbstractDataProxy *q,
                                                     QAbstractDataProxy::DataType type)
    : QObject(0),
      q_ptr(q),
      m_type(type),
      m_series(0)
{
}

QAbstractDataProxyPrivate::~QAbstractDataProxyPrivate()
{
}

void QAbstractDataProxyPrivate::setSeries(QAbstract3DSeries *series)
{
    q_ptr->setParent(series);
    m_series = series;
}

QT_END_NAMESPACE

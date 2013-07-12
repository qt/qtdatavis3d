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

#ifndef QITEMMODELMAPDATAPROXY_H
#define QITEMMODELMAPDATAPROXY_H

#include "qmapdataproxy.h"
#include "qitemmodelmapdatamapping.h"
#include <QAbstractItemModel>
#include <QStringList>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QItemModelMapDataProxyPrivate;

class QT_DATAVIS3D_EXPORT QItemModelMapDataProxy : public QMapDataProxy
{
    Q_OBJECT

public:
    explicit QItemModelMapDataProxy();
    explicit QItemModelMapDataProxy(QAbstractItemModel *itemModel, QItemModelMapDataMapping *mapping);
    virtual ~QItemModelMapDataProxy();

    // Doesn't gain ownership of the model, but does connect to it to listen for data changes.
    void setItemModel(QAbstractItemModel *itemModel);
    QAbstractItemModel *itemModel();

    // Map maps role (label, xPos, yPos, value) to role in model
    // Doesn't gain ownership of mapping, but does connect to it to listen for mapping changes.
    // Modifying mapping that is set to proxy will trigger dataset re-resolving.
    void setMapping(QItemModelMapDataMapping *mapping);
    QItemModelMapDataMapping *mapping();

protected:
    QItemModelMapDataProxyPrivate *dptr();

private:
    Q_DISABLE_COPY(QItemModelMapDataProxy)
};

QT_DATAVIS3D_END_NAMESPACE

#endif

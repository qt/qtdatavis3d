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

#ifndef QOLDBARDATAPROXY_H
#define QOLDBARDATAPROXY_H

#include "qbardataproxy.h"
#include "qdataset.h"
#include <QOpenGLFunctions>

// TODO: NOTE: THIS IS A TEMPORARY PROXY CLASS TO HANDLE OLD QDataSet
// TODO: To be removed once QDataSet is booted off the island.
// TODO: Until then, it serves as an example of how to customize proxy
// TODO: Therefore it doesn't implement private class either.

QT_DATAVIS3D_BEGIN_NAMESPACE

class QT_DATAVIS3D_EXPORT QOldDataProxy : public QBarDataProxy
{
    Q_OBJECT

public:
    explicit QOldDataProxy();
    virtual ~QOldDataProxy();

    // Add a row of data. Each new row is added to the front of the sample space, moving previous
    // rows back (if sample space is more than one row deep)
    void addDataRow(const QVector<GLfloat> &dataRow);

    // ownership of dataItems is transferred
    void addDataRow(const QVector<QDataItem*> &dataRow);

    // ownership of dataRow is transferred
    void addDataRow(QDataRow *dataRow);

    // Add complete data set at a time, as a vector of data rows
    void addDataSet(const QVector< QVector<GLfloat> > &data);

    // ownership of dataItems is transferred
    void addDataSet(const QVector< QVector<QDataItem*> > &data);

    // ownership of dataSet is transferred
    void addDataSet(QDataSet* dataSet);

private:
    void addProxyRow(QDataRow *dataRow);
    void addProxySet(QDataSet* dataSet);

    Q_DISABLE_COPY(QOldDataProxy)

    QDataSet *m_dataSet;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // QOLDBARDATAPROXY_H

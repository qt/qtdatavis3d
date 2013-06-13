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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QDATASET_P_H
#define QDATASET_P_H

#include "qdatavis3dglobal.h"
#include "qdataset.h"
#include "drawer_p.h"
#include "labelitem_p.h"
#include <QVector>
#include <QString>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QDataSetPrivate : public QObject
{
    Q_OBJECT

public:
    explicit QDataSetPrivate(QDataSet *q);
    ~QDataSetPrivate();

    void setDrawer(Drawer *drawer);
    QVector<QDataRow*> set();
    QDataRow *getRow(int rowIndex);
    QVector<QString> rowLabels();
    QVector<QString> columnLabels();
    QVector<LabelItem> rowLabelItems();
    QVector<LabelItem> columnLabelItems();
    void axisLabels(QString *xAxis, QString *zAxis, QString *yAxis);
    void axisLabelItems(LabelItem *xAxisItem, LabelItem *zAxisItem, LabelItem *yAxisItem);
    void verifySize(int colSize, int rowSize = 0); // If rowSize is 0, don't verify rows
    // first = min, second = max
    QPair<GLfloat, GLfloat> limitValues();

public Q_SLOTS:
    void updateTextures();

private:
    QDataSet *q_ptr;
    QVector<QDataRow*> m_set;
    QString m_xAxis;
    QString m_zAxis;
    QString m_yAxis;
    QVector<QString> m_labelsRow;
    QVector<QString> m_labelsColumn;
    LabelItem m_xAxisItem;
    LabelItem m_zAxisItem;
    LabelItem m_yAxisItem;
    QVector<LabelItem> m_labelItemsRow;
    QVector<LabelItem> m_labelItemsColumn;
    Drawer *m_drawer;
    friend class QDataSet;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

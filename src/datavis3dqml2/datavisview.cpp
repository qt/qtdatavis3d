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

//#define TEST1
//#define TEST2

#include "datavisview.h"
#include "scenerenderernode_p.h"

#include <QDebug>

#ifdef TEST1
#include "q3dbars.h"
#include <QtQuick/QQuickWindow>
#endif

QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE

#ifdef TEST1
Q3DBars *test_scene;
QQuickWindow *test_window;
#endif

DataVisView::DataVisView(QQuickItem *parent):
    QQuickItem(parent)
{
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()

    setFlag(ItemHasContents, true);
}

DataVisView::~DataVisView()
{
}

QSGNode *DataVisView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
#ifdef TEST1
    static bool done = false;
    if (!done) {
        qDebug() << "create scene";
        test_scene = new Q3DBars();
        //test_scene->initialize();

        // TODO: For testing. Add some data to scene.
        QVector< QVector<float> > data;
        QVector<float> row;
        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 2; i++)
                row.append(1.0f);
            data.append(row);
            row.clear();
        }
        // Set up sample space based on inserted data
        test_scene->setupSampleSpace(2, 2);
        // Add data to chart
        test_scene->addDataSet(data);

        test_window = window();
        QObject::connect(test_window, &QQuickWindow::beforeRendering, test_scene, &Q3DBars::renderNow,
                         Qt::DirectConnection);
        test_window->update();
        done = true;
    }
#elif TEST2
    // Delete old node and recreate it. This function gets called when window geometry changes.
    if (oldNode)
        delete oldNode;

    // We need to create a node class that does the rendering (ie. a node that "captures" the rendering we do)
    SceneRendererNode *node = new SceneRendererNode(window());
    node->setRect(boundingRect());

    return node;
#else
    return NULL;
#endif
}

QTENTERPRISE_DATAVIS3D_END_NAMESPACE

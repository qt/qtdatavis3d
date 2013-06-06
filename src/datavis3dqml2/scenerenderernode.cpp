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

#include "scenerenderernode_p.h"

#include <QtQuick/QQuickWindow>
#include <QtGui/QOpenGLFramebufferObject>
#include "q3dbars.h"
//#include "q3dmaps.h"

QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE

SceneRendererNode::SceneRendererNode(QQuickWindow *window)
    : m_fbo(0),
      m_texture(0),
      m_window(window),
      m_scene(0)
{
    connect(m_window, SIGNAL(beforeRendering()), this, SLOT(render()));
}

SceneRendererNode::~SceneRendererNode()
{
    delete m_texture;
    delete m_fbo;
    delete m_scene;
}

void SceneRendererNode::render()
{
    QSize size = rect().size().toSize();

    if (!m_fbo) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        m_fbo = new QOpenGLFramebufferObject(size, format);
        m_texture = m_window->createTextureFromId(m_fbo->texture(), size);
        // TODO: If we create the vis3d this way, how do we connect it with QML?
        // Should we create it at QML and give it to DataVisView using a property (setVisualizer or similar)?
        // DataVisView can then give it here as an argument in constructor?

        m_scene = new Q3DBars();
        m_scene->setSelectionMode(ModeNone);
        m_scene->setShadowQuality(ShadowNone);
        m_scene->setTheme(ThemeSystem);
        m_scene->initialize();

        // TODO: For testing. Add some data to scene.
        QVector< QVector<float> > data;
        QVector<float> row;
        for (float j = 0.0f; j < 5.0f; j++) {
            for (float i = 0.0f; i < 5.0f; i++)
                row.append(j / 10.0f + i / 10.0f);
            data.append(row);
            row.clear();
        }
        // Set up sample space based on inserted data
        m_scene->setupSampleSpace(5, 5);
        // Add data to chart
        m_scene->addDataSet(data);
        m_scene->setBarType(Cones);

        //m_scene = new Q3DMaps();
        setTexture(m_texture);
    }

    m_fbo->bind();

    // TODO: Render here, or "capture" the rendering we do at Q3DBars/Q3DMaps
    m_scene->render();

    m_fbo->bindDefault();

    m_window->update();
}

QTENTERPRISE_DATAVIS3D_END_NAMESPACE



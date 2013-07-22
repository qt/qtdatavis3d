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

#include "abstractobjecthelper_p.h"

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

AbstractObjectHelper::AbstractObjectHelper()
    : m_vertexbuffer(0),
      m_normalbuffer(0),
      m_uvbuffer(0),
      m_elementbuffer(0),
      m_indexCount(0),
      m_meshDataLoaded(false)
{
    qDebug() << "AbstractObjectHelper::AbstractObjectHelper";
}

AbstractObjectHelper::~AbstractObjectHelper()
{
    qDebug() << "AbstractObjectHelper::~AbstractObjectHelper";
    glDeleteBuffers(1, &m_vertexbuffer);
    glDeleteBuffers(1, &m_uvbuffer);
    glDeleteBuffers(1, &m_normalbuffer);
    glDeleteBuffers(1, &m_elementbuffer);
}

GLuint AbstractObjectHelper::vertexBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_vertexbuffer;
}

GLuint AbstractObjectHelper::normalBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_normalbuffer;
}

GLuint AbstractObjectHelper::uvBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_uvbuffer;
}

GLuint AbstractObjectHelper::elementBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_elementbuffer;
}

GLuint AbstractObjectHelper::indexCount()
{
    return m_indexCount;
}

GLuint AbstractObjectHelper::indicesType()
{
    return m_indicesType;
}

QT_DATAVIS3D_END_NAMESPACE

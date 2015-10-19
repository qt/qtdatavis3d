/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include "labelitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

LabelItem::LabelItem()
    : m_size(QSize(0, 0)),
      m_textureId(0)
{
}

LabelItem::~LabelItem()
{
    clear();
}

void LabelItem::setSize(const QSize &size)
{
    m_size = size;
}

QSize LabelItem::size() const
{
    return m_size;
}

void LabelItem::setTextureId(GLuint textureId)
{
    QOpenGLContext::currentContext()->functions()->glDeleteTextures(1, &m_textureId);
    m_textureId = textureId;
}

GLuint LabelItem::textureId() const
{
    return m_textureId;
}

void LabelItem::clear()
{
    if (m_textureId && QOpenGLContext::currentContext())
        QOpenGLContext::currentContext()->functions()->glDeleteTextures(1, &m_textureId);
    m_textureId = 0;
    m_size = QSize(0, 0);
}

QT_END_NAMESPACE_DATAVISUALIZATION

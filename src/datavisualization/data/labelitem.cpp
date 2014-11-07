/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

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

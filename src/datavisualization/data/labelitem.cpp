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

#include "labelitem_p.h"

namespace QtDataVisualization {

LabelItem::LabelItem()
    : m_size(QSize(0, 0)),
      m_textureId(0)
{
}

LabelItem::~LabelItem()
{
    glDeleteTextures(1, &m_textureId);
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
    glDeleteTextures(1, &m_textureId);
    m_textureId = textureId;
}

GLuint LabelItem::textureId() const
{
    return m_textureId;
}

void LabelItem::clear()
{
    if (m_textureId) {
        glDeleteTextures(1, &m_textureId);
        m_textureId = 0;
    }
    m_size = QSize(0, 0);
}

}

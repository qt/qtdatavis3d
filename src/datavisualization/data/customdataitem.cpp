/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#include "customdataitem_p.h"
#include "objecthelper_p.h"
#include "texturehelper_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

CustomDataItem::CustomDataItem() :
    m_textureHelper(0),
    m_texture(0)
{
    m_textureHelper = new TextureHelper();
}

CustomDataItem::~CustomDataItem()
{
    m_textureHelper->deleteTexture(&m_texture);
    delete m_textureHelper;
}

void CustomDataItem::setTextureImage(const QImage &textureImage)
{
    m_textureHelper->deleteTexture(&m_texture);

    // Make a texture out of the image
    if (!textureImage.isNull())
        m_texture = m_textureHelper->create2DTexture(textureImage, true, true, true);
}

QT_END_NAMESPACE_DATAVISUALIZATION

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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef LABELITEM_P_H
#define LABELITEM_P_H

#include "datavisualizationglobal_p.h"
#include <QOpenGLFunctions>
#include <QSize>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class LabelItem
{
public:
    explicit LabelItem();
    ~LabelItem();

    void setSize(const QSize &size);
    QSize size() const;
    void setTextureId(GLuint textureId);
    GLuint textureId() const;
    void clear();

private:
    Q_DISABLE_COPY(LabelItem)

    QSize m_size;
    GLuint m_textureId;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

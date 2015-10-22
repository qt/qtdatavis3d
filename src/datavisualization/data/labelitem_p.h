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
#include <QtCore/QSize>

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

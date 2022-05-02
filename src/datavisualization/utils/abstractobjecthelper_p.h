/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
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

#ifndef ABSTRACTOBJECTHELPER_H
#define ABSTRACTOBJECTHELPER_H

#include "datavisualizationglobal_p.h"

QT_BEGIN_NAMESPACE

class AbstractObjectHelper: protected QOpenGLFunctions
{
protected:
    AbstractObjectHelper();
public:
    virtual ~AbstractObjectHelper();

    GLuint vertexBuf();
    GLuint normalBuf();
    virtual GLuint uvBuf();
    GLuint elementBuf();
    GLuint indexCount();

public:
    GLuint m_vertexbuffer;
    GLuint m_normalbuffer;
    GLuint m_uvbuffer;
    GLuint m_elementbuffer;

    GLuint m_indexCount;
    GLboolean m_meshDataLoaded;
};

QT_END_NAMESPACE

#endif

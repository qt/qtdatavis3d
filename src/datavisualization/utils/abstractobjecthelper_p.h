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
#include <QOpenGLFunctions>

namespace QtDataVisualization {

class AbstractObjectHelper: protected QOpenGLFunctions
{
protected:
    AbstractObjectHelper();
public:
    ~AbstractObjectHelper();

    GLuint vertexBuf();
    GLuint normalBuf();
    GLuint uvBuf();
    GLuint elementBuf();
    GLuint indexCount();
    GLuint indicesType();

public:
    GLuint m_vertexbuffer;
    GLuint m_normalbuffer;
    GLuint m_uvbuffer;
    GLuint m_elementbuffer;

    GLuint m_indexCount;
    GLboolean m_meshDataLoaded;

    GLuint m_indicesType;
};

}

#endif // ABSTRACTOBJECTHELPER_H

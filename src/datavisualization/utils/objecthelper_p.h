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

#ifndef OBJECTHELPER_P_H
#define OBJECTHELPER_P_H

#include "datavisualizationglobal_p.h"
#include "abstractobjecthelper_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Abstract3DRenderer;

class ObjectHelper : public AbstractObjectHelper
{
private:
    ObjectHelper(const QString &objectFile);
public:
    virtual ~ObjectHelper();

    static void resetObjectHelper(const Abstract3DRenderer *cacheId, ObjectHelper *&obj,
                                  const QString &meshFile);
    static void releaseObjectHelper(const Abstract3DRenderer *cacheId, ObjectHelper *&obj);
    inline const QString &objectFile() { return m_objectFile; }

    inline const QVector<GLuint> &indices() const { return m_indices; }
    inline const QVector<QVector3D> &indexedvertices() const { return m_indexedVertices; }
    inline const QVector<QVector2D> &indexedUVs() const { return m_indexedUVs; }
    inline const QVector<QVector3D> &indexedNormals() const { return m_indexedNormals; }

private:
    static ObjectHelper *getObjectHelper(const Abstract3DRenderer *cacheId,
                                         const QString &objectFile);
    void load();

    QString m_objectFile;
    QVector<GLuint> m_indices;
    QVector<QVector3D> m_indexedVertices;
    QVector<QVector2D> m_indexedUVs;
    QVector<QVector3D> m_indexedNormals;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

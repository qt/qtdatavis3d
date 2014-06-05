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
    ~ObjectHelper();

    static void resetObjectHelper(const Abstract3DRenderer *cacheId, ObjectHelper *&obj,
                                  const QString &meshFile);
    static void releaseObjectHelper(const Abstract3DRenderer *cacheId, ObjectHelper *&obj);
    inline const QString &objectFile() { return m_objectFile; }

    inline const QVector<unsigned short> &indices() const { return m_indices; }
    inline const QVector<QVector3D> &indexedvertices() const { return m_indexedVertices; }
    inline const QVector<QVector2D> &indexedUVs() const { return m_indexedUVs; }
    inline const QVector<QVector3D> &indexedNormals() const { return m_indexedNormals; }

private:
    static ObjectHelper *getObjectHelper(const Abstract3DRenderer *cacheId,
                                         const QString &objectFile);
    void load();

    QString m_objectFile;
    QVector<unsigned short> m_indices;
    QVector<QVector3D> m_indexedVertices;
    QVector<QVector2D> m_indexedUVs;
    QVector<QVector3D> m_indexedNormals;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

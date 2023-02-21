// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtGraphs API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef SURFACESELECTIONINSTANCING_H
#define SURFACESELECTIONINSTANCING_H

#include <QtQuick3D/private/qquick3dinstancing_p.h>

QT_BEGIN_NAMESPACE

class SurfaceSelectionInstancing : public QQuick3DInstancing
{
    Q_OBJECT

public:
    SurfaceSelectionInstancing();
    ~SurfaceSelectionInstancing();

    void setRotation(const QVector3D &rotation) { m_rotation = rotation; }
    void setScale(const QVector3D &scale) { m_scale = scale; }
    void addPosition(const QVector3D &position);
    void resetPositions();
    void setColor(const QColor &color) { m_color = color; }

protected:
    QByteArray getInstanceBuffer(int *instanceCount) override;

private:
    QByteArray m_instanceData;
    int m_instanceCount = 0;
    QVector3D m_rotation = {.0f, .0f, .0f};
    QVector3D m_scale = {.0f, .0f, .0f};
    QList<QVector3D> m_positions;
    QColor m_color = {0, 0, 0};
    bool m_dirty = true;
};

QT_END_NAMESPACE

#endif // SURFACESELECTIONINSTANCING_H

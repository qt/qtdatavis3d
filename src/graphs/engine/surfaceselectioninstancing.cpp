// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "surfaceselectioninstancing_p.h"

SurfaceSelectionInstancing::SurfaceSelectionInstancing()
{
}

SurfaceSelectionInstancing::~SurfaceSelectionInstancing()
{
}

void SurfaceSelectionInstancing::addPosition(const QVector3D &position)
{
    m_positions.append(position);
    markDirty();
    m_dirty = true;
}

void SurfaceSelectionInstancing::resetPositions()
{
    m_positions.clear();
    markDirty();
    m_dirty = true;
}

QByteArray SurfaceSelectionInstancing::getInstanceBuffer(int *instanceCount)
{
    if (m_dirty) {
        m_instanceData.resize(0);
        int instanceNumber = 0;

        for (auto position : m_positions) {
            auto entry = calculateTableEntry(position, m_scale, m_rotation, m_color);
            m_instanceData.append(reinterpret_cast<const char*>(&entry), sizeof(entry));
            instanceNumber++;
        }
        m_instanceCount = instanceNumber;
        m_dirty = false;
    }

    if (instanceCount)
        *instanceCount = m_instanceCount;

    return m_instanceData;
}

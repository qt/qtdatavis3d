// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "scatterinstancing_p.h"

ScatterInstancing::ScatterInstancing()
{

}

QByteArray ScatterInstancing::getInstanceBuffer(int *instanceCount) {
    if (m_dirty) {
        m_instanceData.resize(0);
        int instanceNumber = 0;

        for (int i = 0; i < m_dataArray.size(); ++i) {
            auto item = m_dataArray.at(i);
            float x = item.position.x();
            float y = item.position.y();
            float z = item.position.z();
            QVector4D customData{};
            if (m_rangeGradient)
                customData.setX(m_customData.at(i));
            auto entry = calculateTableEntryFromQuaternion({x,y,z}, item.scale, item.rotation, QColor(Qt::white), customData);
            m_instanceData.append(reinterpret_cast<char *>(&entry), sizeof(entry));
            instanceNumber++;
        }
        m_instanceCount = instanceNumber;
        m_dirty = false;
    }

    if (instanceCount)
        *instanceCount = m_instanceCount;

    return m_instanceData;
}

bool ScatterInstancing::rangeGradient() const
{
    return m_rangeGradient;
}

void ScatterInstancing::setRangeGradient(bool newRangeGradient)
{
    m_rangeGradient = newRangeGradient;
}

QList<float> &ScatterInstancing::customData()
{
    return m_customData;
}

void ScatterInstancing::setCustomData(const QList<float> &newCustomData)
{
    m_customData = newCustomData;
    markDataDirty();
}

void ScatterInstancing::markDataDirty()
{
    m_dirty = true;
    markDirty();
}

const QList<DataItemHolder> &ScatterInstancing::dataArray() const
{
    return m_dataArray;
}

void ScatterInstancing::setDataArray(const QList<DataItemHolder> &newDataArray)
{
    m_dataArray = newDataArray;
    markDataDirty();
}

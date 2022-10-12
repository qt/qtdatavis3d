// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "datavisquick3dinstancing_p.h"

DatavisQuick3DInstancing::DatavisQuick3DInstancing()
{

}

QByteArray DatavisQuick3DInstancing::getInstanceBuffer(int *instanceCount) {
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

bool DatavisQuick3DInstancing::rangeGradient() const
{
    return m_rangeGradient;
}

void DatavisQuick3DInstancing::setRangeGradient(bool newRangeGradient)
{
    m_rangeGradient = newRangeGradient;
}

QList<float> &DatavisQuick3DInstancing::customData()
{
    return m_customData;
}

void DatavisQuick3DInstancing::setCustomData(const QList<float> &newCustomData)
{
    m_customData = newCustomData;
    markDataDirty();
}

void DatavisQuick3DInstancing::markDataDirty()
{
    m_dirty = true;
    markDirty();
}

const QList<DataItemHolder> &DatavisQuick3DInstancing::dataArray() const
{
    return m_dataArray;
}

void DatavisQuick3DInstancing::setDataArray(const QList<DataItemHolder> &newDataArray)
{
    m_dataArray = newDataArray;
    markDataDirty();
}

// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DATAVISQUICK3DINSTANCING_H
#define DATAVISQUICK3DINSTANCING_H
#include <QtQuick3D/private/qquick3dinstancing_p.h>
struct DataItemHolder {
    QVector3D position;
    QQuaternion rotation;
    QVector3D scale;
};

class DatavisQuick3DInstancing : public QQuick3DInstancing
{
    Q_OBJECT
public:
    DatavisQuick3DInstancing();

    const QList<DataItemHolder> &dataArray() const;
    void setDataArray(const QList<DataItemHolder> &newDataArray);

    QList<float> &customData();
    void setCustomData(const QList<float> &newCustomData);

    void markDataDirty();
    bool rangeGradient() const;
    void setRangeGradient(bool newRangeGradient);

    // QQuick3DInstancing interface

protected:
    QByteArray getInstanceBuffer(int *instanceCount) override;
private:
    QByteArray m_instanceData;
    QList<DataItemHolder> m_dataArray;
    QList<float> m_customData;
    int m_instanceCount = 0;
    bool m_dirty = true;
    bool m_rangeGradient = false;

    friend class ScatterSeriesVisualizer;
    friend class QQuickBarSeriesVisualizer;
};

#endif // DATAVISQUICK3DINSTANCING_H
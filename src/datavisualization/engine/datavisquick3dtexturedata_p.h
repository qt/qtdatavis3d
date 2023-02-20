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

#ifndef DATAVISQUICK3DTEXTUREDATA_P_H
#define DATAVISQUICK3DTEXTUREDATA_P_H
#include <QtQuick3D/qquick3dtexturedata.h>
#include <QList>
#include <QLinearGradient>

class DatavisQuick3DTextureData : public QQuick3DTextureData
{
    Q_OBJECT

public:
    DatavisQuick3DTextureData();
    ~DatavisQuick3DTextureData();

    void createGradient(const QLinearGradient &gradient);

private:
    int m_width =  256;
    int m_height = 256;

    QColor linearInterpolate(QColor startColor, QColor endColor, float value);

};

#endif // DATAVISQUICK3DTEXTUREDATA_P_H

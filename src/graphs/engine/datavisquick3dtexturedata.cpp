// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "datavisquick3dtexturedata_p.h"

DatavisQuick3DTextureData::DatavisQuick3DTextureData()
{
}

DatavisQuick3DTextureData::~DatavisQuick3DTextureData()
{
}

void DatavisQuick3DTextureData::createGradient(const QLinearGradient &gradient)
{
    setSize(QSize(m_width, m_height));
    setFormat(QQuick3DTextureData::RGBA8);
    setHasTransparency(false);

    QByteArray imageData;

    QByteArray gradientScanline;
    gradientScanline.resize(m_width * 4); // RGBA8
    auto stops = gradient.stops();

    int x = 0;
    for (int i = 1; i < stops.size(); i++) {
        QColor startColor = stops.at(i - 1).second;
        QColor endColor = stops.at(i).second;
        int w = 0;
        w = (stops.at(i).first - stops.at(i - 1).first) * m_width;
        for (int t = 0; t <= w; t++) {
            QColor color = linearInterpolate(startColor, endColor, t / float(w));
            int offset = x * 4;
            gradientScanline.data()[offset + 0] = char(color.red());
            gradientScanline.data()[offset + 1] = char(color.green());
            gradientScanline.data()[offset + 2] = char(color.blue());
            gradientScanline.data()[offset + 3] = char(255);
            x++;
        }
    }
    for (int y = 0; y < m_height; y++)
        imageData += gradientScanline;
    setTextureData(imageData);
}

QColor DatavisQuick3DTextureData::linearInterpolate(QColor startColor, QColor endColor, float value)
{
    QColor output;

    output.setRedF(startColor.redF() + (value * (endColor.redF() - startColor.redF())));
    output.setGreenF(startColor.greenF() + (value * (endColor.greenF() - startColor.greenF())));
    output.setBlueF(startColor.blueF() + (value * (endColor.blueF() - startColor.blueF())));

    return output;
}

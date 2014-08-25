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

#ifndef QCUSTOM3DVOLUME_H
#define QCUSTOM3DVOLUME_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtDataVisualization/QCustom3DItem>
#include <QtGui/QColor>
#include <QtGui/QImage>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QCustom3DVolumePrivate;

class QT_DATAVISUALIZATION_EXPORT QCustom3DVolume : public QCustom3DItem
{
    Q_OBJECT
    Q_PROPERTY(int textureWidth READ textureWidth WRITE setTextureWidth NOTIFY textureWidthChanged)
    Q_PROPERTY(int textureHeight READ textureHeight WRITE setTextureHeight NOTIFY textureHeightChanged)
    Q_PROPERTY(int textureDepth READ textureDepth WRITE setTextureDepth NOTIFY textureDepthChanged)
    Q_PROPERTY(int sliceIndexX READ sliceIndexX WRITE setSliceIndexX NOTIFY sliceIndexXChanged)
    Q_PROPERTY(int sliceIndexY READ sliceIndexY WRITE setSliceIndexY NOTIFY sliceIndexYChanged)
    Q_PROPERTY(int sliceIndexZ READ sliceIndexZ WRITE setSliceIndexZ NOTIFY sliceIndexZChanged)
    Q_PROPERTY(QVector<QRgb> colorTable READ colorTable WRITE setColorTable NOTIFY colorTableChanged)
    Q_PROPERTY(QVector<uchar> *textureData READ textureData WRITE setTextureData NOTIFY textureDataChanged)
    Q_PROPERTY(QImage::Format textureFormat READ textureFormat WRITE setTextureFormat NOTIFY textureFormatChanged)
    Q_PROPERTY(float alphaMultiplier READ alphaMultiplier WRITE setAlphaMultiplier NOTIFY alphaMultiplierChanged)
    Q_PROPERTY(bool preserveOpacity READ preserveOpacity WRITE setPreserveOpacity NOTIFY preserveOpacityChanged)
public:

    explicit QCustom3DVolume(QObject *parent = 0);
    explicit QCustom3DVolume(const QVector3D &position, const QVector3D &scaling,
                             const QQuaternion &rotation, int textureWidth,
                             int textureHeight, int textureDepth,
                             QVector<uchar> *textureData, QImage::Format textureFormat,
                             const QVector<QRgb> &colorTable, QObject *parent = 0);
    virtual ~QCustom3DVolume();

    void setTextureWidth(int value);
    int textureWidth() const;
    void setTextureHeight(int value);
    int textureHeight() const;
    void setTextureDepth(int value);
    int textureDepth() const;
    void setTextureDimensions(int width, int height, int depth);
    int textureDataWidth() const;

    void setSliceIndexX(int value);
    int sliceIndexX() const;
    void setSliceIndexY(int value);
    int sliceIndexY() const;
    void setSliceIndexZ(int value);
    int sliceIndexZ() const;
    void setSliceIndices(int x, int y, int z);

    void setColorTable(const QVector<QRgb> &colors);
    QVector<QRgb> colorTable() const;

    void setTextureData(QVector<uchar> *data);
    QVector<uchar> *createTextureData(const QVector<QImage *> &images);
    QVector<uchar> *textureData() const;
    void setSubTextureData(int depthIndex, const uchar *data);
    void setSubTextureData(int depthIndex, const QImage &image);

    void setTextureFormat(QImage::Format format);
    QImage::Format textureFormat() const;

    void setAlphaMultiplier(float mult);
    float alphaMultiplier() const;
    void setPreserveOpacity(bool enable);
    bool preserveOpacity() const;

    QImage renderSlice(Qt::Axis axis, int index);

signals:
    void textureWidthChanged(int value);
    void textureHeightChanged(int value);
    void textureDepthChanged(int value);
    void sliceIndexXChanged(int value);
    void sliceIndexYChanged(int value);
    void sliceIndexZChanged(int value);
    void colorTableChanged();
    void textureDataChanged(QVector<uchar> *data);
    void textureFormatChanged(QImage::Format format);
    void alphaMultiplierChanged(float mult);
    void preserveOpacityChanged(bool enabled);

protected:
    QCustom3DVolumePrivate *dptr();
    const QCustom3DVolumePrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QCustom3DVolume)

    friend class Abstract3DRenderer;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

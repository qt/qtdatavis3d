/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

#include "qheightmapsurfacedataproxy_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QHeightMapSurfaceDataProxy
 * \inmodule QtDataVisualization
 * \brief Proxy class for Q3DSurface.
 * \since 1.0.0
 *
 * QHeightMapSurfaceDataProxy takes care of surface related height map data handling. It provides a
 * way for giving the surface plot a height map to be visualized.
 *
 * \sa QSurfaceDataProxy
 */

/*!
 * \qmltype HeightMapSurfaceDataProxy
 * \instantiates QHeightMapSurfaceDataProxy
 * \inherits SurfaceDataProxy
 *
 * HeightMapSurfaceDataProxy takes care of surface related height map data handling. It provides a
 * way for giving the surface plot a height map to be visualized.
 */

/*!
 * \qmlproperty image HeightMapSurfaceDataProxy::heightMap
 *
 * A height map to be visualized. Setting this property replaces current data with height map data.
 *
 * There are several formats the image can be given in, but if it is not in a directly usable
 * format, a conversion is made. \note If the result seems wrong, the automatic conversion failed
 * and you should try converting the image yourself before setting it. Preferred format is
 * QImage::Format_RGB32 in grayscale.
 *
 * The height of the image is read from the red component of the pixels if the image is in grayscale,
 * otherwise it is an average calculated from red, green and blue components of the pixels. Using
 * grayscale images may improve data conversion speed for large images.
 *
 * Not recommended formats: all mono formats (for example QImage::Format_Mono).
 */

/*!
 * \qmlproperty string HeightMapSurfaceDataProxy::heightMapFile
 *
 * A file with a height map image to be visualized. Setting this property replaces current data
 * with height map data.
 *
 * \sa heightMap
 */

/*!
 * Constructs QHeightMapSurfaceDataProxy with the given \a parent.
 */
QHeightMapSurfaceDataProxy::QHeightMapSurfaceDataProxy(QObject *parent) :
    QSurfaceDataProxy(new QHeightMapSurfaceDataProxyPrivate(this), parent)
{
}

/*!
 * Constructs QHeightMapSurfaceDataProxy with the given \a image and \a parent. Height map is set
 * by calling setHeighMap() with \a image.
 *
 * \sa heightMap
 */
QHeightMapSurfaceDataProxy::QHeightMapSurfaceDataProxy(const QImage &image, QObject *parent) :
    QSurfaceDataProxy(new QHeightMapSurfaceDataProxyPrivate(this), parent)
{
    setHeightMap(image);
}

/*!
 * \internal
 */
QHeightMapSurfaceDataProxy::QHeightMapSurfaceDataProxy(
        QHeightMapSurfaceDataProxyPrivate *d, QObject *parent) :
    QSurfaceDataProxy(d, parent)
{
}

/*!
 * Destroys QHeightMapSurfaceDataProxy.
 */
QHeightMapSurfaceDataProxy::~QHeightMapSurfaceDataProxy()
{
}

/*!
 * \property QHeightMapSurfaceDataProxy::heightMap
 *
 * A height map to be visualized. Setting this property replaces current data with height map data.
 *
 * There are several formats the image can be given in, but if it is not in a directly usable
 * format, a conversion is made. \note If the result seems wrong, the automatic conversion failed
 * and you should try converting the image yourself before setting it. Preferred format is
 * QImage::Format_RGB32 in grayscale.
 *
 * The height of the image is read from the red component of the pixels if the image is in grayscale,
 * otherwise it is an average calculated from red, green and blue components of the pixels. Using
 * grayscale images may improve data conversion speed for large images.
 *
 * Not recommended formats: all mono formats (for example QImage::Format_Mono).
 *
 * The height map is resolved asynchronously. QSurfaceDataProxy::arrayReset() is emitted when the
 * data has been resolved.
 */
void QHeightMapSurfaceDataProxy::setHeightMap(const QImage &image)
{
    dptr()->m_heightMap = image;

    // We do resolving asynchronously to make qml onArrayReset handlers actually get the initial reset
    if (!dptr()->m_resolveTimer.isActive())
        dptr()->m_resolveTimer.start(0);
}

QImage QHeightMapSurfaceDataProxy::heightMap() const
{
    return dptrc()->m_heightMap;
}

/*!
 * \property QHeightMapSurfaceDataProxy::heightMapFile
 *
 * A file with a height map image to be visualized. Setting this property replaces current data
 * with height map data.
 *
 * \sa heightMap
 */
void QHeightMapSurfaceDataProxy::setHeightMapFile(const QString &filename)
{
    dptr()->m_heightMapFile = filename;
    setHeightMap(QImage(filename));
}

QString QHeightMapSurfaceDataProxy::heightMapFile() const
{
    return dptrc()->m_heightMapFile;
}
/*!
 * \internal
 */
QHeightMapSurfaceDataProxyPrivate *QHeightMapSurfaceDataProxy::dptr()
{
    return static_cast<QHeightMapSurfaceDataProxyPrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QHeightMapSurfaceDataProxyPrivate *QHeightMapSurfaceDataProxy::dptrc() const
{
    return static_cast<const QHeightMapSurfaceDataProxyPrivate *>(d_ptr.data());
}

//
//  QHeightMapSurfaceDataProxyPrivate
//

QHeightMapSurfaceDataProxyPrivate::QHeightMapSurfaceDataProxyPrivate(QHeightMapSurfaceDataProxy *q)
    : QSurfaceDataProxyPrivate(q)
{
    m_resolveTimer.setSingleShot(true);
    QObject::connect(&m_resolveTimer, &QTimer::timeout,
                     this, &QHeightMapSurfaceDataProxyPrivate::handlePendingResolve);
}

QHeightMapSurfaceDataProxyPrivate::~QHeightMapSurfaceDataProxyPrivate()
{
}

QHeightMapSurfaceDataProxy *QHeightMapSurfaceDataProxyPrivate::qptr()
{
    return static_cast<QHeightMapSurfaceDataProxy *>(q_ptr);
}

void QHeightMapSurfaceDataProxyPrivate::handlePendingResolve()
{
    QImage heightImage = m_heightMap;
    // Convert to RGB32 to be sure we're reading the right bytes
    if (heightImage.format() != QImage::Format_RGB32)
        heightImage = heightImage.convertToFormat(QImage::Format_RGB32);

    uchar *bits = heightImage.bits();

    int imageHeight = heightImage.height();
    int imageWidth = heightImage.width();
    int bitCount = imageWidth * 4 * (imageHeight - 1);
    int widthBits = imageWidth * 4;
    qreal height = 0;

    // Do not recreate array if dimensions have not changed
    QSurfaceDataArray *dataArray = m_dataArray;
    if (imageWidth != qptr()->columnCount() || imageHeight != dataArray->size()) {
        dataArray = new QSurfaceDataArray;
        dataArray->reserve(imageHeight);
        for (int i = 0; i < imageHeight; i++) {
            QSurfaceDataRow *newProxyRow = new QSurfaceDataRow(imageWidth);
            dataArray->append(newProxyRow);
        }
    }

    if (heightImage.isGrayscale()) {
        // Grayscale, it's enough to read Red byte
        for (int i = 0; i < imageHeight; i++, bitCount -= widthBits) {
            QSurfaceDataRow &newRow = *dataArray->at(i);
            for (int j = 0; j < imageWidth; j++)
                newRow[j] = qreal(bits[bitCount + (j * 4)]);
        }
    } else {
        // Not grayscale, we'll need to calculate height from RGB
        for (int i = 0; i < imageHeight; i++, bitCount -= widthBits) {
            QSurfaceDataRow &newRow = *dataArray->at(i);
            for (int j = 0; j < imageWidth; j++) {
                int nextpixel = j * 4;
                height = (qreal(bits[bitCount + nextpixel])
                        + qreal(bits[1 + bitCount + nextpixel])
                        + qreal(bits[2 + bitCount + nextpixel]));
                newRow[j] = height / 3.0;
            }
        }
    }

    qptr()->resetArray(dataArray, 0.0, imageHeight, 0.0, imageWidth);
}

QT_DATAVISUALIZATION_END_NAMESPACE

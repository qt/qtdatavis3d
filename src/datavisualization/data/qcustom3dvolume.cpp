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

#include "qcustom3dvolume_p.h"
#include "utils_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QCustom3DVolume
 * \inmodule QtDataVisualization
 * \brief The QCustom3DVolume class is for creating volume rendered objects to be added to a graph.
 * \since QtDataVisualization 1.2
 *
 * This class is for creating volume rendered objects to be added to a graph. A volume rendered
 * object is a box with a 3D texture. Three slice planes are supported for the volume, one along
 * each main axis of the volume.
 *
 * \note Volumetric objects are only supported with orthographic projection.
 *
 * \sa QAbstract3DGraph::addCustomItem(), QAbstract3DGraph::orthoProjection
 */

/*!
 * \qmltype Custom3DVolume
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.2
 * \ingroup datavisualization_qml
 * \instantiates QCustom3DVolume
 * \brief The Custom3DVolume type is for creating volume rendered objects to be added to a graph.
 *
 * This class is for creating volume rendered objects to be added to a graph. A volume rendered
 * object is a box with a 3D texture. Three slice planes are supported for the volume, one along
 * each main axis of the volume.
 *
 * \note: Filling in the volume data would not typically be efficient or practical from pure QML,
 * so properties directly related to that are not fully supported from QML.
 * Make a hybrid QML/C++ application if you want to use volume objects with QML ui.
 *
 * \note Volumetric objects are only supported with orthographic projection.
 *
 * \sa AbstractGraph3D::orthoProjection
 */

/*! \qmlproperty int Custom3DVolume::textureWidth
 *
 * The width of the 3D texture defining the volume content in pixels. Defaults to \c{0}.
 *
 * \note: Changing this property from QML is not supported, as the texture data cannot be resized
 * to match.
 */

/*! \qmlproperty int Custom3DVolume::textureHeight
 *
 * The height of the 3D texture defining the volume content in pixels. Defaults to \c{0}.
 *
 * \note: Changing this property from QML is not supported, as the texture data cannot be resized
 * to match.
 */

/*! \qmlproperty int Custom3DVolume::textureDepth
 *
 * The depth of the 3D texture defining the volume content in pixels. Defaults to \c{0}.
 *
 * \note: Changing this property from QML is not supported, as the texture data cannot be resized
 * to match.
 */

/*! \qmlproperty int Custom3DVolume::sliceIndexX
 *
 * The X dimension index into the texture data indicating which vertical slice to show.
 * Setting any dimension to negative indicates no slice for that dimension is drawn.
 * If all dimensions are negative, no slices are drawn and the volume is drawn normally.
 * Defaults to \c{-1}.
 *
 * \sa QCustom3DVolume::textureData
 */

/*! \qmlproperty int Custom3DVolume::sliceIndexY
 *
 * The Y dimension index into the texture data indicating which horizontal slice to show.
 * Setting any dimension to negative indicates no slice for that dimension is drawn.
 * If all dimensions are negative, no slices are drawn and the volume is drawn normally.
 * Defaults to \c{-1}.
 *
 * \sa QCustom3DVolume::textureData
 */

/*! \qmlproperty int Custom3DVolume::sliceIndexZ
 *
 * The Z dimension index into the texture data indicating which vertical slice to show.
 * Setting any dimension to negative indicates no slice for that dimension is drawn.
 * If all dimensions are negative, no slices are drawn and the volume is drawn normally.
 * Defaults to \c{-1}.
 *
 * \sa QCustom3DVolume::textureData
 */

/*!
 * Constructs QCustom3DVolume with given \a parent.
 */
QCustom3DVolume::QCustom3DVolume(QObject *parent) :
    QCustom3DItem(new QCustom3DVolumePrivate(this), parent)
{
}

/*!
 * Constructs QCustom3DVolume with given \a position, \a scaling, \a rotation,
 * \a textureWidth, \a textureHeight, \a textureDepth, \a textureData, \a textureFormat,
 * \a colorTable, and optional \a parent.
 *
 * \sa textureData, textureFormat, colorTable
 */
QCustom3DVolume::QCustom3DVolume(const QVector3D &position, const QVector3D &scaling,
                                 const QQuaternion &rotation, int textureWidth,
                                 int textureHeight, int textureDepth,
                                 QVector<uchar> *textureData, QImage::Format textureFormat,
                                 const QVector<QRgb> &colorTable, QObject *parent) :
    QCustom3DItem(new QCustom3DVolumePrivate(this, position, scaling, rotation, textureWidth,
                                             textureHeight, textureDepth,
                                             textureData, textureFormat, colorTable), parent)
{
}


/*!
 * Destroys QCustom3DVolume.
 */
QCustom3DVolume::~QCustom3DVolume()
{
}

/*! \property QCustom3DVolume::textureWidth
 *
 * The width of the 3D texture defining the volume content in pixels. Defaults to \c{0}.
 *
 * \note The textureData may need to be resized or recreated if this value is changed.
 * Defaults to \c{0}.
 *
 * \sa textureData, textureHeight, textureDepth, textureFormat, textureDataWidth()
 */
void QCustom3DVolume::setTextureWidth(int value)
{
    if (value >= 0) {
        if (dptr()->m_textureWidth != value) {
            dptr()->m_textureWidth = value;
            dptr()->m_dirtyBitsVolume.textureDimensionsDirty = true;
            emit textureWidthChanged(value);
            emit dptr()->needUpdate();
        }
    } else {
        qWarning() << __FUNCTION__ << "Cannot set negative value.";
    }
}

int QCustom3DVolume::textureWidth() const
{
    return dptrc()->m_textureWidth;
}

/*! \property QCustom3DVolume::textureHeight
 *
 * The height of the 3D texture defining the volume content in pixels. Defaults to \c{0}.
 *
 * \note The textureData may need to be resized or recreated if this value is changed.
 * Defaults to \c{0}.
 *
 * \sa textureData, textureWidth, textureDepth, textureFormat
 */
void QCustom3DVolume::setTextureHeight(int value)
{
    if (value >= 0) {
        if (dptr()->m_textureHeight != value) {
            dptr()->m_textureHeight = value;
            dptr()->m_dirtyBitsVolume.textureDimensionsDirty = true;
            emit textureHeightChanged(value);
            emit dptr()->needUpdate();
        }
    } else {
        qWarning() << __FUNCTION__ << "Cannot set negative value.";
    }

}

int QCustom3DVolume::textureHeight() const
{
    return dptrc()->m_textureHeight;
}

/*! \property QCustom3DVolume::textureDepth
 *
 * The depth of the 3D texture defining the volume content in pixels. Defaults to \c{0}.
 *
 * \note The textureData may need to be resized or recreated if this value is changed.
 * Defaults to \c{0}.
 *
 * \sa textureData, textureWidth, textureHeight, textureFormat
 */
void QCustom3DVolume::setTextureDepth(int value)
{
    if (value >= 0) {
        if (dptr()->m_textureDepth != value) {
            dptr()->m_textureDepth = value;
            dptr()->m_dirtyBitsVolume.textureDimensionsDirty = true;
            emit textureDepthChanged(value);
            emit dptr()->needUpdate();
        }
    } else {
        qWarning() << __FUNCTION__ << "Cannot set negative value.";
    }
}

int QCustom3DVolume::textureDepth() const
{
    return dptrc()->m_textureDepth;
}

/*!
 * A convenience function for setting all three texture dimensions
 * (\a width, \a height, and \a depth) at once.
 *
 * \sa textureData
 */
void QCustom3DVolume::setTextureDimensions(int width, int height, int depth)
{
    setTextureWidth(width);
    setTextureWidth(height);
    setTextureWidth(depth);
}

/*!
 * \return the actual texture data width. When the texture format is QImage::Format_Indexed8,
 * this is textureWidth aligned to 32bit boundary. Otherwise this is four times textureWidth.
 */
int QCustom3DVolume::textureDataWidth() const
{
    int dataWidth = dptrc()->m_textureWidth;

    if (dptrc()->m_textureFormat == QImage::Format_Indexed8)
        dataWidth += dataWidth % 4;
    else
        dataWidth *= 4;

    return dataWidth;
}

/*! \property QCustom3DVolume::sliceIndexX
 *
 * The X dimension index into the texture data indicating which vertical slice to show.
 * Setting any dimension to negative indicates no slice for that dimension is drawn.
 * If all dimensions are negative, no slices are drawn and the volume is drawn normally.
 * Defaults to \c{-1}.
 *
 * \sa textureData
 */
void QCustom3DVolume::setSliceIndexX(int value)
{
    if (dptr()->m_sliceIndexX != value) {
        dptr()->m_sliceIndexX = value;
        dptr()->m_dirtyBitsVolume.sliceIndicesDirty = true;
        emit sliceIndexXChanged(value);
        emit dptr()->needUpdate();
    }
}

int QCustom3DVolume::sliceIndexX() const
{
    return dptrc()->m_sliceIndexX;
}

/*! \property QCustom3DVolume::sliceIndexY
 *
 * The Y dimension index into the texture data indicating which horizontal slice to show.
 * Setting any dimension to negative indicates no slice for that dimension is drawn.
 * If all dimensions are negative, no slices are drawn and the volume is drawn normally.
 * Defaults to \c{-1}.
 *
 * \sa textureData
 */
void QCustom3DVolume::setSliceIndexY(int value)
{
    if (dptr()->m_sliceIndexY != value) {
        dptr()->m_sliceIndexY = value;
        dptr()->m_dirtyBitsVolume.sliceIndicesDirty = true;
        emit sliceIndexYChanged(value);
        emit dptr()->needUpdate();
    }
}

int QCustom3DVolume::sliceIndexY() const
{
    return dptrc()->m_sliceIndexY;
}

/*! \property QCustom3DVolume::sliceIndexZ
 *
 * The Z dimension index into the texture data indicating which vertical slice to show.
 * Setting any dimension to negative indicates no slice for that dimension is drawn.
 * If all dimensions are negative, no slices are drawn and the volume is drawn normally.
 * Defaults to \c{-1}.
 *
 * \sa textureData
 */
void QCustom3DVolume::setSliceIndexZ(int value)
{
    if (dptr()->m_sliceIndexZ != value) {
        dptr()->m_sliceIndexZ = value;
        dptr()->m_dirtyBitsVolume.sliceIndicesDirty = true;
        emit sliceIndexZChanged(value);
        emit dptr()->needUpdate();
    }
}

int QCustom3DVolume::sliceIndexZ() const
{
    return dptrc()->m_sliceIndexZ;
}

/*!
 * A convenience function for setting all three slice indices (\a x, \a y, and \a z) at once.
 *
 * \sa textureData
 */
void QCustom3DVolume::setSliceIndices(int x, int y, int z)
{
    setSliceIndexX(x);
    setSliceIndexY(y);
    setSliceIndexZ(z);
}

/*! \property QCustom3DVolume::colorTable
 *
 * The array containing the colors for indexed texture formats. If the texture format is not
 * indexed, this array is not used and can be empty.
 *
 * Defaults to \c{0}.
 *
 * \sa textureData, textureFormat, QImage::colorTable()
 */
void QCustom3DVolume::setColorTable(const QVector<QRgb> &colors)
{
    if (dptr()->m_colorTable != colors) {
        dptr()->m_colorTable = colors;
        dptr()->m_dirtyBitsVolume.colorTableDirty = true;
        emit colorTableChanged();
        emit dptr()->needUpdate();
    }
}

QVector<QRgb> QCustom3DVolume::colorTable() const
{
    return dptrc()->m_colorTable;
}

/*! \property QCustom3DVolume::textureData
 *
 * The array containing the texture data in the format specified by textureFormat.
 * The size of this array must be at least
 * (\c{textureDataWidth * textureHeight * textureDepth * texture format color depth in bytes}).
 *
 * A 3D texture is defined by a stack of 2D subtextures. Each subtexture must be of identical size
 * (\c{textureDataWidth * textureHeight}), and the depth of the stack is defined by textureDepth
 * property. Each 2D texture data is identical to a QImage data with the same format, so
 * QImage::bits() can be used to supply the data for each subtexture.
 *
 * Ownership of the new array transfers to QCustom3DVolume instance.
 * If another array is set, the previous array is deleted.
 * If the same array is set again, it is assumed that the array contents have been changed and the
 * graph rendering is triggered.
 *
 * \note Each X-line of the data needs to be 32bit aligned. If the textureFormat is
 * QImage::Format_Indexed8 and textureWidth is not divisible by four, padding bytes need
 * to be added to each X-line of the \a data. You can get the padded byte count with
 * textureDataWidth() function.
 *
 * Defaults to \c{0}.
 *
 * \sa colorTable, textureFormat, setSubTextureData(), textureDataWidth()
 */
void QCustom3DVolume::setTextureData(QVector<uchar> *data)
{
    if (dptr()->m_textureData != data)
        delete dptr()->m_textureData;

    // Even if the pointer is same as previously, consider this property changed, as the values
    // can be changed unbeknownst to us via the array pointer.
    dptr()->m_textureData = data;
    dptr()->m_dirtyBitsVolume.textureDataDirty = true;
    emit textureDataChanged(data);
    emit dptr()->needUpdate();
}

/*!
 * This function creates a new texture data array from an array of \a images and sets it as
 * textureData for this volume object. The texture dimensions are also set according to image
 * and array dimensions. All of the images in the array must be the same size. If the images are not
 * all in QImage::Format_Indexed8 format, all texture data will be converted into
 * QImage::Format_ARGB32 format. If the images are in QImage::Format_Indexed8 format, the colorTable
 * for the entire volume will be taken from the first image.
 *
 * \return pointer to the newly created array.
 *
 * \sa textureData, textureWidth, textureHeight, textureDepth, textureFormat
 */
QVector<uchar> *QCustom3DVolume::createTextureData(const QVector<QImage *> &images)
{
    int imageCount = images.size();
    if (imageCount) {
        QImage *currentImage = images.at(0);
        int imageWidth = currentImage->width();
        int imageHeight = currentImage->height();
        QImage::Format imageFormat = currentImage->format();
        bool convert = false;
        if (imageFormat != QImage::Format_Indexed8 && imageFormat != QImage::Format_ARGB32) {
            convert = true;
            imageFormat = QImage::Format_ARGB32;
        } else {
            for (int i = 0; i < imageCount; i++) {
                currentImage = images.at(i);
                if (imageWidth != currentImage->width() || imageHeight != currentImage->height()) {
                    qWarning() << __FUNCTION__ << "Not all images were of the same size.";
                    setTextureData(0);
                    setTextureWidth(0);
                    setTextureHeight(0);
                    setTextureDepth(0);
                    return 0;

                }
                if (currentImage->format() != imageFormat) {
                    convert = true;
                    imageFormat = QImage::Format_ARGB32;
                    break;
                }
            }
        }
        int colorBytes = (imageFormat == QImage::Format_Indexed8) ? 1 : 4;
        int imageByteWidth = (imageFormat == QImage::Format_Indexed8)
                ? currentImage->bytesPerLine() : imageWidth;
        int frameSize = imageByteWidth * imageHeight * colorBytes;
        QVector<uchar> *newTextureData = new QVector<uchar>;
        newTextureData->resize(frameSize * imageCount);
        uchar *texturePtr = newTextureData->data();
        QImage convertedImage;

        for (int i = 0; i < imageCount; i++) {
            currentImage = images.at(i);
            if (convert) {
                convertedImage = currentImage->convertToFormat(imageFormat);
                currentImage = &convertedImage;
            }
            memcpy(texturePtr, static_cast<void *>(currentImage->bits()), frameSize);
            texturePtr += frameSize;
        }

        if (imageFormat == QImage::Format_Indexed8)
            setColorTable(images.at(0)->colorTable());
        setTextureData(newTextureData);
        setTextureFormat(imageFormat);
        setTextureWidth(imageWidth);
        setTextureHeight(imageHeight);
        setTextureDepth(imageCount);
    } else {
        setTextureData(0);
        setTextureWidth(0);
        setTextureHeight(0);
        setTextureDepth(0);
    }
    return dptr()->m_textureData;
}

QVector<uchar> *QCustom3DVolume::textureData() const
{
    return dptrc()->m_textureData;
}

/*!
 * This function allows setting a single 2D subtexture of the 3D texture.
 * The \a depthIndex parameter specifies the subtexture to set.
 * The texture\a data must be in the format specified by textureFormat property and have size of
 * (\c{textureDataWidth * textureHeight * texture format color depth in bytes}).
 *
 * \note Each X-line of the data needs to be 32bit aligned. If the textureFormat is
 * QImage::Format_Indexed8 and textureWidth is not divisible by four, padding bytes need
 * to be added to each X-line of the \a data.
 *
 * \sa textureData
 */
void QCustom3DVolume::setSubTextureData(int depthIndex, const uchar *data)
{
    if (data) {
        int frameSize = textureDataWidth() * dptr()->m_textureHeight;
        int startIndex = depthIndex * frameSize;

        if (depthIndex >= dptr()->m_textureDepth
                || (startIndex + frameSize) > dptr()->m_textureData->size()) {
            qWarning() << __FUNCTION__ << "Attempted to set invalid subtexture.";
        } else {
            void *subTexPtr = dptr()->m_textureData->data() + startIndex;
            memcpy(subTexPtr, static_cast<const void *>(data), frameSize);
            dptr()->m_dirtyBitsVolume.textureDataDirty = true;
            emit textureDataChanged(dptr()->m_textureData);
            emit dptr()->needUpdate();
        }
    } else {
        qWarning() << __FUNCTION__ << "Tried to set null data.";
    }
}

/*!
 * This function allows setting a single 2D subtexture of the 3D texture to a source \a image;
 * The \a depthIndex parameter specifies the subtexture to set.
 * The image must be in the format specified by the textureFormat property if the textureFormat
 * is indexed. If the textureFormat is QImage::Format_ARGB32, the image is converted to that format.
 * The image must have the size of (\c{textureWidth * textureHeight}).
 *
 * \sa textureData
 */
void QCustom3DVolume::setSubTextureData(int depthIndex, const QImage &image)
{
    if (image.width() == dptr()->m_textureWidth
            && image.height() == dptr()->m_textureHeight
            && (image.format() == dptr()->m_textureFormat
                || dptr()->m_textureFormat == QImage::Format_ARGB32)) {
        QImage convertedImage;
        if (dptr()->m_textureFormat == QImage::Format_ARGB32
                && image.format() != QImage::Format_ARGB32) {
            convertedImage = image.convertToFormat(QImage::Format_ARGB32);
        } else {
            convertedImage = image;
        }
        setSubTextureData(depthIndex, convertedImage.bits());
    } else {
        qWarning() << __FUNCTION__ << "Invalid image size or format.";
    }
}

/*! \property QCustom3DVolume::textureFormat
 *
 * The format of the textureData. Only two formats are supported currently:
 * QImage::Format_Indexed8 and QImage::Format_ARGB32. If an indexed format is specified, colorTable
 * must also be set.
 * Defaults to QImage::Format_ARGB32.
 *
 * \sa colorTable, textureData
 */
void QCustom3DVolume::setTextureFormat(QImage::Format format)
{
    if (format == QImage::Format_ARGB32 || format == QImage::Format_Indexed8) {
        if (dptr()->m_textureFormat != format) {
            dptr()->m_textureFormat = format;
            dptr()->m_dirtyBitsVolume.textureFormatDirty = true;
            emit textureFormatChanged(format);
            emit dptr()->needUpdate();
        }
    } else {
        qWarning() << __FUNCTION__ << "Attempted to set invalid texture format.";
    }
}

QImage::Format QCustom3DVolume::textureFormat() const
{
    return dptrc()->m_textureFormat;
}


/*!
 * \internal
 */
QCustom3DVolumePrivate *QCustom3DVolume::dptr()
{
    return static_cast<QCustom3DVolumePrivate *>(d_ptr.data());
}

/*!
 * \internal
 */
const QCustom3DVolumePrivate *QCustom3DVolume::dptrc() const
{
    return static_cast<const QCustom3DVolumePrivate *>(d_ptr.data());
}

QCustom3DVolumePrivate::QCustom3DVolumePrivate(QCustom3DVolume *q) :
    QCustom3DItemPrivate(q),
    m_sliceIndexX(-1),
    m_sliceIndexY(-1),
    m_sliceIndexZ(-1),
    m_textureWidth(0),
    m_textureHeight(0),
    m_textureDepth(0),
    m_textureFormat(QImage::Format_ARGB32),
    m_textureData(0)
{
    m_isVolumeItem = true;
    m_meshFile = QStringLiteral(":/defaultMeshes/barFull");
}

QCustom3DVolumePrivate::QCustom3DVolumePrivate(QCustom3DVolume *q, const QVector3D &position,
                                               const QVector3D &scaling,
                                               const QQuaternion &rotation,
                                               int textureWidth, int textureHeight,
                                               int textureDepth, QVector<uchar> *textureData,
                                               QImage::Format textureFormat,
                                               const QVector<QRgb> &colorTable) :
    QCustom3DItemPrivate(q, QStringLiteral(":/defaultMeshes/barFull"), position, scaling, rotation),
    m_sliceIndexX(-1),
    m_sliceIndexY(-1),
    m_sliceIndexZ(-1),
    m_textureWidth(textureWidth),
    m_textureHeight(textureHeight),
    m_textureDepth(textureDepth),
    m_textureFormat(textureFormat),
    m_colorTable(colorTable),
    m_textureData(textureData)
{
    m_isVolumeItem = true;
    m_shadowCasting = false;

    if (m_textureWidth < 0)
        m_textureWidth = 0;
    if (m_textureHeight < 0)
        m_textureHeight = 0;
    if (m_textureDepth < 0)
        m_textureDepth = 0;

    if (m_colorTable.size() != 256)
        m_colorTable.clear();

    if (m_textureFormat != QImage::Format_Indexed8)
        m_textureFormat = QImage::Format_ARGB32;

}

QCustom3DVolumePrivate::~QCustom3DVolumePrivate()
{
    delete m_textureData;
}

void QCustom3DVolumePrivate::resetDirtyBits()
{
    QCustom3DItemPrivate::resetDirtyBits();

    m_dirtyBitsVolume.textureDimensionsDirty = false;
    m_dirtyBitsVolume.sliceIndicesDirty = false;
    m_dirtyBitsVolume.colorTableDirty = false;
    m_dirtyBitsVolume.textureDataDirty = false;
    m_dirtyBitsVolume.textureFormatDirty = false;
}

QCustom3DVolume *QCustom3DVolumePrivate::qptr()
{
    return static_cast<QCustom3DVolume *>(q_ptr);
}

QT_END_NAMESPACE_DATAVISUALIZATION

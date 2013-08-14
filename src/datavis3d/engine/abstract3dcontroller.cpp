/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include "abstract3dcontroller_p.h"
#include "camerahelper_p.h"
#include "qabstractaxis_p.h"
#include "qvalueaxis.h"

QT_DATAVIS3D_BEGIN_NAMESPACE

Abstract3DController::Abstract3DController(QRect boundRect, QObject *parent) :
    QObject(parent),
    m_boundingRect(boundRect.x(), boundRect.y(), boundRect.width(), boundRect.height()),
    m_horizontalRotation(-45.0f),
    m_verticalRotation(15.0f),
    m_theme(),
    m_font(QFont(QStringLiteral("Arial"))),
    m_shadowQuality(QDataVis::ShadowLow),
    m_labelTransparency(QDataVis::TransparencyFromTheme),
    m_cameraHelper(new CameraHelper()),
    m_zoomLevel(100),
    m_axisX(0),
    m_axisY(0),
    m_axisZ(0)
{
    m_theme.useColorTheme(QDataVis::ThemeSystem);
}

Abstract3DController::~Abstract3DController()
{
    delete m_cameraHelper;
    delete m_axisX;
    delete m_axisY;
    delete m_axisZ;
}

void Abstract3DController::setSize(const int width, const int height)
{
    m_boundingRect.setWidth(width);
    m_boundingRect.setHeight(height);

    //qDebug() << "Abstract3DController::setSize " << m_boundingRect.width() << "x" <<m_boundingRect.height();
    emit boundingRectChanged(m_boundingRect);
}

const QSize Abstract3DController::size()
{
    return m_boundingRect.size();
}

const QRect Abstract3DController::boundingRect()
{
    return m_boundingRect;
}

void Abstract3DController::setBoundingRect(const QRect boundingRect)
{
    m_boundingRect = boundingRect;

    //qDebug() << "Abstract3DController::setBoundingRect " << m_boundingRect.width() << "x" <<m_boundingRect.height();
    emit boundingRectChanged(m_boundingRect);
}

void Abstract3DController::setWidth(const int width)
{
    m_boundingRect.setWidth(width);
    emit sizeChanged(m_boundingRect);
}

int Abstract3DController::width()
{
    return m_boundingRect.width();
}

void Abstract3DController::setHeight(const int height)
{
    m_boundingRect.setHeight(height);
    emit sizeChanged(m_boundingRect);
}

int Abstract3DController::height()
{
    return m_boundingRect.height();
}

void Abstract3DController::setX(const int x)
{
    m_boundingRect.setX(x);
    emit positionChanged(m_boundingRect);
}

int Abstract3DController::x()
{
    return m_boundingRect.x();
}

void Abstract3DController::setY(const int y)
{
    m_boundingRect.setY(y);
    emit positionChanged(m_boundingRect);
}

int Abstract3DController::y()
{
    return m_boundingRect.y();
}

void Abstract3DController::setAxisX(QAbstractAxis *axis)
{
    setAxisHelper(QAbstractAxis::AxisOrientationX, axis, &m_axisX);
}

QAbstractAxis *Abstract3DController::axisX()
{
    return m_axisX;
}

void Abstract3DController::setAxisY(QAbstractAxis *axis)
{
    setAxisHelper(QAbstractAxis::AxisOrientationY, axis, &m_axisY);
}

QAbstractAxis *Abstract3DController::axisY()
{
    return m_axisY;
}

void Abstract3DController::setAxisZ(QAbstractAxis *axis)
{
    setAxisHelper(QAbstractAxis::AxisOrientationZ, axis, &m_axisZ);
}

QAbstractAxis *Abstract3DController::axisZ()
{
    return m_axisZ;
}

int Abstract3DController::zoomLevel()
{
    return m_zoomLevel;
}

void Abstract3DController::setZoomLevel(int zoomLevel)
{
    m_zoomLevel = zoomLevel;
    emit zoomLevelChanged(zoomLevel);
}

void Abstract3DController::setCameraPreset(QDataVis::CameraPreset preset)
{
    m_cameraHelper->setCameraPreset(preset);
}

void Abstract3DController::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    m_horizontalRotation = qBound(-180.0f, horizontal, 180.0f);
    m_verticalRotation = qBound(0.0f, vertical, 90.0f);
    m_zoomLevel = qBound(10, distance, 500);
    m_cameraHelper->setCameraRotation(QPointF(m_horizontalRotation,
                                              m_verticalRotation));
    //qDebug() << "camera rotation set to" << m_horizontalRotation << m_verticalRotation;
}

void Abstract3DController::setObjectColor(QColor baseColor, QColor heightColor, QColor depthColor,
                                          bool uniform)
{
    m_theme.m_baseColor = baseColor;
    m_theme.m_heightColor = heightColor;
    m_theme.m_depthColor = depthColor;
    m_theme.m_uniformColor = uniform;

    emit themeChanged(m_theme);
}

void Abstract3DController::setColorTheme(QDataVis::ColorTheme colorTheme)
{
    m_theme.useColorTheme(colorTheme);
    emit themeChanged(m_theme);
}

Theme Abstract3DController::theme()
{
    return m_theme;
}

void Abstract3DController::setFontSize(float fontsize)
{
    m_font.setPointSizeF(fontsize);
    emit fontChanged(m_font);
}

float Abstract3DController::fontSize()
{
    return m_font.pointSizeF();
}

void Abstract3DController::setFont(const QFont &font)
{
    m_font = font;
    emit fontChanged(m_font);
}

QFont Abstract3DController::font()
{
    return m_font;
}


void Abstract3DController::setShadowQuality(QDataVis::ShadowQuality quality)
{
    m_shadowQuality = quality;
    emit shadowQualityChanged(m_shadowQuality);
}

QDataVis::ShadowQuality Abstract3DController::shadowQuality()
{
    return m_shadowQuality;
}


void Abstract3DController::setLabelTransparency(QDataVis::LabelTransparency transparency)
{
    m_labelTransparency = transparency;
    emit labelTransparencyUpdated(m_labelTransparency);
}

QDataVis::LabelTransparency Abstract3DController::labelTransparency()
{
    return m_labelTransparency;
}

void Abstract3DController::handleAxisTitleChanged(const QString &title)
{
    if (sender() == m_axisX)
        emit axisTitleChanged(QAbstractAxis::AxisOrientationX, title);
    else if (sender() == m_axisY)
        emit axisTitleChanged(QAbstractAxis::AxisOrientationY, title);
    else if (sender() == m_axisZ)
        emit axisTitleChanged(QAbstractAxis::AxisOrientationZ, title);
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
}

void Abstract3DController::handleAxisLabelsChanged()
{
    if (sender() == m_axisX)
        emit axisLabelsChanged(QAbstractAxis::AxisOrientationX, m_axisX->labels());
    else if (sender() == m_axisY)
        emit axisLabelsChanged(QAbstractAxis::AxisOrientationY, m_axisY->labels());
    else if (sender() == m_axisZ)
        emit axisLabelsChanged(QAbstractAxis::AxisOrientationZ, m_axisZ->labels());
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
}

void Abstract3DController::handleAxisRangeChanged(qreal min, qreal max)
{
    if (sender() == m_axisX)
        emit axisRangeChanged(QAbstractAxis::AxisOrientationX, min, max);
    else if (sender() == m_axisY)
        emit axisRangeChanged(QAbstractAxis::AxisOrientationY, min, max);
    else if (sender() == m_axisZ)
        emit axisRangeChanged(QAbstractAxis::AxisOrientationZ, min, max);
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
}

void Abstract3DController::handleAxisSegmentCountChanged(int count)
{
    if (sender() == m_axisX)
        emit axisSegmentCountChanged(QAbstractAxis::AxisOrientationX, count);
    else if (sender() == m_axisY)
        emit axisSegmentCountChanged(QAbstractAxis::AxisOrientationY, count);
    else if (sender() == m_axisZ)
        emit axisSegmentCountChanged(QAbstractAxis::AxisOrientationZ, count);
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
}

void Abstract3DController::handleAxisSubSegmentCountChanged(int count)
{
    if (sender() == m_axisX)
        emit axisSubSegmentCountChanged(QAbstractAxis::AxisOrientationX, count);
    else if (sender() == m_axisY)
        emit axisSubSegmentCountChanged(QAbstractAxis::AxisOrientationY, count);
    else if (sender() == m_axisZ)
        emit axisSubSegmentCountChanged(QAbstractAxis::AxisOrientationZ, count);
    else
        qWarning() << __FUNCTION__ << "invoked for invalid axis";
}

void Abstract3DController::handleAxisAutoAdjustRangeChanged(bool autoAdjust)
{
    Q_UNUSED(autoAdjust)
    // Adjust axis range according to data. Default implementation does nothing,
    // This needs to be implemented by subclass.
}

void Abstract3DController::setAxisHelper(QAbstractAxis::AxisOrientation orientation,
                                         QAbstractAxis *axis, QAbstractAxis **axisPtr)
{
    Q_ASSERT(axis);

    delete *axisPtr;
    *axisPtr = axis;
    axis->d_ptr->setOrientation(orientation);

    QObject::connect(axis, &QAbstractAxis::titleChanged,
                     this, &Abstract3DController::handleAxisTitleChanged);
    QObject::connect(axis, &QAbstractAxis::labelsChanged,
                     this, &Abstract3DController::handleAxisLabelsChanged);

    emit axisTypeChanged(orientation, axis->type());
    emit axisTitleChanged(orientation, axis->title());
    emit axisLabelsChanged(orientation, axis->labels());

    if (axis->type() & QAbstractAxis::AxisTypeValue) {
        QValueAxis *valueAxis = static_cast<QValueAxis *>(axis);
        QObject::connect(valueAxis, &QValueAxis::rangeChanged,
                         this, &Abstract3DController::handleAxisRangeChanged);
        QObject::connect(valueAxis, &QValueAxis::segmentCountChanged,
                         this, &Abstract3DController::handleAxisSegmentCountChanged);
        QObject::connect(valueAxis, &QValueAxis::subSegmentCountChanged,
                         this, &Abstract3DController::handleAxisSubSegmentCountChanged);
        QObject::connect(valueAxis, &QValueAxis::autoAdjustRangeChanged,
                         this, &Abstract3DController::handleAxisAutoAdjustRangeChanged);
        emit axisRangeChanged(orientation, valueAxis->min(), valueAxis->max());
        emit axisSegmentCountChanged(orientation, valueAxis->segmentCount());
        emit axisSubSegmentCountChanged(orientation, valueAxis->subSegmentCount());
        handleAxisAutoAdjustRangeChanged(valueAxis->isAutoAdjustRange());
    }
}

QT_DATAVIS3D_END_NAMESPACE

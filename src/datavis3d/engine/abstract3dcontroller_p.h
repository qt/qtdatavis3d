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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef CONTROLLER3DBASE_H
#define CONTROLLER3DBASE_H

#include <QObject>

#include "datavis3dglobal_p.h"
#include "theme_p.h"
#include "qabstractaxis.h"
#include "drawer_p.h"

class QFont;

QT_DATAVIS3D_BEGIN_NAMESPACE

class CameraHelper;

class QT_DATAVIS3D_EXPORT Abstract3DController : public QObject
{
    Q_OBJECT

public:
    enum MouseState {
        MouseNone = 0,
        MouseOnScene,
        MouseOnOverview,
        MouseOnZoom,
        MouseRotating,
        MouseOnPinch
    };

private:
    QRect m_boundingRect;
    GLfloat m_horizontalRotation;
    GLfloat m_verticalRotation;
    Theme m_theme;
    QFont m_font;
    QDataVis::ShadowQuality m_shadowQuality;
    QDataVis::LabelTransparency m_labelTransparency;

protected:
    CameraHelper *m_cameraHelper;
    int m_zoomLevel;
    QAbstractAxis *m_axisX;
    QAbstractAxis *m_axisY;
    QAbstractAxis *m_axisZ;

    explicit Abstract3DController(QRect boundRect, QObject *parent = 0);
    ~Abstract3DController();

public:

    virtual void render(const GLuint defaultFboHandle)=0;

    // Size
    virtual void setSize(const int width, const int height);
    virtual const QSize size();
    virtual const QRect boundingRect();
    virtual void setBoundingRect(const QRect boundingRect);
    virtual void setWidth(const int width);
    virtual int width();
    virtual void setHeight(const int height);
    virtual int height();
    virtual void setX(const int x);
    virtual int x();
    virtual void setY(const int y);
    virtual int y();
    virtual void setAxisX(QAbstractAxis *axis);
    virtual QAbstractAxis *axisX();
    virtual void setAxisY(QAbstractAxis *axis);
    virtual QAbstractAxis *axisY();
    virtual void setAxisZ(QAbstractAxis *axis);
    virtual QAbstractAxis *axisZ();

    virtual int zoomLevel();
    virtual void setZoomLevel(int zoomLevel);

    // Select preset camera placement
    virtual void setCameraPreset(QDataVis::CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) (or (-90...90) if there are negative values) angles and distance in
    // percentage (10...500))
    virtual void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height)
    // color to change from bottom to top
    virtual void setObjectColor(QColor baseColor, QColor heightColor, QColor depthColor,
                                bool uniform = true);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    virtual void setColorTheme(QDataVis::ColorTheme colorTheme);
    virtual Theme theme();

    // Font size adjustment
    virtual void setFontSize(float fontsize);
    virtual float fontSize();

    // Set font
    virtual void setFont(const QFont &font);
    virtual QFont font();


    // Adjust shadow quality
    virtual void setShadowQuality(QDataVis::ShadowQuality quality);
    virtual QDataVis::ShadowQuality shadowQuality();

    // Label transparency adjustment
    virtual void setLabelTransparency(QDataVis::LabelTransparency transparency);
    virtual QDataVis::LabelTransparency labelTransparency();

public slots:
    virtual void handleAxisTitleChanged(const QString &title);
    virtual void handleAxisLabelsChanged();
    virtual void handleAxisRangeChanged(qreal min, qreal max);
    virtual void handleAxisSegmentCountChanged(int count);
    virtual void handleAxisSubSegmentCountChanged(int count);
    virtual void handleAxisAutoAdjustRangeChanged(bool autoAdjust);

signals:
    void boundingRectChanged(QRect boundingRect);
    void sizeChanged(QRect boundingRect);
    void positionChanged(QRect boundingRect);
    void zoomLevelChanged(int zoomLevel);
    void themeChanged(Theme theme);
    void fontChanged(QFont font); // TODO should be handled via axis?? What about font for selection label?
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void labelTransparencyUpdated(QDataVis::LabelTransparency transparency);
    void axisTypeChanged(QAbstractAxis::AxisOrientation orientation, QAbstractAxis::AxisType type);
    void axisTitleChanged(QAbstractAxis::AxisOrientation orientation, QString title);
    void axisLabelsChanged(QAbstractAxis::AxisOrientation orientation, QStringList labels);
    void axisRangeChanged(QAbstractAxis::AxisOrientation orientation, qreal min, qreal max);
    void axisSegmentCountChanged(QAbstractAxis::AxisOrientation orientation, int count);
    void axisSubSegmentCountChanged(QAbstractAxis::AxisOrientation orientation, int count);

private:
    void setAxisHelper(QAbstractAxis::AxisOrientation orientation, QAbstractAxis *axis, QAbstractAxis **axisPtr);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // CONTROLLER3DBASE_H

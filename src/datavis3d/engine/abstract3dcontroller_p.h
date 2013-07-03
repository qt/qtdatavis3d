/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
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
    ShadowQuality m_shadowQuality;
    LabelTransparency m_labelTransparency;

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
    virtual void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) (or (-90...90) if there are negative values) angles and distance in
    // percentage (10...500))
    virtual void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height)
    // color to change from bottom to top
    virtual void setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                             bool uniform = true);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    virtual void setColorTheme(ColorTheme colorTheme);
    virtual Theme theme();

    // Adjust shadow quality
    virtual void setShadowQuality(ShadowQuality quality);
    virtual ShadowQuality shadowQuality();

    // Label transparency adjustment
    virtual void setLabelTransparency(LabelTransparency transparency);
    virtual LabelTransparency labelTransparency();

    // TODO: abstract renderer should have accessor for Drawer instead
    virtual Drawer *drawer() = 0;

signals:
    void boundingRectChanged(QRect boundingRect);
    void sizeChanged(QRect boundingRect);
    void positionChanged(QRect boundingRect);
    void zoomLevelChanged(int zoomLevel);
    void themeChanged(Theme theme);
    void shadowQualityChanged(ShadowQuality quality);
    void labelTransparencyUpdated(LabelTransparency transparency);
    void axisChanged(QAbstractAxis::AxisOrientation orientation, QAbstractAxis *newAxis);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // CONTROLLER3DBASE_H

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

#ifndef Q3DMAPS_H
#define Q3DMAPS_H

#include <QtDataVis3D/qdatavis3dnamespace.h>
#include <QtDataVis3D/q3dwindow.h>

#include <QFont>
#include <QVector3D>

class QImage;
class QRect;

QT_DATAVIS3D_BEGIN_NAMESPACE

class Maps3DController;
class Q3DMapsPrivate;
class QMapDataProxy;

class QT_DATAVIS3D_EXPORT Q3DMaps : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(float fontSize READ fontSize WRITE setFontSize)

public:

    enum AdjustmentDirection {
        AdjustHeight = 0,   // map value to y
        AdjustWidth,        // map value to x
        AdjustDepth,        // map value to z
        AdjustRadius,       // map value to x and z
        AdjustAll           // map value to all (x, y, z)
    };

public:
    explicit Q3DMaps();
    ~Q3DMaps();

    void setWidth(const int width);
    void setHeight(const int height);

    // bar specifications; base thickness in x, y and z, enum to indicate which direction is increased with value
    // TODO: Start using thickness also in adjustment direction; use it as a relative value.
    // For example, in AdjustAll mode setting thickness to (0.1f, 1.0f, 0.5f) would apply value to
    // x at 10%, y at 100% and z at 50%. If a dimension is not included, given thickness states its absolute value.
    void setBarSpecs(const QVector3D &thickness = QVector3D(1.0f, 1.0f, 1.0f),
                     AdjustmentDirection direction = AdjustHeight);

    // bar type; bars (=cubes), pyramids, cones, cylinders, balls, etc.
    void setBarType(MeshStyle style, bool smooth = false);

    // override bar type with own mesh
    void setMeshFileName(const QString &objFileName);

    // Select preset camera placement
    void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) angles and distance in percentage (10...500))
    void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text colors are affected)
    void setTheme(ColorTheme theme);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height) color to change from bottom to top
    void setBarColor(QColor baseColor, QColor heightColor, bool uniform = true);

    // Set area specs
    void setAreaSpecs(const QRect &areaRect, const QImage &image);

    // Set area image
    void setImage(const QImage &image);

    // TODO: light placement API

    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode() const;

    // Set window title
    void setWindowTitle(const QString &title);
    QString windowTitle() const;

    // Font size adjustment
    void setFontSize(float fontsize);
    float fontSize() const;

    // Set font
    void setFont(const QFont &font);
    QFont font() const;

    // Label transparency adjustment
    void setLabelTransparency(LabelTransparency transparency);
    LabelTransparency labelTransparency() const;

    // Adjust shadow quality
    ShadowQuality setShadowQuality(ShadowQuality quality);
    ShadowQuality shadowQuality() const;

    // Sets the data proxy. Assumes ownership of the data proxy. Deletes old proxy.
    void setDataProxy(QMapDataProxy *proxy);
    QMapDataProxy *dataProxy();

protected:
    void render();

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QScopedPointer<Q3DMapsPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DMaps)
};

QT_DATAVIS3D_END_NAMESPACE

#endif

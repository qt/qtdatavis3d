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

#ifndef Q3DSCATTER_H
#define Q3DSCATTER_H

#include <QtDataVis3D/qdatavis3denums.h>
#include <QtDataVis3D/q3dwindow.h>
#include <QFont>

QT_DATAVIS3D_BEGIN_NAMESPACE

class Q3DScatterPrivate;
class LabelItem;
class QValueAxis;
class QCategoryAxis;
class QScatterDataProxy;

class QT_DATAVIS3D_EXPORT Q3DScatter : public Q3DWindow
{
    Q_OBJECT
    Q_PROPERTY(QtDataVis3D::QDataVis::SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(QtDataVis3D::QDataVis::LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(QtDataVis3D::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(float fontSize READ fontSize WRITE setFontSize)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_ENUMS(QtDataVis3D::QDataVis::SelectionMode)
    Q_ENUMS(QtDataVis3D::QDataVis::ShadowQuality)
    Q_ENUMS(QtDataVis3D::QDataVis::LabelTransparency)

public:
    explicit Q3DScatter();
    ~Q3DScatter();

    void setObjectType(QDataVis::MeshStyle style, bool smooth = false);

    void setCameraPreset(QDataVis::CameraPreset preset);

    void setCameraPosition(qreal horizontal, qreal vertical, int distance = 100);

    void setTheme(QDataVis::ColorTheme theme);

    void setObjectColor(QColor baseColor, QColor heightColor, QColor depthColor,
                        bool uniform = true);

    void setMeshFileName(const QString &objFileName);

    void setSelectionMode(QDataVis::SelectionMode mode);
    QDataVis::SelectionMode selectionMode() const;

    void setWindowTitle(const QString &title);
    QString windowTitle() const;

    void setFontSize(float fontsize);
    float fontSize() const;

    void setFont(const QFont &font);
    QFont font() const;

    void setLabelTransparency(QDataVis::LabelTransparency transparency);
    QDataVis::LabelTransparency labelTransparency() const;

    void setGridVisible(bool visible);
    bool isGridVisible() const;

    void setWidth(const int width);
    void setHeight(const int height);

    void setBackgroundVisible(bool visible);
    bool isBackgroundVisible() const;

    void setShadowQuality(QDataVis::ShadowQuality quality);
    QDataVis::ShadowQuality shadowQuality() const;

    void setValueAxisX(QValueAxis *axis);
    QValueAxis *valueAxisX();

    void setValueAxisY(QValueAxis *axis);
    QValueAxis *valueAxisY();

    void setValueAxisZ(QValueAxis *axis);
    QValueAxis *valueAxisZ();

    void setDataProxy(QtDataVis3D::QScatterDataProxy *proxy);
    QScatterDataProxy *dataProxy();

public slots:
    // Used to detect when shadow quality changes autonomously due to e.g. resizing.
    void handleShadowQualityUpdate(QDataVis::ShadowQuality quality);

signals:
    void shadowQualityChanged(QDataVis::ShadowQuality quality);

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
    QScopedPointer<Q3DScatterPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DScatter)
};

QT_DATAVIS3D_END_NAMESPACE

#endif

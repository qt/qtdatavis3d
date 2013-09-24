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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef ABSTRACTDECLARATIVE_P_H
#define ABSTRACTDECLARATIVE_P_H

#include "datavisualizationglobal_p.h"
#include "abstract3dcontroller_p.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class AbstractDeclarative : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::QDataVis::SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(QtDataVisualization::QDataVis::LabelTransparency labelTransparency READ labelTransparency WRITE setLabelTransparency)
    Q_PROPERTY(QtDataVisualization::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality)
    Q_PROPERTY(QtDataVisualization::QDataVis::CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset)
    Q_PROPERTY(QtDataVisualization::QDataVis::ColorTheme theme READ theme WRITE setTheme)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible)
    Q_PROPERTY(bool backgroundVisible READ isBackgroundVisible WRITE setBackgroundVisible)
    Q_ENUMS(QtDataVisualization::QDataVis::SelectionMode)
    Q_ENUMS(QtDataVisualization::QDataVis::ShadowQuality)
    Q_ENUMS(QtDataVisualization::QDataVis::LabelTransparency)
    Q_ENUMS(QtDataVisualization::QDataVis::CameraPreset)
    Q_ENUMS(QtDataVisualization::QDataVis::ColorTheme)
    Q_PROPERTY(QString itemLabelFormat READ itemLabelFormat WRITE setItemLabelFormat)

public:
    explicit AbstractDeclarative(QQuickItem *parent = 0);
    virtual ~AbstractDeclarative();

    Q_INVOKABLE void setCameraPosition(qreal horizontal, qreal vertical, int distance);

    virtual void setCameraPreset(QDataVis::CameraPreset preset);
    virtual QDataVis::CameraPreset cameraPreset() const;

    virtual void setTheme(QDataVis::ColorTheme theme);
    virtual QDataVis::ColorTheme theme() const;

    virtual void setSelectionMode(QDataVis::SelectionMode mode);
    virtual QDataVis::SelectionMode selectionMode() const;

    virtual void setFont(const QFont &font);
    virtual QFont font() const;

    virtual void setLabelTransparency(QDataVis::LabelTransparency transparency);
    virtual QDataVis::LabelTransparency labelTransparency() const;

    virtual void setGridVisible(bool visible);
    virtual bool isGridVisible() const;

    virtual void setBackgroundVisible(bool visible);
    virtual bool isBackgroundVisible() const;

    virtual void setShadowQuality(QDataVis::ShadowQuality quality);
    virtual QDataVis::ShadowQuality shadowQuality() const;

    virtual void setItemLabelFormat(const QString &format);
    virtual QString itemLabelFormat() const;

    void setSharedController(Abstract3DController *controller);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    // Used to detect when shadow quality changes autonomously due to e.g. resizing.
    virtual void handleShadowQualityUpdate(QDataVis::ShadowQuality quality);

signals:
    // Signals shadow quality changes.
    void shadowQualityChanged(QDataVis::ShadowQuality quality);

private:
    Abstract3DController *m_controller;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // ABSTRACTDECLARATIVE_P_H

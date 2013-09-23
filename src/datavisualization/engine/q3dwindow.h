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

#ifndef Q3DWINDOW_H
#define Q3DWINDOW_H

#include <QtDataVisualization/qdatavisualizationenums.h>

#include <QWindow>
#include <QOpenGLFunctions>
#include <QScreen>

class QPainter;

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DWindowPrivate;
class Abstract3DController;

class QT_DATAVISUALIZATION_EXPORT Q3DWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Q3DWindow(QWindow *parent = 0);
    virtual ~Q3DWindow();

protected slots:
    void renderLater();
    void renderNow();

protected:
    virtual void render();

    bool event(QEvent *event);
    void exposeEvent(QExposeEvent *event);
    void setVisualController(Abstract3DController *controller);
    void handleDevicePixelRatioChange();

private:
    QScopedPointer<Q3DWindowPrivate> d_ptr;

    friend class Q3DBars;
    friend class Q3DScatter;
    friend class Q3DSurface;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

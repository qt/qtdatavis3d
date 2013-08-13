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

#ifndef Q3DWINDOW_H
#define Q3DWINDOW_H

#include <QtDataVis3D/qdatavis3dnamespace.h>

#include <QWindow>
#include <QOpenGLFunctions>

class QPainter;

QT_DATAVIS3D_BEGIN_NAMESPACE

class Q3DWindowPrivate;

class QT_DATAVIS3D_EXPORT Q3DWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Q3DWindow(QWindow *parent = 0);
    ~Q3DWindow();

private slots:
    void renderLater();
    void renderNow();

protected:
    virtual void render();

    void setAnimating(bool animating);
    bool event(QEvent *event);
    void exposeEvent(QExposeEvent *event);

private:
    QScopedPointer<Q3DWindowPrivate> d_ptr;

    friend class Q3DBarsPrivate;
    friend class Q3DBars;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

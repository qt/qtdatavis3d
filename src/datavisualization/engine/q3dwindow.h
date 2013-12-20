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
class QAbstract3DInputHandler;
class Q3DTheme;

class QT_DATAVISUALIZATION_EXPORT Q3DWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
    Q_PROPERTY(Q3DTheme* activeTheme READ activeTheme WRITE setActiveTheme NOTIFY activeThemeChanged)

protected:
    explicit Q3DWindow(Q3DWindowPrivate *d, QWindow *parent = 0);
public:
    virtual ~Q3DWindow();

    void addInputHandler(QAbstract3DInputHandler *inputHandler);
    void releaseInputHandler(QAbstract3DInputHandler *inputHandler);
    void setActiveInputHandler(QAbstract3DInputHandler *inputHandler);
    QAbstract3DInputHandler *activeInputHandler();

    void addTheme(Q3DTheme *theme);
    void releaseTheme(Q3DTheme *theme);
    void setActiveTheme(Q3DTheme *theme);
    Q3DTheme *activeTheme() const;
    QList<Q3DTheme *> themes() const;

protected:
    bool event(QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void exposeEvent(QExposeEvent *event);

signals:
    void activeThemeChanged(Q3DTheme *theme);

private:
    QScopedPointer<Q3DWindowPrivate> d_ptr;

    friend class Q3DBars;
    friend class Q3DScatter;
    friend class Q3DSurface;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

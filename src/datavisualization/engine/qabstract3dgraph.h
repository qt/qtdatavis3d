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

#ifndef QABSTRACT3DGRAPH_H
#define QABSTRACT3DGRAPH_H

#include <QtDataVisualization/qdatavisualizationglobal.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/qabstract3dinputhandler.h>
#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QAbstract3DGraphPrivate;

class QT_DATAVISUALIZATION_EXPORT QAbstract3DGraph : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
    Q_ENUMS(ShadowQuality)
    Q_FLAGS(SelectionFlag SelectionFlags)
    Q_PROPERTY(QAbstract3DInputHandler* activeInputHandler READ activeInputHandler WRITE setActiveInputHandler NOTIFY activeInputHandlerChanged)
    Q_PROPERTY(Q3DTheme* activeTheme READ activeTheme WRITE setActiveTheme NOTIFY activeThemeChanged)
    Q_PROPERTY(SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(Q3DScene* scene READ scene)

protected:
    explicit QAbstract3DGraph(QAbstract3DGraphPrivate *d, const QSurfaceFormat *format, QWindow *parent = 0);

public:
    enum SelectionFlag {
        SelectionNone              = 0,
        SelectionItem              = 1,
        SelectionRow               = 2,
        SelectionItemAndRow        = SelectionItem | SelectionRow,
        SelectionColumn            = 4,
        SelectionItemAndColumn     = SelectionItem | SelectionColumn,
        SelectionRowAndColumn      = SelectionRow | SelectionColumn,
        SelectionItemRowAndColumn  = SelectionItem | SelectionRow | SelectionColumn,
        SelectionSlice             = 8,
        SelectionMultiSeries       = 16
    };
    Q_DECLARE_FLAGS(SelectionFlags, SelectionFlag)

    enum ShadowQuality {
        ShadowQualityNone = 0,
        ShadowQualityLow,
        ShadowQualityMedium,
        ShadowQualityHigh,
        ShadowQualitySoftLow,
        ShadowQualitySoftMedium,
        ShadowQualitySoftHigh
    };

public:
    virtual ~QAbstract3DGraph();

    void addInputHandler(QAbstract3DInputHandler *inputHandler);
    void releaseInputHandler(QAbstract3DInputHandler *inputHandler);
    void setActiveInputHandler(QAbstract3DInputHandler *inputHandler);
    QAbstract3DInputHandler *activeInputHandler() const;
    QList<QAbstract3DInputHandler *> inputHandlers() const;

    void addTheme(Q3DTheme *theme);
    void releaseTheme(Q3DTheme *theme);
    void setActiveTheme(Q3DTheme *theme);
    Q3DTheme *activeTheme() const;
    QList<Q3DTheme *> themes() const;

    void setSelectionMode(SelectionFlags mode);
    SelectionFlags selectionMode() const;

    void setShadowQuality(ShadowQuality quality);
    ShadowQuality shadowQuality() const;
    virtual bool shadowsSupported() const;

    Q3DScene *scene() const;

    void clearSelection();

protected:
    bool event(QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void exposeEvent(QExposeEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);


signals:
    void activeInputHandlerChanged(QAbstract3DInputHandler *inputHandler);
    void activeThemeChanged(Q3DTheme *theme);
    void selectionModeChanged(SelectionFlags mode);
    void shadowQualityChanged(ShadowQuality quality);

private:
    Q_DISABLE_COPY(QAbstract3DGraph)
    QScopedPointer<QAbstract3DGraphPrivate> d_ptr;

    friend class Q3DBars;
    friend class Q3DScatter;
    friend class Q3DSurface;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstract3DGraph::SelectionFlags)

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

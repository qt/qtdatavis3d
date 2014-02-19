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
#include "qabstract3dinputhandler.h"
#include "declarativescene_p.h"

#include <QtCore/QAbstractItemModel>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class DeclarativeRenderNode;

class AbstractDeclarative : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(ShadowQuality)
    Q_ENUMS(RenderingMode)
    Q_FLAGS(SelectionFlag SelectionFlags)
    Q_PROPERTY(SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(int msaaSamples READ msaaSamples WRITE setMsaaSamples NOTIFY msaaSamplesChanged)
    Q_PROPERTY(Declarative3DScene* scene READ scene NOTIFY sceneChanged)
    Q_PROPERTY(QAbstract3DInputHandler* inputHandler READ inputHandler WRITE setInputHandler NOTIFY inputHandlerChanged)
    Q_PROPERTY(Q3DTheme* theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(RenderingMode renderingMode READ renderingMode WRITE setRenderingMode NOTIFY renderingModeChanged)

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

    enum RenderingMode {
        RenderDirectToBackground = 0,
        RenderDirectToBackground_NoClear,
        RenderIndirect
    };

public:
    explicit AbstractDeclarative(QQuickItem *parent = 0);
    virtual ~AbstractDeclarative();

    virtual void setRenderingMode(RenderingMode mode);
    virtual AbstractDeclarative::RenderingMode renderingMode() const;

    virtual void setSelectionMode(SelectionFlags mode);
    virtual AbstractDeclarative::SelectionFlags selectionMode() const;

    virtual void setShadowQuality(ShadowQuality quality);
    virtual AbstractDeclarative::ShadowQuality shadowQuality() const;

    virtual void setMsaaSamples(int samples);
    virtual int msaaSamples() const;

    virtual Declarative3DScene *scene() const;

    virtual QAbstract3DInputHandler *inputHandler() const;
    virtual void setInputHandler(QAbstract3DInputHandler *inputHandler);

    virtual void setTheme(Q3DTheme *theme);
    virtual Q3DTheme *theme() const;

    Q_INVOKABLE virtual void clearSelection();

    virtual void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

    void setSharedController(Abstract3DController *controller);
    // Used to synch up data model from controller to renderer while main thread is locked
    void synchDataToRenderer();
    void render();

    void activateOpenGLContext(QQuickWindow *window);
    void doneOpenGLContext(QQuickWindow *window);

    void checkWindowList(QQuickWindow *window);

public slots:
    virtual void handleAxisXChanged(QAbstract3DAxis *axis) = 0;
    virtual void handleAxisYChanged(QAbstract3DAxis *axis) = 0;
    virtual void handleAxisZChanged(QAbstract3DAxis *axis) = 0;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void handleWindowChanged(QQuickWindow *win);
    virtual void itemChange(ItemChange change, const ItemChangeData &value);
    virtual void updateWindowParameters();
    virtual void handleSelectionModeChange(QAbstract3DGraph::SelectionFlags mode);
    virtual void handleShadowQualityChange(QAbstract3DGraph::ShadowQuality quality);
    virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

signals:
    void selectionModeChanged(SelectionFlags mode);
    void shadowQualityChanged(ShadowQuality quality);
    void msaaSamplesChanged(int samples);
    void sceneChanged(Q3DScene *scene);
    void inputHandlerChanged(QAbstract3DInputHandler *inputHandler);
    void themeChanged(Q3DTheme *theme);
    void renderingModeChanged(RenderingMode mode);

private:
    QPointer<Abstract3DController> m_controller;
    QRectF m_cachedGeometry;
    QOpenGLContext *m_context;
    QOpenGLContext *m_qtContext;
    QQuickWindow *m_contextWindow;
    AbstractDeclarative::RenderingMode m_renderMode;
    int m_samples;
    QSize m_initialisedSize;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractDeclarative::SelectionFlags)

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

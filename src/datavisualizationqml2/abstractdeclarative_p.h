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
#include "qabstract3dinputhandler.h"

#include <QAbstractItemModel>
#include <QQuickItem>
#include <QObject>
#include <QQuickWindow>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class AbstractDeclarative : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::QDataVis::SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(Q3DScene* scene READ scene NOTIFY sceneChanged)
    Q_PROPERTY(QAbstract3DInputHandler* inputHandler READ inputHandler WRITE setInputHandler NOTIFY inputHandlerChanged)
    Q_PROPERTY(Q3DTheme* theme READ theme WRITE setTheme NOTIFY themeChanged)

public:
    explicit AbstractDeclarative(QQuickItem *parent = 0);
    virtual ~AbstractDeclarative();

    virtual Q3DScene *scene() const;

    virtual QAbstract3DInputHandler *inputHandler() const;
    virtual void setInputHandler(QAbstract3DInputHandler *inputHandler);

    virtual void setTheme(Q3DTheme *theme);
    virtual Q3DTheme *theme() const;

    virtual void setSelectionMode(QDataVis::SelectionFlags mode);
    virtual QDataVis::SelectionFlags selectionMode() const;

    virtual void setShadowQuality(QDataVis::ShadowQuality quality);
    virtual QDataVis::ShadowQuality shadowQuality() const;

    void setSharedController(Abstract3DController *controller);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *);

signals:
    // Signals shadow quality changes.
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void inputHandlerChanged(QAbstract3DInputHandler *inputHandler);
    void themeChanged(Q3DTheme *theme);
    void selectionModeChanged(QDataVis::SelectionFlags mode);
    void itemLabelFormatChanged(QString format);
    void sceneChanged(Q3DScene *scene);

private:
    Abstract3DController *m_controller;
    QSize m_initialisedSize;
    qreal m_devicePixelRatio;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // ABSTRACTDECLARATIVE_P_H

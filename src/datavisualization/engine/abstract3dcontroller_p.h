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

#ifndef CONTROLLER3DBASE_H
#define CONTROLLER3DBASE_H

#include "datavisualizationglobal_p.h"
#include "theme_p.h"
#include "q3dabstractaxis.h"
#include "drawer_p.h"
#include "qabstract3dinputhandler.h"
#include "qabstractdataproxy.h"
#include "q3dscene.h"
#include "q3dbox.h"

#include <QObject>

class QFont;

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class CameraHelper;
class Abstract3DRenderer;

struct Abstract3DChangeBitField {
    bool positionChanged             : 1;
    bool zoomLevelChanged            : 1;
    bool themeChanged                : 1;
    bool fontChanged                 : 1;
    bool labelStyleChanged           : 1;
    bool boundingRectChanged         : 1;
    bool sizeChanged                 : 1;
    bool shadowQualityChanged        : 1;
    bool selectionModeChanged        : 1;
    bool objFileChanged              : 1;
    bool gridEnabledChanged          : 1;
    bool backgroundEnabledChanged    : 1;
    bool axisXTypeChanged            : 1;
    bool axisYTypeChanged            : 1;
    bool axisZTypeChanged            : 1;
    bool axisXTitleChanged           : 1;
    bool axisYTitleChanged           : 1;
    bool axisZTitleChanged           : 1;
    bool axisXLabelsChanged          : 1;
    bool axisYLabelsChanged          : 1;
    bool axisZLabelsChanged          : 1;
    bool axisXRangeChanged           : 1;
    bool axisYRangeChanged           : 1;
    bool axisZRangeChanged           : 1;
    bool axisXSegmentCountChanged    : 1;
    bool axisYSegmentCountChanged    : 1;
    bool axisZSegmentCountChanged    : 1;
    bool axisXSubSegmentCountChanged : 1;
    bool axisYSubSegmentCountChanged : 1;
    bool axisZSubSegmentCountChanged : 1;
    bool axisXLabelFormatChanged     : 1;
    bool axisYLabelFormatChanged     : 1;
    bool axisZLabelFormatChanged     : 1;
    bool inputStateChanged           : 1;
    bool inputPositionChanged        : 1;

    Abstract3DChangeBitField() :
        positionChanged(true),
        zoomLevelChanged(true),
        themeChanged(true),
        fontChanged(true),
        labelStyleChanged(true),
        boundingRectChanged(true),
        sizeChanged(true),
        shadowQualityChanged(true),
        selectionModeChanged(true),
        objFileChanged(true),
        gridEnabledChanged(true),
        backgroundEnabledChanged(true),
        axisXTypeChanged(true),
        axisYTypeChanged(true),
        axisZTypeChanged(true),
        axisXTitleChanged(true),
        axisYTitleChanged(true),
        axisZTitleChanged(true),
        axisXLabelsChanged(true),
        axisYLabelsChanged(true),
        axisZLabelsChanged(true),
        axisXRangeChanged(true),
        axisYRangeChanged(true),
        axisZRangeChanged(true),
        axisXSegmentCountChanged(true),
        axisYSegmentCountChanged(true),
        axisZSegmentCountChanged(true),
        axisXSubSegmentCountChanged(true),
        axisYSubSegmentCountChanged(true),
        axisZSubSegmentCountChanged(true),
        axisXLabelFormatChanged(true),
        axisYLabelFormatChanged(true),
        axisZLabelFormatChanged(true),
        inputStateChanged(true),
        inputPositionChanged(true)
    {
    }
};

class QT_DATAVISUALIZATION_EXPORT Abstract3DController : public QObject
{
    Q_OBJECT

public:
    enum SelectionType {
        SelectionNone = 0,
        SelectionItem,
        SelectionRow,
        SelectionColumn
    };

    enum MouseState {
        MouseNone = 0,
        MouseOnScene,
        MouseOnOverview,
        MouseOnZoom,
        MouseRotating,
        MouseOnPinch
    };

private:
    Abstract3DChangeBitField m_changeTracker;
    QRect m_boundingRect;
    GLfloat m_horizontalRotation;
    GLfloat m_verticalRotation;
    Theme m_theme;
    QFont m_font;
    QDataVis::SelectionFlags m_selectionMode;
    QDataVis::ShadowQuality m_shadowQuality;
    QDataVis::LabelStyle m_labelStyle;
    bool m_isBackgroundEnabled;
    bool m_isGridEnabled;
    QString m_objFile;
    Q3DScene *m_scene;

protected:
    QList<QAbstract3DInputHandler *> m_inputHandlers; // List of all added input handlers
    QAbstract3DInputHandler *m_activeInputHandler;
    CameraHelper *m_cameraHelper;
    // Active axes
    Q3DAbstractAxis *m_axisX;
    Q3DAbstractAxis *m_axisY;
    Q3DAbstractAxis *m_axisZ;

    QList<Q3DAbstractAxis *> m_axes; // List of all added axes
    Abstract3DRenderer *m_renderer;
    bool m_isDataDirty;

    QAbstractDataProxy *m_data;
    QList<QAbstractDataProxy *> m_dataProxies;

    bool m_renderPending;

    explicit Abstract3DController(QRect boundRect, QObject *parent = 0);
    virtual ~Abstract3DController();

public:

    inline bool isInitialized() { return (m_renderer != 0); }

    /**
     * @brief synchDataToRenderer Called on the render thread while main GUI thread is blocked before rendering.
     */
    virtual void synchDataToRenderer();

    virtual void render(const GLuint defaultFboHandle = 0);

    /**
     * @brief setRenderer Sets the renderer to be used. isInitialized returns true from this point onwards.
     * @param renderer Renderer to be used.
     */
    void setRenderer(Abstract3DRenderer *renderer);

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

    virtual QRect primarySubViewport() const;
    virtual void setPrimarySubViewport(const QRect &primarySubViewport);

    virtual QRect secondarySubViewport() const;
    virtual void setSecondarySubViewport(const QRect &secondarySubViewport);

    virtual void setAxisX(Q3DAbstractAxis *axis);
    virtual Q3DAbstractAxis *axisX();
    virtual void setAxisY(Q3DAbstractAxis *axis);
    virtual Q3DAbstractAxis *axisY();
    virtual void setAxisZ(Q3DAbstractAxis *axis);
    virtual Q3DAbstractAxis *axisZ();
    virtual void addAxis(Q3DAbstractAxis *axis);
    virtual void releaseAxis(Q3DAbstractAxis *axis);
    virtual QList<Q3DAbstractAxis *> axes() const; // Omits default axes

    virtual void addInputHandler(QAbstract3DInputHandler *inputHandler);
    virtual void releaseInputHandler(QAbstract3DInputHandler *inputHandler);
    virtual void setActiveInputHandler(QAbstract3DInputHandler *inputHandler);
    virtual QAbstract3DInputHandler *activeInputHandler();

    virtual QAbstractDataProxy *activeDataProxy() const;
    virtual void addDataProxy(QAbstractDataProxy *proxy);
    virtual void releaseDataProxy(QAbstractDataProxy *proxy);
    virtual QList<QAbstractDataProxy *> dataProxies() const;
    virtual void setActiveDataProxy(QAbstractDataProxy *proxy);
    virtual void updateDevicePixelRatio(qreal ratio);

    virtual int zoomLevel();
    virtual void setZoomLevel(int zoomLevel);

    // Set color if you don't want to use themes.
    virtual void setObjectColor(const QColor &baseColor, bool uniform = true);
    virtual QColor objectColor() const;

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    virtual void setTheme(QDataVis::Theme theme);
    virtual Theme theme();

    // Set font
    virtual void setFont(const QFont &font);
    virtual QFont font();

    // Selection mode
    virtual void setSelectionMode(QDataVis::SelectionFlags mode);
    virtual QDataVis::SelectionFlags selectionMode();

    // Adjust shadow quality
    virtual void setShadowQuality(QDataVis::ShadowQuality quality);
    virtual QDataVis::ShadowQuality shadowQuality();

    // Label style adjustment
    virtual void setLabelStyle(QDataVis::LabelStyle style);
    virtual QDataVis::LabelStyle labelStyle();

    // Enable or disable background mesh
    virtual void setBackgroundEnabled(bool enable);
    virtual bool backgroundEnabled();

    // Enable or disable background grid
    virtual void setGridEnabled(bool enable);
    virtual bool gridEnabled();

    // Query input state and position
    QDataVis::InputState inputState();
    QPoint inputPosition();

    // Enable or disable slicing mode
    bool isSlicingActive();
    void setSlicingActive(bool isSlicing);

    // override bar type with own mesh
    virtual void setMeshFileName(const QString &fileName);
    virtual QString meshFileName();

    Q3DScene *scene();

    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);
    virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    virtual void wheelEvent(QWheelEvent *event);

    virtual void handleAxisTitleChangedBySender(QObject *sender);
    virtual void handleAxisLabelsChangedBySender(QObject *sender);
    virtual void handleAxisRangeChangedBySender(QObject *sender);
    virtual void handleAxisSegmentCountChangedBySender(QObject *sender);
    virtual void handleAxisSubSegmentCountChangedBySender(QObject *sender);
    virtual void handleAxisAutoAdjustRangeChangedInOrientation(
            Q3DAbstractAxis::AxisOrientation orientation, bool autoAdjust) = 0;
    virtual void handleAxisLabelFormatChangedBySender(QObject *sender);

public slots:
    void handleAxisTitleChanged(const QString &title);
    void handleAxisLabelsChanged();
    void handleAxisRangeChanged(qreal min, qreal max);
    void handleAxisSegmentCountChanged(int count);
    void handleAxisSubSegmentCountChanged(int count);
    void handleAxisAutoAdjustRangeChanged(bool autoAdjust);
    void handleAxisLabelFormatChanged(const QString &format);
    void handleInputStateChanged(QDataVis::InputState state);
    void handleInputPositionChanged(const QPoint &position);

signals:
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void activeInputHandlerChanged(QAbstract3DInputHandler *inputHandler);
    void needRender();

protected:
    virtual Q3DAbstractAxis *createDefaultAxis(Q3DAbstractAxis::AxisOrientation orientation);
    Q3DValueAxis *createDefaultValueAxis();
    Q3DCategoryAxis *createDefaultCategoryAxis();
    void emitNeedRender();

private:
    void setAxisHelper(Q3DAbstractAxis::AxisOrientation orientation, Q3DAbstractAxis *axis,
                       Q3DAbstractAxis **axisPtr);
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif // CONTROLLER3DBASE_H

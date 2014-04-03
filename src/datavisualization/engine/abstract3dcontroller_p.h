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

#ifndef ABSTRACT3DCONTROLLER_P_H
#define ABSTRACT3DCONTROLLER_P_H

#include "datavisualizationglobal_p.h"
#include "qabstract3daxis.h"
#include "drawer_p.h"
#include "qabstract3dinputhandler.h"
#include "qabstractdataproxy.h"
#include "q3dscene_p.h"
#include <QtGui/QLinearGradient>

class QFont;
class QOpenGLFramebufferObject;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class CameraHelper;
class Abstract3DRenderer;
class QAbstract3DSeries;
class ThemeManager;

struct Abstract3DChangeBitField {
    bool zoomLevelChanged              : 1;
    bool themeChanged                  : 1;
    bool shadowQualityChanged          : 1;
    bool selectionModeChanged          : 1;
    bool objFileChanged                : 1;
    bool axisXTypeChanged              : 1;
    bool axisYTypeChanged              : 1;
    bool axisZTypeChanged              : 1;
    bool axisXTitleChanged             : 1;
    bool axisYTitleChanged             : 1;
    bool axisZTitleChanged             : 1;
    bool axisXLabelsChanged            : 1;
    bool axisYLabelsChanged            : 1;
    bool axisZLabelsChanged            : 1;
    bool axisXRangeChanged             : 1;
    bool axisYRangeChanged             : 1;
    bool axisZRangeChanged             : 1;
    bool axisXSegmentCountChanged      : 1;
    bool axisYSegmentCountChanged      : 1;
    bool axisZSegmentCountChanged      : 1;
    bool axisXSubSegmentCountChanged   : 1;
    bool axisYSubSegmentCountChanged   : 1;
    bool axisZSubSegmentCountChanged   : 1;
    bool axisXLabelFormatChanged       : 1;
    bool axisYLabelFormatChanged       : 1;
    bool axisZLabelFormatChanged       : 1;
    bool axisXFormatterChanged         : 1;
    bool axisYFormatterChanged         : 1;
    bool axisZFormatterChanged         : 1;

    Abstract3DChangeBitField() :
        zoomLevelChanged(true),
        themeChanged(true),
        shadowQualityChanged(true),
        selectionModeChanged(true),
        objFileChanged(true),
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
        axisXFormatterChanged(true),
        axisYFormatterChanged(true),
        axisZFormatterChanged(true)
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
    GLfloat m_horizontalRotation;
    GLfloat m_verticalRotation;
    ThemeManager *m_themeManager;
    QAbstract3DGraph::SelectionFlags m_selectionMode;
    QAbstract3DGraph::ShadowQuality m_shadowQuality;

protected:
    Q3DScene *m_scene;
    QList<QAbstract3DInputHandler *> m_inputHandlers; // List of all added input handlers
    QAbstract3DInputHandler *m_activeInputHandler;
    CameraHelper *m_cameraHelper;
    // Active axes
    QAbstract3DAxis *m_axisX;
    QAbstract3DAxis *m_axisY;
    QAbstract3DAxis *m_axisZ;

    QList<QAbstract3DAxis *> m_axes; // List of all added axes
    Abstract3DRenderer *m_renderer;
    bool m_isDataDirty;
    bool m_isSeriesVisibilityDirty;
    bool m_isSeriesVisualsDirty;
    bool m_renderPending;

    QList<QAbstract3DSeries *> m_seriesList;

    explicit Abstract3DController(QRect initialViewport, Q3DScene *scene, QObject *parent = 0);

public:
    virtual ~Abstract3DController();

    inline bool isInitialized() { return (m_renderer != 0); }
    virtual void destroyRenderer();
    virtual void synchDataToRenderer();
    virtual void render(const GLuint defaultFboHandle = 0);
    virtual void initializeOpenGL() = 0;
    void setRenderer(Abstract3DRenderer *renderer);

    virtual void addSeries(QAbstract3DSeries *series);
    virtual void insertSeries(int index, QAbstract3DSeries *series);
    virtual void removeSeries(QAbstract3DSeries *series);
    QList<QAbstract3DSeries *> seriesList();

    virtual void setAxisX(QAbstract3DAxis *axis);
    virtual QAbstract3DAxis *axisX();
    virtual void setAxisY(QAbstract3DAxis *axis);
    virtual QAbstract3DAxis *axisY();
    virtual void setAxisZ(QAbstract3DAxis *axis);
    virtual QAbstract3DAxis *axisZ();
    virtual void addAxis(QAbstract3DAxis *axis);
    virtual void releaseAxis(QAbstract3DAxis *axis);
    virtual QList<QAbstract3DAxis *> axes() const; // Omits default axes

    virtual void addInputHandler(QAbstract3DInputHandler *inputHandler);
    virtual void releaseInputHandler(QAbstract3DInputHandler *inputHandler);
    virtual void setActiveInputHandler(QAbstract3DInputHandler *inputHandler);
    virtual QAbstract3DInputHandler *activeInputHandler();
    virtual QList<QAbstract3DInputHandler *> inputHandlers() const;

    virtual int zoomLevel();
    virtual void setZoomLevel(int zoomLevel);

    virtual void addTheme(Q3DTheme *theme);
    virtual void releaseTheme(Q3DTheme *theme);
    virtual void setActiveTheme(Q3DTheme *theme);
    virtual Q3DTheme *activeTheme() const;
    virtual QList<Q3DTheme *> themes() const;

    virtual void setSelectionMode(QAbstract3DGraph::SelectionFlags mode);
    virtual QAbstract3DGraph::SelectionFlags selectionMode() const;

    virtual void setShadowQuality(QAbstract3DGraph::ShadowQuality quality);
    virtual QAbstract3DGraph::ShadowQuality shadowQuality() const;
    virtual bool shadowsSupported() const;

    bool isSlicingActive() const;
    void setSlicingActive(bool isSlicing);

    Q3DScene *scene();

    void markDataDirty();
    void markSeriesVisualsDirty();

    void requestRender(QOpenGLFramebufferObject *fbo);

    void emitNeedRender();

    virtual void clearSelection() = 0;

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
            QAbstract3DAxis::AxisOrientation orientation, bool autoAdjust) = 0;
    virtual void handleAxisLabelFormatChangedBySender(QObject *sender);
    virtual void handleAxisFormatterDirtyBySender(QObject *sender);
    virtual void handleSeriesVisibilityChangedBySender(QObject *sender);
    virtual void handlePendingClick() = 0;

    void markSeriesItemLabelsDirty();

public slots:
    void handleAxisTitleChanged(const QString &title);
    void handleAxisLabelsChanged();
    void handleAxisRangeChanged(float min, float max);
    void handleAxisSegmentCountChanged(int count);
    void handleAxisSubSegmentCountChanged(int count);
    void handleAxisAutoAdjustRangeChanged(bool autoAdjust);
    void handleAxisLabelFormatChanged(const QString &format);
    void handleAxisFormatterDirty();
    void handleInputViewChanged(QAbstract3DInputHandler::InputView view);
    void handleInputPositionChanged(const QPoint &position);
    void handleSeriesVisibilityChanged(bool visible);

    void handleThemeColorStyleChanged(Q3DTheme::ColorStyle style);
    void handleThemeBaseColorsChanged(const QList<QColor> &color);
    void handleThemeBaseGradientsChanged(const QList<QLinearGradient> &gradient);
    void handleThemeSingleHighlightColorChanged(const QColor &color);
    void handleThemeSingleHighlightGradientChanged(const QLinearGradient &gradient);
    void handleThemeMultiHighlightColorChanged(const QColor &color);
    void handleThemeMultiHighlightGradientChanged(const QLinearGradient &gradient);
    void handleThemeTypeChanged(Q3DTheme::Theme theme);

    // Renderer callback handlers
    void handleRequestShadowQuality(QAbstract3DGraph::ShadowQuality quality);

signals:
    void shadowQualityChanged(QAbstract3DGraph::ShadowQuality quality);
    void activeInputHandlerChanged(QAbstract3DInputHandler *inputHandler);
    void activeThemeChanged(Q3DTheme *activeTheme);
    void selectionModeChanged(QAbstract3DGraph::SelectionFlags mode);
    void needRender();
    void axisXChanged(QAbstract3DAxis *axis);
    void axisYChanged(QAbstract3DAxis *axis);
    void axisZChanged(QAbstract3DAxis *axis);
    void elementSelected(QAbstract3DGraph::ElementType type);

protected:
    virtual QAbstract3DAxis *createDefaultAxis(QAbstract3DAxis::AxisOrientation orientation);
    QValue3DAxis *createDefaultValueAxis();
    QCategory3DAxis *createDefaultCategoryAxis();
    virtual void startRecordingRemovesAndInserts();

private:
    void setAxisHelper(QAbstract3DAxis::AxisOrientation orientation, QAbstract3DAxis *axis,
                       QAbstract3DAxis **axisPtr);

    friend class Bars3DController;
    friend class QAbstract3DGraphPrivate;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

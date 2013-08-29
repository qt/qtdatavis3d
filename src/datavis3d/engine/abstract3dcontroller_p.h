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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef CONTROLLER3DBASE_H
#define CONTROLLER3DBASE_H

#include <QObject>

#include "datavis3dglobal_p.h"
#include "theme_p.h"
#include "qabstractaxis.h"
#include "drawer_p.h"
#include "qabstract3dinputhandler.h"
#include "qabstractdataproxy.h"

class QFont;

QT_DATAVIS3D_BEGIN_NAMESPACE

class CameraHelper;
class Abstract3DRenderer;

struct Abstract3DChangeBitField {
    bool positionChanged             : 1;
    bool zoomLevelChanged            : 1;
    bool themeChanged                : 1;
    bool fontChanged                 : 1;
    bool labelTransparencyChanged    : 1;
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

    Abstract3DChangeBitField() :
        positionChanged(true),
        zoomLevelChanged(true),
        themeChanged(true),
        fontChanged(true),
        labelTransparencyChanged(true),
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
        axisZLabelFormatChanged(true)
    {
    }
};

class QT_DATAVIS3D_EXPORT Abstract3DController : public QObject
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
    QDataVis::SelectionMode m_selectionMode;
    QDataVis::ShadowQuality m_shadowQuality;
    QDataVis::LabelTransparency m_labelTransparency;
    bool m_isBackgroundEnabled;
    bool m_isGridEnabled;
    QString m_objFile;

protected:
    QAbstract3DInputHandler *m_inputHandler;
    CameraHelper *m_cameraHelper;
    int m_zoomLevel;
    // Active axes
    QAbstractAxis *m_axisX;
    QAbstractAxis *m_axisY;
    QAbstractAxis *m_axisZ;

    QList<QAbstractAxis *> m_axes; // List of all added axes
    Abstract3DRenderer *m_renderer;
    bool m_isDataDirty;

    QAbstractDataProxy *m_data;
    QList<QAbstractDataProxy *> m_dataProxies;

    explicit Abstract3DController(QRect boundRect, QObject *parent = 0);
    ~Abstract3DController();

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
    virtual void setAxisX(QAbstractAxis *axis);
    virtual QAbstractAxis *axisX();
    virtual void setAxisY(QAbstractAxis *axis);
    virtual QAbstractAxis *axisY();
    virtual void setAxisZ(QAbstractAxis *axis);
    virtual QAbstractAxis *axisZ();
    virtual void addAxis(QAbstractAxis *axis);
    virtual void releaseAxis(QAbstractAxis *axis);
    virtual QList<QAbstractAxis *> axes() const; // Omits default axes

    virtual QAbstractDataProxy *activeDataProxy() const;
    virtual void addDataProxy(QAbstractDataProxy *proxy);
    virtual void releaseDataProxy(QAbstractDataProxy *proxy);
    virtual QList<QAbstractDataProxy *> dataProxies() const;
    virtual void setActiveDataProxy(QAbstractDataProxy *proxy);

    virtual int zoomLevel();
    virtual void setZoomLevel(int zoomLevel);

    // Select preset camera placement
    virtual void setCameraPreset(QDataVis::CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) (or (-90...90) if there are negative values) angles and distance in
    // percentage (10...500))
    virtual void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height)
    // color to change from bottom to top
    virtual void setObjectColor(const QColor &baseColor, const QColor &heightColor,
                                const QColor &depthColor, bool uniform = true);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text
    // colors are affected)
    virtual void setColorTheme(QDataVis::ColorTheme colorTheme);
    virtual Theme theme();

    // Set font
    virtual void setFont(const QFont &font);
    virtual QFont font();

    // Selection mode
    virtual void setSelectionMode(QDataVis::SelectionMode mode);
    virtual QDataVis::SelectionMode selectionMode();

    // Adjust shadow quality
    virtual void setShadowQuality(QDataVis::ShadowQuality quality);
    virtual QDataVis::ShadowQuality shadowQuality();

    // Label transparency adjustment
    virtual void setLabelTransparency(QDataVis::LabelTransparency transparency);
    virtual QDataVis::LabelTransparency labelTransparency();

    // Enable or disable background mesh
    virtual void setBackgroundEnabled(bool enable);
    virtual bool backgroundEnabled();

    // Enable or disable background grid
    virtual void setGridEnabled(bool enable);
    virtual bool gridEnabled();

    // override bar type with own mesh
    virtual void setMeshFileName(const QString &fileName);
    virtual QString meshFileName();

    virtual void handleAxisTitleChangedBySender(QObject *sender);
    virtual void handleAxisLabelsChangedBySender(QObject *sender);
    virtual void handleAxisRangeChangedBySender(QObject *sender);
    virtual void handleAxisSegmentCountChangedBySender(QObject *sender);
    virtual void handleAxisSubSegmentCountChangedBySender(QObject *sender);
    virtual void handleAxisAutoAdjustRangeChangedInOrientation(
            QAbstractAxis::AxisOrientation orientation, bool autoAdjust) = 0;
    virtual void handleAxisLabelFormatChangedBySender(QObject *sender);

public slots:
    void handleAxisTitleChanged(const QString &title);
    void handleAxisLabelsChanged();
    void handleAxisRangeChanged(qreal min, qreal max);
    void handleAxisSegmentCountChanged(int count);
    void handleAxisSubSegmentCountChanged(int count);
    void handleAxisAutoAdjustRangeChanged(bool autoAdjust);
    void handleAxisLabelFormatChanged(const QString &format);

signals:
    void boundingRectChanged(QRect boundingRect);
    void sizeChanged(QRect boundingRect);
    void positionChanged(QRect boundingRect);
    void zoomLevelChanged(int zoomLevel);
    void themeChanged(Theme theme);
    void fontChanged(QFont font); // TODO should be handled via axis?? What about font for selection label?
    void shadowQualityChanged(QDataVis::ShadowQuality quality);
    void labelTransparencyChanged(QDataVis::LabelTransparency transparency);
    void selectionModeChanged(QDataVis::SelectionMode mode);
    void backgroundEnabledChanged(bool enable);
    void gridEnabledChanged(bool enable); // TODO: Should be handled via axes?
    void meshFileNameChanged(QString fileName);

protected:
    virtual QAbstractAxis *createDefaultAxis(QAbstractAxis::AxisOrientation orientation);
    QValueAxis *createDefaultValueAxis();
    QCategoryAxis *createDefaultCategoryAxis();

private:
    void setAxisHelper(QAbstractAxis::AxisOrientation orientation, QAbstractAxis *axis,
                       QAbstractAxis **axisPtr);
};

QT_DATAVIS3D_END_NAMESPACE

#endif // CONTROLLER3DBASE_H

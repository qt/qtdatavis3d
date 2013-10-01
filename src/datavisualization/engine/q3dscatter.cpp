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

#include "q3dscatter.h"
#include "q3dscatter_p.h"
#include "scatter3dcontroller_p.h"
#include "q3dvalueaxis.h"
#include "qscatterdataproxy.h"

#include <QMouseEvent>

#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DScatter
 * \inmodule QtDataVisualization
 * \brief The Q3DScatter class provides methods for rendering 3D scatter graphs.
 * \since 1.0.0
 *
 * This class enables developers to render scatter graphs in 3D and to view them by rotating the scene
 * freely. Rotation is done by holding down the right mouse button and moving the mouse. Zooming
 * is done by mouse wheel. Selection, if enabled, is done by left mouse button. The scene can be
 * reset to default camera view by clicking mouse wheel. In touch devices rotation is done
 * by tap-and-move, selection by tap-and-hold and zoom by pinch.
 *
 * If no axes are explicitly set to Q3DScatter, temporary default axes with no labels are created.
 * These default axes can be modified via axis accessors, but as soon any axis is explicitly
 * set for the orientation, the default axis for that orientation is destroyed.
 *
 * Data proxies work similarly: If no data proxy is explicitly set, Q3DScatter creates a default
 * proxy. If any other proxy is set as active data proxy later, the default proxy and all data
 * added to it is destroyed.
 *
 * Methods are provided for changing item styles, themes, item selection modes and so on. See the
 * methods for more detailed descriptions.
 *
 * \section1 How to construct a minimal Q3DScatter chart
 *
 * First, construct Q3DScatter:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 0
 *
 * Now Q3DScatter is ready to receive data to be rendered. Add one set of 3 QVector3D items:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 1
 *
 * Finally you will need to set it visible:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 2
 *
 * The complete code needed to create and display this chart is:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 3
 *
 * And this is what those few lines of code produce:
 *
 * \image q3dscatter-minimal.png
 *
 * The scene can be rotated and zoomed into, but no other interaction is included in this minimal
 * code example. You can learn more by familiarizing yourself with the examples provided, like
 * the \l{Scatter Chart Example}.
 *
 * \sa Q3DBars, Q3DSurface, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs a new 3D scatter window.
 */
Q3DScatter::Q3DScatter()
    : d_ptr(new Q3DScatterPrivate(this, geometry()))
{
    setVisualController(d_ptr->m_shared);
    d_ptr->m_shared->initializeOpenGL();
    QObject::connect(d_ptr->m_shared, &Scatter3DController::selectedItemIndexChanged, this,
                     &Q3DScatter::selectedItemIndexChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::needRender, this,
                     &Q3DWindow::renderLater);
}

/*!
 *  Destroys the 3D scatter window.
 */
Q3DScatter::~Q3DScatter()
{
}

/*!
 * \internal
 */
void Q3DScatter::mouseDoubleClickEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseDoubleClickEvent(event);
}

/*!
 * \internal
 */
void Q3DScatter::touchEvent(QTouchEvent *event)
{
    d_ptr->m_shared->touchEvent(event);
}

/*!
 * \internal
 */
void Q3DScatter::mousePressEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mousePressEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DScatter::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseReleaseEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DScatter::mouseMoveEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseMoveEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DScatter::wheelEvent(QWheelEvent *event)
{
    d_ptr->m_shared->wheelEvent(event);
}

/*!
 * \internal
 */
void Q3DScatter::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    d_ptr->m_shared->setSize(width(), height());
}

/*!
 * Sets window \a width.
 */
void Q3DScatter::setWidth(const int width)
{
    d_ptr->m_shared->setWidth(width);
    QWindow::setWidth(width);
}

/*!
 * Sets window \a height.
 */
void Q3DScatter::setHeight(const int height)
{
    d_ptr->m_shared->setHeight(height);
    QWindow::setHeight(height);
}

/*!
 * Sets the item \a style to one of the values in \c QDataVis::MeshStyle. It is preset to
 * \c QDataVis::Spheres by default. A \a smooth flag can be used to set shading to smooth.
 * It is \c false by default.
 *
 * \sa setMeshFileName()
 */
void Q3DScatter::setObjectType(QDataVis::MeshStyle style, bool smooth)
{
    d_ptr->m_shared->setObjectType(style, smooth);
}

/*!
 * \property Q3DScatter::cameraPreset
 *
 * The \a preset position of the camera. The position can be one of \c QDataVis::CameraPreset.
 */
void Q3DScatter::setCameraPreset(QDataVis::CameraPreset preset)
{
    d_ptr->m_shared->setCameraPreset(preset);
}

QDataVis::CameraPreset Q3DScatter::cameraPreset() const
{
    return d_ptr->m_shared->cameraPreset();
}

/*!
 * Move camera to a wanted position based on \a horizontal and \a vertical angles. Angles are limited
 * to -180...180 in horizontal direction and -90...90 in vertical. \a distance is adjustable
 * between 10 and 500, being \c 100 by default.
 */
void Q3DScatter::setCameraPosition(qreal horizontal, qreal vertical, int distance)
{
    d_ptr->m_shared->setCameraPosition(GLfloat(horizontal), GLfloat(vertical), GLint(distance));
}

/*!
 * Sets a predefined \a theme from \c QDataVis::ColorTheme. It is preset to \c QDataVis::ThemeQt by
 * default. Theme affects bar colors, label colors, text color, background color, window color and
 * grid color. Lighting is also adjusted by themes.
 *
 * \sa setObjectColor()
 *
 * \warning This method is subject to change.
 */
void Q3DScatter::setTheme(QDataVis::ColorTheme theme)
{
    d_ptr->m_shared->setColorTheme(theme);
}

/*!
 * Set item color using your own colors. \a baseColor sets the base color of a item. The \a uniform
 * -flag is used to define if color needs to be uniform throughout item's length, or will the colors
 * be applied by height. It is \c true by default.
 *
 * Calling this method overrides colors from theme.
 *
 * \sa setTheme()
 *
 * \warning This method is subject to change.
 */
void Q3DScatter::setObjectColor(const QColor &baseColor, bool uniform)
{
    d_ptr->m_shared->setObjectColor(baseColor, uniform);
}

/*!
 * \return item color in use.
 */
QColor Q3DScatter::objectColor() const
{
    return d_ptr->m_shared->objectColor();
}

/*!
 * \property Q3DScatter::selectionMode
 *
 * Sets item selection \a mode to one of \c QDataVis::SelectionMode. It is preset to
 * \c QDataVis::ModeItem by default.
 */
void Q3DScatter::setSelectionMode(QDataVis::SelectionMode mode)
{
    d_ptr->m_shared->setSelectionMode(mode);
}

QDataVis::SelectionMode Q3DScatter::selectionMode() const
{
    return d_ptr->m_shared->selectionMode();
}

/*!
 * \property Q3DScatter::meshFileName
 *
 * Override item type with a mesh object located in \a objFileName.
 * \note Object needs to be in Wavefront obj format and include vertices, normals and UVs.
 * It also needs to be in triangles.
 *
 * \sa setObjectType()
 */
void Q3DScatter::setMeshFileName(const QString &objFileName)
{
    d_ptr->m_shared->setMeshFileName(objFileName);
}

QString Q3DScatter::meshFileName() const
{
    return d_ptr->m_shared->meshFileName();
}

/*!
 * \property Q3DScatter::font
 *
 * Sets the \a font for labels. It is preset to \c Arial by default.
 */
void Q3DScatter::setFont(const QFont &font)
{
    d_ptr->m_shared->setFont(font);
}

QFont Q3DScatter::font() const
{
    return d_ptr->m_shared->font();
}

/*!
 * \property Q3DScatter::labelTransparency
 *
 * Sets label \a transparency to one of \c QDataVis::LabelTransparency. It is preset to
 * \c QDataVis::TransparencyFromTheme by default.
 */
void Q3DScatter::setLabelTransparency(QDataVis::LabelTransparency transparency)
{
    d_ptr->m_shared->setLabelTransparency(transparency);
}

QDataVis::LabelTransparency Q3DScatter::labelTransparency() const
{
    return d_ptr->m_shared->labelTransparency();
}

/*!
 * \property Q3DScatter::gridVisible
 *
 * Sets grid visibility to \a visible. It is preset to \c true by default.
 */
void Q3DScatter::setGridVisible(bool visible)
{
    d_ptr->m_shared->setGridEnabled(visible);
}

bool Q3DScatter::isGridVisible() const
{
    return d_ptr->m_shared->gridEnabled();
}

/*!
 * \property Q3DScatter::backgroundVisible
 *
 * Sets background visibility to \a visible. It is preset to \c true by default.
 */
void Q3DScatter::setBackgroundVisible(bool visible)
{
    d_ptr->m_shared->setBackgroundEnabled(visible);
}

bool Q3DScatter::isBackgroundVisible() const
{
    return d_ptr->m_shared->backgroundEnabled();
}

/*!
 * \property Q3DScatter::selectedItemIndex
 *
 * Selects an item in the \a index. Only one item can be selected at a time.
 * To clear selection, specify an illegal \a index, e.g. -1.
 */
void Q3DScatter::setSelectedItemIndex(int index)
{
    d_ptr->m_shared->setSelectedItemIndex(index);
}

int Q3DScatter::selectedItemIndex() const
{
    return d_ptr->m_shared->selectedItemIndex();
}

/*!
 * \property Q3DScatter::shadowQuality
 *
 * Sets shadow \a quality to one of \c QDataVis::ShadowQuality. It is preset to
 * \c QDataVis::ShadowMedium by default.
 *
 * \note If setting QDataVis::ShadowQuality of a certain level fails, a level is lowered
 * until it is successful and shadowQualityChanged signal is emitted for each time the change is
 * done.
 */
void Q3DScatter::setShadowQuality(QDataVis::ShadowQuality quality)
{
    return d_ptr->m_shared->setShadowQuality(quality);
}

QDataVis::ShadowQuality Q3DScatter::shadowQuality() const
{
    return d_ptr->m_shared->shadowQuality();
}

/*!
 * Sets a user-defined X-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is explicitly set to same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DScatter::setAxisX(Q3DValueAxis *axis)
{
    d_ptr->m_shared->setAxisX(axis);
}

/*!
 * \return used X-axis.
 */
Q3DValueAxis *Q3DScatter::axisX() const
{
    return static_cast<Q3DValueAxis *>(d_ptr->m_shared->axisX());
}

/*!
 * Sets a user-defined Y-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is explicitly set to same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DScatter::setAxisY(Q3DValueAxis *axis)
{
    d_ptr->m_shared->setAxisY(axis);
}

/*!
 * \return used Y-axis.
 */
Q3DValueAxis *Q3DScatter::axisY() const
{
    return static_cast<Q3DValueAxis *>(d_ptr->m_shared->axisY());
}

/*!
 * Sets a user-defined Z-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is explicitly set to same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DScatter::setAxisZ(Q3DValueAxis *axis)
{
    d_ptr->m_shared->setAxisZ(axis);
}

/*!
 * \return used Z-axis.
 */
Q3DValueAxis *Q3DScatter::axisZ() const
{
    return static_cast<Q3DValueAxis *>(d_ptr->m_shared->axisZ());
}

/*!
 * Adds \a axis to the graph. The axes added via addAxis are not yet taken to use,
 * addAxis is simply used to give the ownership of the \a axis to the graph.
 * The \a axis must not be null or added to another graph.
 *
 * \sa releaseAxis(), setAxisX(), setAxisY(), setAxisZ()
 */
void Q3DScatter::addAxis(Q3DValueAxis *axis)
{
    d_ptr->m_shared->addAxis(axis);
}

/*!
 * Releases the ownership of the \a axis back to the caller, if it is added to this graph.
 * If the released \a axis is in use, a new default axis will be created and set active.
 *
 * If the default axis is released and added back later, it behaves as any other axis would.
 *
 * \sa addAxis(), setAxisX(), setAxisY(), setAxisZ()
 */
void Q3DScatter::releaseAxis(Q3DValueAxis *axis)
{
    d_ptr->m_shared->releaseAxis(axis);
}

/*!
 * \return list of all added axes.
 *
 * \sa addAxis()
 */
QList<Q3DValueAxis *> Q3DScatter::axes() const
{
    QList<Q3DAbstractAxis *> abstractAxes = d_ptr->m_shared->axes();
    QList<Q3DValueAxis *> retList;
    foreach (Q3DAbstractAxis *axis, abstractAxes)
        retList.append(static_cast<Q3DValueAxis *>(axis));

    return retList;
}

/*!
 * Sets the active data \a proxy. Implicitly calls addDataProxy() to transfer ownership of
 * the \a proxy to this graph.
 *
 * If the \a proxy is null, a temporary default proxy is created and activated.
 * This temporary proxy is destroyed if another \a proxy is explicitly set active via this method.
 *
 * \sa addDataProxy(), releaseDataProxy()
 */
void Q3DScatter::setActiveDataProxy(QScatterDataProxy *proxy)
{
    d_ptr->m_shared->setActiveDataProxy(proxy);
}

/*!
 * \return active data proxy.
 */
QScatterDataProxy *Q3DScatter::activeDataProxy() const
{
    return static_cast<QScatterDataProxy *>(d_ptr->m_shared->activeDataProxy());
}

/*!
 * Adds data \a proxy to the graph. The proxies added via addDataProxy are not yet taken to use,
 * addDataProxy is simply used to give the ownership of the data \a proxy to the graph.
 * The \a proxy must not be null or added to another graph.
 *
 * \sa releaseDataProxy(), setActiveDataProxy()
 */
void Q3DScatter::addDataProxy(QScatterDataProxy *proxy)
{
    d_ptr->m_shared->addDataProxy(proxy);
}

/*!
 * Releases the ownership of the data \a proxy back to the caller, if it is added to this graph.
 * If the released \a proxy is in use, a new empty default proxy is created and taken to use.
 *
 * If the default \a proxy is released and added back later, it behaves as any other proxy would.
 *
 * \sa addDataProxy(), setActiveDataProxy()
 */
void Q3DScatter::releaseDataProxy(QScatterDataProxy *proxy)
{
    d_ptr->m_shared->releaseDataProxy(proxy);
}

/*!
 * \return list of all added data proxies.
 *
 * \sa addDataProxy()
 */
QList<QScatterDataProxy *> Q3DScatter::dataProxies() const
{
    QList<QScatterDataProxy *> retList;
    QList<QAbstractDataProxy *> abstractList = d_ptr->m_shared->dataProxies();
    foreach (QAbstractDataProxy *proxy, abstractList)
        retList.append(static_cast<QScatterDataProxy *>(proxy));

    return retList;
}

/*!
 * \fn void Q3DScatter::shadowQualityChanged(QDataVis::ShadowQuality quality)
 *
 * This signal is emitted when shadow \a quality changes.
 */

Q3DScatterPrivate::Q3DScatterPrivate(Q3DScatter *q, QRect rect)
    : q_ptr(q),
      m_shared(new Scatter3DController(rect))
{
    QObject::connect(m_shared, &Abstract3DController::shadowQualityChanged, this,
                     &Q3DScatterPrivate::handleShadowQualityUpdate);
}

Q3DScatterPrivate::~Q3DScatterPrivate()
{
    qDebug() << "Destroying Q3DScatterPrivate";
    delete m_shared;
}

void Q3DScatterPrivate::handleShadowQualityUpdate(QDataVis::ShadowQuality quality)
{
    emit q_ptr->shadowQualityChanged(quality);
}

QT_DATAVISUALIZATION_END_NAMESPACE


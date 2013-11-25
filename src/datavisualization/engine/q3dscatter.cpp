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
#include "q3dcamera.h"
#include "qscatter3dseries_p.h"

#include <QMouseEvent>
#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DScatter
 * \inmodule QtDataVisualization
 * \brief The Q3DScatter class provides methods for rendering 3D scatter graphs.
 * \since Qt Data Visualization 1.0
 *
 * This class enables developers to render scatter graphs in 3D and to view them by rotating the scene
 * freely. Rotation is done by holding down the right mouse button and moving the mouse. Zooming
 * is done by mouse wheel. Selection, if enabled, is done by left mouse button. The scene can be
 * reset to default camera view by clicking mouse wheel. In touch devices rotation is done
 * by tap-and-move, selection by tap-and-hold and zoom by pinch.
 *
 * If no axes are set explicitly to Q3DScatter, temporary default axes with no labels are created.
 * These default axes can be modified via axis accessors, but as soon any axis is set explicitly
 * for the orientation, the default axis for that orientation is destroyed.
 *
 * Q3DScatter supports more than one series visible at the same time.
 *
 * Methods are provided for changing themes, item selection modes and so on. See the
 * methods for more detailed descriptions.
 *
 * \section1 How to construct a minimal Q3DScatter graph
 *
 * First, construct Q3DScatter:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 0
 *
 * Now Q3DScatter is ready to receive data to be rendered. Add one series of 3 QVector3D items:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 1
 *
 * Finally you will need to set it visible:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 2
 *
 * The complete code needed to create and display this graph is:
 *
 * \snippet doc_src_q3dscatter_construction.cpp 3
 *
 * And this is what those few lines of code produce:
 *
 * \image q3dscatter-minimal.png
 *
 * The scene can be rotated and zoomed into, but no other interaction is included in this minimal
 * code example. You can learn more by familiarizing yourself with the examples provided, like
 * the \l{Scatter Example}.
 *
 * \sa Q3DBars, Q3DSurface, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs a new 3D scatter graph.
 */
Q3DScatter::Q3DScatter()
    : d_ptr(new Q3DScatterPrivate(this, geometry()))
{
    setVisualController(d_ptr->m_shared);
    d_ptr->m_shared->initializeOpenGL();
    QObject::connect(d_ptr->m_shared, &Abstract3DController::selectionModeChanged, this,
                     &Q3DScatter::selectionModeChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::labelStyleChanged, this,
                     &Q3DScatter::labelStyleChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::shadowQualityChanged, this,
                     &Q3DScatter::shadowQualityChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::meshFileNameChanged, this,
                     &Q3DScatter::meshFileNameChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::fontChanged, this,
                     &Q3DScatter::fontChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::themeChanged, this,
                     &Q3DScatter::themeChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::gridVisibleChanged, this,
                     &Q3DScatter::gridVisibleChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::backgroundVisibleChanged, this,
                     &Q3DScatter::backgroundVisibleChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::colorStyleChanged, this,
                     &Q3DScatter::colorStyleChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::objectColorChanged, this,
                     &Q3DScatter::itemColorChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::objectGradientChanged, this,
                     &Q3DScatter::itemGradientChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::singleHighlightColorChanged, this,
                     &Q3DScatter::singleHighlightColorChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::singleHighlightGradientChanged, this,
                     &Q3DScatter::singleHighlightGradientChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::multiHighlightColorChanged, this,
                     &Q3DScatter::multiHighlightColorChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::multiHighlightGradientChanged, this,
                     &Q3DScatter::multiHighlightGradientChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::needRender, this,
                     &Q3DWindow::renderLater);
}

/*!
 * Destroys the 3D scatter graph.
 */
Q3DScatter::~Q3DScatter()
{
}

/*!
 * Adds the \a series to the graph. A graph can contain multiple series, but has only one set of
 * axes. If the newly added series has specified a selected item, it will be highlighted and
 * any existing selection will be cleared. Only one added series can have an active selection.
 */
void Q3DScatter::addSeries(QScatter3DSeries *series)
{
    d_ptr->m_shared->addSeries(series);
}

/*!
 * Removes the \a series from the graph.
 */
void Q3DScatter::removeSeries(QScatter3DSeries *series)
{
    d_ptr->m_shared->removeSeries(series);
}

/*!
 * \return list of series added to this graph.
 */
QList<QScatter3DSeries *> Q3DScatter::seriesList()
{
    return d_ptr->m_shared->scatterSeriesList();
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
 * \c QDataVis::MeshStyleSpheres by default. A \a smooth flag can be used to set shading to smooth.
 * It is \c false by default.
 *
 * \sa setMeshFileName()
 */
void Q3DScatter::setObjectType(QDataVis::MeshStyle style, bool smooth)
{
    d_ptr->m_shared->setObjectType(style, smooth);
}

/*!
 * \property Q3DScatter::theme
 *
 * TODO: Add docs
 */
void Q3DScatter::setTheme(Q3DTheme *theme)
{
    d_ptr->m_shared->setTheme(theme);
}

Q3DTheme *Q3DScatter::theme() const
{
    return d_ptr->m_shared->theme();
}

/*!
 * \property Q3DScatter::selectionMode
 *
 * Sets item selection \a mode to one of \c QDataVis::SelectionMode. It is preset to
 * \c QDataVis::SelectionItem by default.
 *
 * \note Only \c QDataVis::SelectionItem and \c QDataVis::SelectionNone are supported.
 */
void Q3DScatter::setSelectionMode(QDataVis::SelectionFlags mode)
{
    d_ptr->m_shared->setSelectionMode(mode);
}

QDataVis::SelectionFlags Q3DScatter::selectionMode() const
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
 * \property Q3DScatter::scene
 *
 * This property contains the read only Q3DScene that can be used to access e.g. camera object.
 */
Q3DScene *Q3DScatter::scene() const
{
    return d_ptr->m_shared->scene();
}

/*!
 * \property Q3DScatter::labelStyle
 *
 * Sets label \a style to one of \c QDataVis::LabelStyle. It is preset to
 * \c QDataVis::LabelStyleFromTheme by default.
 */
void Q3DScatter::setLabelStyle(QDataVis::LabelStyle style)
{
    d_ptr->m_shared->setLabelStyle(style);
}

QDataVis::LabelStyle Q3DScatter::labelStyle() const
{
    return d_ptr->m_shared->labelStyle();
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
 * \property Q3DScatter::shadowQuality
 *
 * Sets shadow \a quality to one of \c QDataVis::ShadowQuality. It is preset to
 * \c QDataVis::ShadowQualityMedium by default.
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
 * \property Q3DScatter::colorStyle
 *
 * Sets the color \a style used to render items.
 * Defaults to true.
 *
 * \sa itemColor, itemGradient
 */
void Q3DScatter::setColorStyle(QDataVis::ColorStyle style)
{
    d_ptr->m_shared->setColorStyle(style);
}

QDataVis::ColorStyle Q3DScatter::colorStyle() const
{
    return d_ptr->m_shared->colorStyle();
}

/*!
 * \property Q3DScatter::itemColor
 *
 * Set item color to the \a color for this set. Overrides any previously set item gradient for this
 * set, as well as any item gradient or color from the theme.
 *
 * \sa theme, itemGradient
 */
void Q3DScatter::setItemColor(const QColor &color)
{
    d_ptr->m_shared->setObjectColor(color);
}

QColor Q3DScatter::itemColor() const
{
    return d_ptr->m_shared->objectColor();
}

/*!
 * \property Q3DScatter::itemGradient
 *
 * Set item gradient to the \a gradient for this set. Overrides any previously set item color for this
 * set, as well as any item gradient or color from the theme.
 *
 * \sa theme, itemColor, colorStyle
 */
void Q3DScatter::setItemGradient(const QLinearGradient &gradient)
{
    d_ptr->m_shared->setObjectGradient(gradient);
}

QLinearGradient Q3DScatter::itemGradient() const
{
    return d_ptr->m_shared->objectGradient();
}

/*!
 * \property Q3DScatter::singleHighlightColor
 *
 * Set single item highlight color to the \a color for this set. Overrides any previously set single
 * item highlight gradient for this set, as well as any single item highlight gradient or color from the theme.
 *
 * \sa theme, singleHighlightGradient
 */
void Q3DScatter::setSingleHighlightColor(const QColor &color)
{
    d_ptr->m_shared->setSingleHighlightColor(color);
}

QColor Q3DScatter::singleHighlightColor() const
{
    return d_ptr->m_shared->singleHighlightColor();
}

/*!
 * \property Q3DScatter::singleHighlightGradient
 *
 * Set single item highlight gradient to the \a gradient for this set.
 * Overrides any previously set single item highlight color for this
 * set, as well as any single item highlight gradient or color from the theme.
 *
 * \sa theme, singleHighlightColor, colorStyle
 */
void Q3DScatter::setSingleHighlightGradient(const QLinearGradient &gradient)
{
    d_ptr->m_shared->setSingleHighlightGradient(gradient);
}

QLinearGradient Q3DScatter::singleHighlightGradient() const
{
    return d_ptr->m_shared->singleHighlightGradient();
}

/*!
 * \property Q3DScatter::multiHighlightColor
 *
 * Set multiple item highlight (e.g. row/column highlight) color to the \a color for this set.
 * Overrides any previously set multiple item highlight gradient for this set, as well as any
 * multiple item highlight gradient or color from the theme.
 *
 * \sa theme, multiHighlightGradient
 */
void Q3DScatter::setMultiHighlightColor(const QColor &color)
{
    d_ptr->m_shared->setMultiHighlightColor(color);
}

QColor Q3DScatter::multiHighlightColor() const
{
    return d_ptr->m_shared->multiHighlightColor();
}

/*!
 * \property Q3DScatter::multiHighlightGradient
 *
 * Set multiple item highlight (e.g. row/column highlight) gradient to the \a gradient for this set.
 * Overrides any previously set multiple item highlight color for this
 * set, as well as any multiple item highlight gradient or color from the theme.
 *
 * \sa theme, multiHighlightColor, colorStyle
 */
void Q3DScatter::setMultiHighlightGradient(const QLinearGradient &gradient)
{
    d_ptr->m_shared->setMultiHighlightGradient(gradient);
}

QLinearGradient Q3DScatter::multiHighlightGradient() const
{
    return d_ptr->m_shared->multiHighlightGradient();
}

/*!
 * Sets a user-defined X-axis. Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
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
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
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
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
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


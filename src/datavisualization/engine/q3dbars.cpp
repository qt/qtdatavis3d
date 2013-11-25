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

#include "q3dbars.h"
#include "q3dbars_p.h"
#include "bars3dcontroller_p.h"
#include "q3dvalueaxis.h"
#include "q3dcategoryaxis.h"
#include "q3dcamera.h"
#include "qbar3dseries_p.h"

#include <QMouseEvent>

#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class Q3DBars
 * \inmodule QtDataVisualization
 * \brief The Q3DBars class provides methods for rendering 3D bar graphs.
 * \since Qt Data Visualization 1.0
 *
 * This class enables developers to render bar graphs in 3D and to view them by rotating the scene
 * freely. Rotation is done by holding down the right mouse button and moving the mouse. Zooming
 * is done by mouse wheel. Selection, if enabled, is done by left mouse button. The scene can be
 * reset to default camera view by clicking mouse wheel. In touch devices rotation is done
 * by tap-and-move, selection by tap-and-hold and zoom by pinch.
 *
 * If no axes are set explicitly to Q3DBars, temporary default axes with no labels are created.
 * These default axes can be modified via axis accessors, but as soon any axis is set explicitly
 * for the orientation, the default axis for that orientation is destroyed.
 *
 * Q3DBars supports more than one series visible at the same time. It is not necessary for all series
 * to have the same amount of rows and columns.
 * Row and column labels are taken from the first added series, unless explicitly defined to
 * row and column axes.
 *
 * Methods are provided for changing themes, bar selection modes and so on. See the
 * methods for more detailed descriptions.
 *
 * \section1 How to construct a minimal Q3DBars graph
 *
 * First, construct an instance of Q3DBars:
 *
 * \snippet doc_src_q3dbars_construction.cpp 4
 *
 * After constructing Q3DBars, you can set the data window by changing the range on the row and
 * column axes. It is not mandatory, as data window will default to showing all of the data in
 * the series. If the amount of data is large, it is usually preferable to show just a
 * portion of it. For the example, let's set the data window to show first five rows and columns:
 *
 * \snippet doc_src_q3dbars_construction.cpp 0
 *
 * Now Q3DBars is ready to receive data to be rendered. Create a series with one row of 5 values:
 *
 * \snippet doc_src_q3dbars_construction.cpp 1
 *
 * \note We set the data window to 5 x 5, but we are adding only one row of data. This is ok,
 * the rest of the rows will just be blank.
 *
 * Finally you will need to set it visible:
 *
 * \snippet doc_src_q3dbars_construction.cpp 2
 *
 * The complete code needed to create and display this graph is:
 *
 * \snippet doc_src_q3dbars_construction.cpp 3
 *
 * And this is what those few lines of code produce:
 *
 * \image q3dbars-minimal.png
 *
 * The scene can be rotated, zoomed into, and a bar can be selected to view it's value,
 * but no other interaction is included in this minimal code example. You can learn more by
 * familiarizing yourself with the examples provided, like the \l{Bars Example} or
 * the \l{Custom Proxy Example}.
 *
 * \sa Q3DScatter, Q3DSurface, {Qt Data Visualization C++ Classes}
 */

/*!
 * Constructs a new 3D bar graph.
 */
Q3DBars::Q3DBars()
    : d_ptr(new Q3DBarsPrivate(this, geometry()))
{
    setVisualController(d_ptr->m_shared);
    d_ptr->m_shared->initializeOpenGL();
    QObject::connect(d_ptr->m_shared, &Abstract3DController::selectionModeChanged, this,
                     &Q3DBars::selectionModeChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::labelStyleChanged, this,
                     &Q3DBars::labelStyleChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::shadowQualityChanged, this,
                     &Q3DBars::shadowQualityChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::meshFileNameChanged, this,
                     &Q3DBars::meshFileNameChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::fontChanged, this,
                     &Q3DBars::fontChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::themeChanged, this,
                     &Q3DBars::themeChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::gridVisibleChanged, this,
                     &Q3DBars::gridVisibleChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::backgroundVisibleChanged, this,
                     &Q3DBars::backgroundVisibleChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::colorStyleChanged, this,
                     &Q3DBars::colorStyleChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::objectColorChanged, this,
                     &Q3DBars::barColorChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::objectGradientChanged, this,
                     &Q3DBars::barGradientChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::singleHighlightColorChanged, this,
                     &Q3DBars::singleHighlightColorChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::singleHighlightGradientChanged, this,
                     &Q3DBars::singleHighlightGradientChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::multiHighlightColorChanged, this,
                     &Q3DBars::multiHighlightColorChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::multiHighlightGradientChanged, this,
                     &Q3DBars::multiHighlightGradientChanged);
    QObject::connect(d_ptr->m_shared, &Abstract3DController::needRender, this,
                     &Q3DWindow::renderLater);
}

/*!
 * Destroys the 3D bar graph.
 */
Q3DBars::~Q3DBars()
{
}

/*!
 * Adds the \a series to the graph. A graph can contain multiple series, but only one set of axes,
 * so the rows and columns of all series must match for the visualized data to be meaningful.
 * If the graph has multiple visible series, only the first one added will
 * generate the row or column labels on the axes in cases where the labels are not explicitly set
 * to the axes. If the newly added series has specified a selected bar, it will be highlighted and
 * any existing selection will be cleared. Only one added series can have an active selection.
 */
void Q3DBars::addSeries(QBar3DSeries *series)
{
    d_ptr->m_shared->addSeries(series);
}

/*!
 * Removes the \a series from the graph.
 */
void Q3DBars::removeSeries(QBar3DSeries *series)
{
    d_ptr->m_shared->removeSeries(series);
}

/*!
 * \return list of series added to this graph.
 */
QList<QBar3DSeries *> Q3DBars::seriesList()
{
    return d_ptr->m_shared->barSeriesList();
}

/*!
 * \internal
 */
void Q3DBars::mouseDoubleClickEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseDoubleClickEvent(event);
}

/*!
 * \internal
 */
void Q3DBars::touchEvent(QTouchEvent *event)
{
    d_ptr->m_shared->touchEvent(event);
}

/*!
 * \internal
 */
void Q3DBars::mousePressEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mousePressEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DBars::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseReleaseEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DBars::mouseMoveEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseMoveEvent(event, event->pos());
}

/*!
 * \internal
 */
void Q3DBars::wheelEvent(QWheelEvent *event)
{
    d_ptr->m_shared->wheelEvent(event);
}

/*!
 * \internal
 */
void Q3DBars::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    d_ptr->m_shared->setSize(width(), height());
}

/*!
 * Sets window \a width.
 */
void Q3DBars::setWidth(const int width)
{
    d_ptr->m_shared->setWidth(width);
    QWindow::setWidth(width);
}

/*!
 * Sets window \a height.
 */
void Q3DBars::setHeight(const int height)
{
    d_ptr->m_shared->setHeight(height);
    QWindow::setHeight(height);
}

/*!
 * \property Q3DBars::barThickness
 *
 * Bar thickness ratio between X and Z dimensions. 1.0 means bars are as wide as they are deep, 0.5
 * makes them twice as deep as they are wide. It is preset to \c 1.0 by default.
 */
void Q3DBars::setBarThickness(float thicknessRatio)
{
    if (thicknessRatio != barThickness()) {
        d_ptr->m_shared->setBarSpecs(GLfloat(thicknessRatio), barSpacing(), isBarSpacingRelative());
        emit barThicknessChanged(thicknessRatio);
    }
}

float Q3DBars::barThickness()
{
    return d_ptr->m_shared->barThickness();
}

/*!
 * \property Q3DBars::barSpacing
 *
 * Bar spacing, which is the empty space between bars, in X and Z dimensions. It is preset to
 * \c {(1.0, 1.0)} by default. Spacing is affected by barSpacingRelative -property.
 *
 * \sa barSpacingRelative
 */
void Q3DBars::setBarSpacing(QSizeF spacing)
{
    if (spacing != barSpacing()) {
        d_ptr->m_shared->setBarSpecs(GLfloat(barThickness()), spacing, isBarSpacingRelative());
        emit barSpacingChanged(spacing);
    }
}

QSizeF Q3DBars::barSpacing()
{
    return d_ptr->m_shared->barSpacing();
}

/*!
 * \property Q3DBars::barSpacingRelative
 *
 * This is used to indicate if spacing is meant to be absolute or relative to bar thickness.
 * If it is true, value of 0.0 means the bars are side-to-side and for example 1.0 means
 * there is one thickness in between the bars. It is preset to \c true.
 */
void Q3DBars::setBarSpacingRelative(bool relative)
{
    if (relative != isBarSpacingRelative()) {
        d_ptr->m_shared->setBarSpecs(GLfloat(barThickness()), barSpacing(), relative);
        emit barSpacingRelativeChanged(relative);
    }
}

bool Q3DBars::isBarSpacingRelative()
{
    return d_ptr->m_shared->isBarSpecRelative();
}

/*!
 * Sets the bar \a style to one of the values in \c QDataVis::MeshStyle. It is preset to
 * \c QDataVis::MeshStyleBars by default. A \a smooth flag can be used to set shading to smooth.
 * It is \c false by default.
 *
 * \sa setMeshFileName()
 */
void Q3DBars::setBarType(QDataVis::MeshStyle style, bool smooth)
{
    d_ptr->m_shared->setBarType(style, smooth);
}

/*!
 * \property Q3DBars::theme
 *
 * A user-defined theme.
 *
 * TODO: Add docs.
 */
void Q3DBars::setTheme(Q3DTheme *theme)
{
    d_ptr->m_shared->setTheme(theme);
}

Q3DTheme *Q3DBars::theme() const
{
    return d_ptr->m_shared->theme();
}

/*!
 * \property Q3DBars::selectionMode
 *
 * Sets bar selection \a mode to one of \c QDataVis::SelectionMode. It is preset to
 * \c QDataVis::SelectionModeItem by default.
 */
void Q3DBars::setSelectionMode(QDataVis::SelectionFlags mode)
{
    d_ptr->m_shared->setSelectionMode(mode);
}

QDataVis::SelectionFlags Q3DBars::selectionMode() const
{
    return d_ptr->m_shared->selectionMode();
}

/*!
 * \property Q3DBars::meshFileName
 *
 * Override bar type with a mesh object located in \a objFileName.
 * \note Object needs to be in Wavefront obj format and include vertices, normals and UVs.
 * It also needs to be in triangles.
 *
 * \sa setBarType()
 */
void Q3DBars::setMeshFileName(const QString &objFileName)
{
    d_ptr->m_shared->setMeshFileName(objFileName);
}

QString Q3DBars::meshFileName() const
{
    return d_ptr->m_shared->meshFileName();
}

/*!
 * \property Q3DBars::font
 *
 * Sets the \a font for labels. It is preset to \c Arial by default.
 */
void Q3DBars::setFont(const QFont &font)
{
    d_ptr->m_shared->setFont(font);
}

QFont Q3DBars::font() const
{
    return d_ptr->m_shared->font();
}

/*!
 * \property Q3DBars::scene
 *
 * This property contains the read only Q3DScene that can be used to access e.g. camera object.
 */
Q3DScene *Q3DBars::scene() const
{
    return d_ptr->m_shared->scene();
}

/*!
 * \property Q3DBars::labelStyle
 *
 * Sets label \a style to one of \c QDataVis::LabelStyle. It is preset to
 * \c QDataVis::LabelStyleFromTheme by default.
 */
void Q3DBars::setLabelStyle(QDataVis::LabelStyle style)
{
    d_ptr->m_shared->setLabelStyle(style);
}

QDataVis::LabelStyle Q3DBars::labelStyle() const
{
    return d_ptr->m_shared->labelStyle();
}

/*!
 * \property Q3DBars::gridVisible
 *
 * Sets grid visibility to \a visible. It is preset to \c true by default.
 */
void Q3DBars::setGridVisible(bool visible)
{
    d_ptr->m_shared->setGridEnabled(visible);
}

bool Q3DBars::isGridVisible() const
{
    return d_ptr->m_shared->gridEnabled();
}

/*!
 * \property Q3DBars::backgroundVisible
 *
 * Sets background visibility to \a visible. It is preset to \c true by default.
 */
void Q3DBars::setBackgroundVisible(bool visible)
{
    d_ptr->m_shared->setBackgroundEnabled(visible);
}

bool Q3DBars::isBackgroundVisible() const
{
    return d_ptr->m_shared->backgroundEnabled();
}

/*!
 * \property Q3DBars::shadowQuality
 *
 * Sets shadow \a quality to one of \c QDataVis::ShadowQuality. It is preset to
 * \c QDataVis::ShadowQualityMedium by default.
 *
 * \note If setting QDataVis::ShadowQuality of a certain level fails, a level is lowered
 * until it is successful and shadowQualityChanged signal is emitted for each time the change is
 * done.
 */
void Q3DBars::setShadowQuality(QDataVis::ShadowQuality quality)
{
    d_ptr->m_shared->setShadowQuality(quality);
}

QDataVis::ShadowQuality Q3DBars::shadowQuality() const
{
    return d_ptr->m_shared->shadowQuality();
}

/*!
 * \property Q3DBars::colorStyle
 *
 * Sets the color \a style used to render bars.
 * Defaults to true.
 *
 * \sa barColor, barGradient
 */
void Q3DBars::setColorStyle(QDataVis::ColorStyle style)
{
    d_ptr->m_shared->setColorStyle(style);
}

QDataVis::ColorStyle Q3DBars::colorStyle() const
{
    return d_ptr->m_shared->colorStyle();
}

/*!
 * \property Q3DBars::barColor
 *
 * Set bar color to the \a color for this set. Overrides any previously set bar gradient for this
 * set, as well as any bar gradient or color from the theme.
 *
 * \sa theme, barGradient
 */
void Q3DBars::setBarColor(const QColor &color)
{
    d_ptr->m_shared->setObjectColor(color);
}

QColor Q3DBars::barColor() const
{
    return d_ptr->m_shared->objectColor();
}

/*!
 * \property Q3DBars::barGradient
 *
 * Set bar gradient to the \a gradient for this set. Overrides any previously set bar color for this
 * set, as well as any bar gradient or color from the theme.
 *
 * \sa theme, barColor, colorStyle
 */
void Q3DBars::setBarGradient(const QLinearGradient &gradient)
{
    d_ptr->m_shared->setObjectGradient(gradient);
}

QLinearGradient Q3DBars::barGradient() const
{
    return d_ptr->m_shared->objectGradient();
}

/*!
 * \property Q3DBars::singleHighlightColor
 *
 * Set single bar highlight color to the \a color for this set. Overrides any previously set single
 * bar highlight gradient for this set, as well as any single bar highlight gradient or color from the theme.
 *
 * \sa theme, singleHighlightGradient
 */
void Q3DBars::setSingleHighlightColor(const QColor &color)
{
    d_ptr->m_shared->setSingleHighlightColor(color);
}

QColor Q3DBars::singleHighlightColor() const
{
    return d_ptr->m_shared->singleHighlightColor();
}

/*!
 * \property Q3DBars::singleHighlightGradient
 *
 * Set single bar highlight gradient to the \a gradient for this set.
 * Overrides any previously set single bar highlight color for this
 * set, as well as any single bar highlight gradient or color from the theme.
 *
 * \sa theme, singleHighlightColor, colorStyle
 */
void Q3DBars::setSingleHighlightGradient(const QLinearGradient &gradient)
{
    d_ptr->m_shared->setSingleHighlightGradient(gradient);
}

QLinearGradient Q3DBars::singleHighlightGradient() const
{
    return d_ptr->m_shared->singleHighlightGradient();
}

/*!
 * \property Q3DBars::multiHighlightColor
 *
 * Set multiple bar highlight (e.g. row/column highlight) color to the \a color for this set.
 * Overrides any previously set multiple bar highlight gradient for this set, as well as any
 * multiple bar highlight gradient or color from the theme.
 *
 * \sa theme, multiHighlightGradient
 */
void Q3DBars::setMultiHighlightColor(const QColor &color)
{
    d_ptr->m_shared->setMultiHighlightColor(color);
}

QColor Q3DBars::multiHighlightColor() const
{
    return d_ptr->m_shared->multiHighlightColor();
}

/*!
 * \property Q3DBars::multiHighlightGradient
 *
 * Set multiple bar highlight (e.g. row/column highlight) gradient to the \a gradient for this set.
 * Overrides any previously set multiple bar highlight color for this
 * set, as well as any multiple bar highlight gradient or color from the theme.
 *
 * \sa theme, multiHighlightColor, colorStyle
 */
void Q3DBars::setMultiHighlightGradient(const QLinearGradient &gradient)
{
    d_ptr->m_shared->setMultiHighlightGradient(gradient);
}

QLinearGradient Q3DBars::multiHighlightGradient() const
{
    return d_ptr->m_shared->multiHighlightGradient();
}

/*!
 * Sets a user-defined row \a axis. Implicitly calls addAxis() to transfer ownership of
 * the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels is created.
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DBars::setRowAxis(Q3DCategoryAxis *axis)
{
    d_ptr->m_shared->setAxisX(axis);
}

/*!
 * \return category axis for rows.
 */
Q3DCategoryAxis *Q3DBars::rowAxis() const
{
    return static_cast<Q3DCategoryAxis *>(d_ptr->m_shared->axisX());
}

/*!
 * Sets a user-defined column \a axis. Implicitly calls addAxis() to transfer ownership of
 * the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels is created.
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DBars::setColumnAxis(Q3DCategoryAxis *axis)
{
    d_ptr->m_shared->setAxisZ(axis);
}

/*!
 * \return category axis for columns.
 */
Q3DCategoryAxis *Q3DBars::columnAxis() const
{
    return static_cast<Q3DCategoryAxis *>(d_ptr->m_shared->axisZ());
}

/*!
 * Sets a user-defined value \a axis (the Y-axis). Implicitly calls addAxis() to transfer ownership
 * of the \a axis to this graph.
 *
 * If the \a axis is null, a temporary default axis with no labels and automatically adjusting
 * range is created.
 * This temporary axis is destroyed if another \a axis is set explicitly to the same orientation.
 *
 * \sa addAxis(), releaseAxis()
 */
void Q3DBars::setValueAxis(Q3DValueAxis *axis)
{
    d_ptr->m_shared->setAxisY(axis);
}

/*!
 * \return used value axis (Y-axis).
 */
Q3DValueAxis *Q3DBars::valueAxis() const
{
    return static_cast<Q3DValueAxis *>(d_ptr->m_shared->axisY());
}

/*!
 * Adds \a axis to the graph. The axes added via addAxis are not yet taken to use,
 * addAxis is simply used to give the ownership of the \a axis to the graph.
 * The \a axis must not be null or added to another graph.
 *
 * \sa releaseAxis(), setValueAxis(), setRowAxis(), setColumnAxis()
 */
void Q3DBars::addAxis(Q3DAbstractAxis *axis)
{
    d_ptr->m_shared->addAxis(axis);
}

/*!
 * Releases the ownership of the \a axis back to the caller, if it is added to this graph.
 * If the released \a axis is in use, a new default axis will be created and set active.
 *
 * If the default axis is released and added back later, it behaves as any other axis would.
 *
 * \sa addAxis(), setValueAxis(), setRowAxis(), setColumnAxis()
 */
void Q3DBars::releaseAxis(Q3DAbstractAxis *axis)
{
    d_ptr->m_shared->releaseAxis(axis);
}

/*!
 * \return list of all added axes.
 *
 * \sa addAxis()
 */
QList<Q3DAbstractAxis *> Q3DBars::axes() const
{
    return d_ptr->m_shared->axes();
}

Q3DBarsPrivate::Q3DBarsPrivate(Q3DBars *q, QRect rect)
    : q_ptr(q),
      m_shared(new Bars3DController(rect))
{
    QObject::connect(m_shared, &Abstract3DController::shadowQualityChanged, this,
                     &Q3DBarsPrivate::handleShadowQualityUpdate);
}

Q3DBarsPrivate::~Q3DBarsPrivate()
{
    delete m_shared;
}

void Q3DBarsPrivate::handleShadowQualityUpdate(QDataVis::ShadowQuality quality)
{
    emit q_ptr->shadowQualityChanged(quality);
}

QT_DATAVISUALIZATION_END_NAMESPACE

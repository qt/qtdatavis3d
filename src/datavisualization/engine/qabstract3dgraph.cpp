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

#include "qabstract3dgraph.h"
#include "qabstract3dgraph_p.h"
#include "abstract3dcontroller_p.h"
#include "qabstract3dinputhandler_p.h"
#include "q3dscene_p.h"
#include "qutils.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

/*!
 * \class QAbstract3DGraph
 * \inmodule QtDataVisualization
 * \brief The QAbstract3DGraph class provides a window and render loop for graphs.
 * \since QtDataVisualization 1.0
 *
 * This class subclasses a QWindow and provides render loop for graphs inheriting it.
 *
 * You should not need to use this class directly, but one of its subclasses instead.
 *
 * Anti-aliasing is turned on by default on C++, except in OpenGL ES2
 * environments, where anti-aliasing is not supported by Qt Data Visualization.
 * To specify non-default anti-aliasing for a graph, give a custom surface format as
 * a constructor parameter. You can use the convenience function \c QtDataVisualization::qDefaultSurfaceFormat()
 * to create the surface format object.
 *
 * \note QAbstract3DGraph sets window flag \c Qt::FramelessWindowHint on by default. If you want to display
 * graph windows as standalone windows with regular window frame, clear this flag after constructing
 * the graph. For example:
 *
 * \code
 *  Q3DBars *graphWindow = new Q3DBars;
 *  graphWindow->setFlags(graphWindow->flags() ^ Qt::FramelessWindowHint);
 * \endcode
 *
 * \sa Q3DBars, Q3DScatter, Q3DSurface, {Qt Data Visualization C++ Classes}
 */

/*!
    \enum QAbstract3DGraph::SelectionFlag

    Item selection modes. Values of this enumeration can be combined with OR operator.

    \value SelectionNone
           Selection mode disabled.
    \value SelectionItem
           Selection highlights a single item.
    \value SelectionRow
           Selection highlights a single row.
    \value SelectionItemAndRow
           Combination flag for highlighting both item and row with different colors.
    \value SelectionColumn
           Selection highlights a single column.
    \value SelectionItemAndColumn
           Combination flag for highlighting both item and column with different colors.
    \value SelectionRowAndColumn
           Combination flag for highlighting both row and column.
    \value SelectionItemRowAndColumn
           Combination flag for highlighting item, row, and column.
    \value SelectionSlice
           Setting this mode flag indicates that the graph should take care of the slice view handling
           automatically. If you wish to control the slice view yourself via Q3DScene, do not set this
           flag. When setting this mode flag, either \c SelectionRow or \c SelectionColumn must also
           be set, but not both. Slicing is supported by Q3DBars and Q3DSurface only.
           When this flag is set, slice mode is entered in the following situations:
           \list
           \li When selection is changed explicitly via series API to a visible item
           \li When selection is changed by clicking on the graph
           \li When the selection mode changes and the selected item is visible
           \endlist
    \value SelectionMultiSeries
           Setting this mode means that items for all series at same position are highlighted, instead
           of just the selected item. The actual selection in the other series doesn't change.
           Multi-series selection is not supported for Q3DScatter.
*/

/*!
    \enum QAbstract3DGraph::ShadowQuality

    Quality of shadows.

    \value ShadowQualityNone
           Shadows are disabled.
    \value ShadowQualityLow
           Shadows are rendered in low quality.
    \value ShadowQualityMedium
           Shadows are rendered in medium quality.
    \value ShadowQualityHigh
           Shadows are rendered in high quality.
    \value ShadowQualitySoftLow
           Shadows are rendered in low quality with softened edges.
    \value ShadowQualitySoftMedium
           Shadows are rendered in medium quality with softened edges.
    \value ShadowQualitySoftHigh
           Shadows are rendered in high quality with softened edges.
*/

/*!
    \enum QAbstract3DGraph::ElementType
    \since QtDataVisualization 1.1

    Type of an element in the graph.

    \value ElementNone
           No defined element.
    \value ElementSeries
           A series (i.e. an item in a series).
    \value ElementAxisXLabel
           X axis label.
    \value ElementAxisYLabel
           Y axis label.
    \value ElementAxisZLabel
           Z axis label.
    \value ElementCustomItem
           Custom item.
*/

/*!
    \enum QAbstract3DGraph::OptimizationHint
    \since Qt Data Visualization 1.1

    The optimization hint for rendering.

    \value OptimizationDefault
           Provides the full feature set at a reasonable performance.
    \value OptimizationStatic
           Optimizes the rendering of static data sets at the expense of some features.
*/

/*!
 * \internal
 */
QAbstract3DGraph::QAbstract3DGraph(QAbstract3DGraphPrivate *d, const QSurfaceFormat *format,
                                   QWindow *parent)
    : QWindow(parent),
      d_ptr(d)
{
    qRegisterMetaType<QAbstract3DGraph::ShadowQuality>("QAbstract3DGraph::ShadowQuality");
    qRegisterMetaType<QAbstract3DGraph::ElementType>("QAbstract3DGraph::ElementType");

    // Default to frameless window, as typically graphs are not toplevel
    setFlags(flags() | Qt::FramelessWindowHint);

    QSurfaceFormat surfaceFormat;
    if (format) {
        surfaceFormat = *format;
        // Make sure renderable type is correct
#if !defined(QT_OPENGL_ES_2)
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
#else
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
    } else {
        surfaceFormat = qDefaultSurfaceFormat();
    }

    d_ptr->m_context = new QOpenGLContext(this);
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(surfaceFormat);

    create();

    d_ptr->m_context->setFormat(requestedFormat());
    d_ptr->m_context->create();
    d_ptr->m_context->makeCurrent(this);

    initializeOpenGLFunctions();

    const GLubyte *shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
#ifndef QT_NO_DEBUG
    const GLubyte *openGLVersion = glGetString(GL_VERSION);
    qDebug() << "OpenGL version:" << (const char *)openGLVersion;
    qDebug() << "GLSL version:" << (const char *)shaderVersion;
#endif

#if !defined(QT_OPENGL_ES_2)
    // If we have real OpenGL, GLSL version must be 1.2 or over. Quit if not.
    QStringList splitversionstr =
            QString::fromLatin1((const char *)shaderVersion).split(QChar::fromLatin1(' '));
    if (splitversionstr[0].toFloat() < 1.2)
        qFatal("GLSL version must be 1.20 or higher. Try installing latest display drivers.");
#else
    Q_UNUSED(shaderVersion)
#endif

    d_ptr->renderLater();
}

/*!
 * Destroys QAbstract3DGraph.
 */
QAbstract3DGraph::~QAbstract3DGraph()
{
}

/*!
 * Adds the given \a inputHandler to the graph. The input handlers added via addInputHandler
 * are not taken in to use directly. Only the ownership of the \a inputHandler is given to the graph.
 * The \a inputHandler must not be null or already added to another graph.
 *
 * \sa releaseInputHandler(), setActiveInputHandler()
 */
void QAbstract3DGraph::addInputHandler(QAbstract3DInputHandler *inputHandler)
{
    d_ptr->m_visualController->addInputHandler(inputHandler);
}

/*!
 * Releases the ownership of the \a inputHandler back to the caller, if it was added to this graph.
 * If the released \a inputHandler is in use there will be no input handler active after this call.
 *
 * If the default input handler is released and added back later, it behaves as any other input handler would.
 *
 * \sa addInputHandler(), setActiveInputHandler()
 */
void QAbstract3DGraph::releaseInputHandler(QAbstract3DInputHandler *inputHandler)
{
    d_ptr->m_visualController->releaseInputHandler(inputHandler);
}

/*!
 * \property QAbstract3DGraph::activeInputHandler
 *
 * The active \a inputHandler used in the graph. Implicitly calls addInputHandler() to transfer
 * ownership of the \a inputHandler to this graph.
 *
 * If the \a inputHandler is null, no input handler will be active after this call.
 *
 * \sa addInputHandler(), releaseInputHandler()
 */
void QAbstract3DGraph::setActiveInputHandler(QAbstract3DInputHandler *inputHandler)
{
    d_ptr->m_visualController->setActiveInputHandler(inputHandler);
}

QAbstract3DInputHandler *QAbstract3DGraph::activeInputHandler() const
{
    return d_ptr->m_visualController->activeInputHandler();
}

/*!
 * \return list of all added input handlers.
 *
 * \sa addInputHandler()
 */
QList<QAbstract3DInputHandler *> QAbstract3DGraph::inputHandlers() const
{
    return d_ptr->m_visualController->inputHandlers();
}

/*!
 * Adds the given \a theme to the graph. The themes added via addTheme are not taken in to use
 * directly. Only the ownership of the a\ theme is given to the graph.
 * The \a theme must not be null or already added to another graph.
 *
 * \sa releaseTheme(), setActiveTheme()
 */
void QAbstract3DGraph::addTheme(Q3DTheme *theme)
{
    d_ptr->m_visualController->addTheme(theme);
}

/*!
 * Releases the ownership of the \a theme back to the caller, if it was added to this graph.
 * If the released \a theme is in use, a new default theme will be created and set active.
 *
 * If the default theme is released and added back later, it behaves as any other theme would.
 *
 * \sa addTheme(), setActiveTheme()
 */
void QAbstract3DGraph::releaseTheme(Q3DTheme *theme)
{
    d_ptr->m_visualController->releaseTheme(theme);
}

/*!
 * \property QAbstract3DGraph::activeTheme
 *
 * The active \a theme to be used for the graph. Implicitly calls addTheme() to transfer ownership
 * of the \a theme to this graph.
 * If the \a theme is null, a temporary default theme is created. This temporary theme is destroyed
 * if any theme is explicitly set later.
 * Properties of the \a theme can be modified even after setting it, and the modifications take
 * effect immediately.
 */
void QAbstract3DGraph::setActiveTheme(Q3DTheme *theme)
{
    d_ptr->m_visualController->setActiveTheme(theme);
}


Q3DTheme *QAbstract3DGraph::activeTheme() const
{
    return d_ptr->m_visualController->activeTheme();
}

/*!
 * \return list of all added themes.
 *
 * \sa addTheme()
 */
QList<Q3DTheme *> QAbstract3DGraph::themes() const
{
    return d_ptr->m_visualController->themes();
}

/*!
 * \property QAbstract3DGraph::selectionMode
 *
 * Sets selection \a mode to a combination of SelectionFlags. It is preset to
 * \c SelectionItem by default.
 * Different graph types support different selection modes. See SelectionFlags
 * documentation for details.
 */
void QAbstract3DGraph::setSelectionMode(SelectionFlags mode)
{
    d_ptr->m_visualController->setSelectionMode(mode);
}

QAbstract3DGraph::SelectionFlags QAbstract3DGraph::selectionMode() const
{
    return d_ptr->m_visualController->selectionMode();
}

/*!
 * \property QAbstract3DGraph::shadowQuality
 *
 * Sets the shadow \a quality to one of the ShadowQuality enum values. It is preset to
 * \c ShadowQualityMedium by default.
 *
 * \note If setting the shadow quality to a certain level fails, the level is lowered
 * until it is successfully set. The \c shadowQualityChanged signal is emitted for each time
 * a change is done.
 */
void QAbstract3DGraph::setShadowQuality(ShadowQuality quality)
{
    d_ptr->m_visualController->setShadowQuality(quality);
}

QAbstract3DGraph::ShadowQuality QAbstract3DGraph::shadowQuality() const
{
    return d_ptr->m_visualController->shadowQuality();
}

/*!
 * \return true if shadows are supported with the current configuration.
 * OpenGL ES2 configurations do not support shadows.
 */
bool QAbstract3DGraph::shadowsSupported() const
{
    return d_ptr->m_visualController->shadowsSupported();
}

/*!
 * \property QAbstract3DGraph::scene
 *
 * This property is read-only and contains the Q3DScene pointer that can be used to manipulate
 * the scene and access the scene elements, such as the active camera.
 */
Q3DScene *QAbstract3DGraph::scene() const
{
    return d_ptr->m_visualController->scene();
}

/*!
 * Clears selection from all attached series.
 */
void QAbstract3DGraph::clearSelection()
{
    d_ptr->m_visualController->clearSelection();
}

/*!
 * Adds a QCustom3DItem \a item to the graph. Graph takes ownership of the added item.
 *
 * \return index to the added item if add was successful, -1 if trying to add a null item, and
 * index of the item if trying to add an already added item.
 *
 * \sa removeCustomItems(), removeCustomItem(), removeCustomItemAt()
 *
 * \since QtDataVisualization 1.1
 */
int QAbstract3DGraph::addCustomItem(QCustom3DItem *item)
{
    return d_ptr->m_visualController->addCustomItem(item);
}

/*!
 * Removes all custom items. Deletes the resources allocated to them.
 *
 * \since QtDataVisualization 1.1
 */
void QAbstract3DGraph::removeCustomItems()
{
    d_ptr->m_visualController->deleteCustomItems();
}

/*!
 * Removes the custom \a {item}. Deletes the resources allocated to it.
 *
 * \since QtDataVisualization 1.1
 */
void QAbstract3DGraph::removeCustomItem(QCustom3DItem *item)
{
    d_ptr->m_visualController->deleteCustomItem(item);
}

/*!
 * Removes all custom items at \a {position}. Deletes the resources allocated to them.
 *
 * \since QtDataVisualization 1.1
 */
void QAbstract3DGraph::removeCustomItemAt(const QVector3D &position)
{
    d_ptr->m_visualController->deleteCustomItem(position);
}

/*!
 * Gets ownership of given \a item back and removes the \a item from the graph.
 *
 * \since QtDataVisualization 1.1
 *
 * \note If the same item is added back to the graph, the texture or the texture file needs to be
 * re-set.
 *
 * \sa QCustom3DItem::setTextureImage(), QCustom3DItem::setTextureFile()
 */
void QAbstract3DGraph::releaseCustomItem(QCustom3DItem *item)
{
    return d_ptr->m_visualController->releaseCustomItem(item);
}

/*!
 * Can be used to query the index of the selected label after receiving \c selectedElementChanged
 * signal with any label type. Selection is valid until the next \c selectedElementChanged signal.
 *
 * \return index of the selected label, or -1.
 *
 * \since QtDataVisualization 1.1
 *
 * \sa selectedElement
 */
int QAbstract3DGraph::selectedLabelIndex() const
{
    return d_ptr->m_visualController->selectedLabelIndex();
}

/*!
 * Can be used to get the selected axis after receiving \c selectedElementChanged signal with any label
 * type. Selection is valid until the next \c selectedElementChanged signal.
 *
 * \return pointer to the selected axis, or null.
 *
 * \since QtDataVisualization 1.1
 *
 * \sa selectedElement
 */
QAbstract3DAxis *QAbstract3DGraph::selectedAxis() const
{
    return d_ptr->m_visualController->selectedAxis();
}

/*!
 * Can be used to query the index of the selected custom item after receiving \c selectedElementChanged
 * signal with QAbstract3DGraph::ElementCustomItem type. Selection is valid until the next
 * \c selectedElementChanged signal.
 *
 * \return index of the selected custom item, or -1.
 *
 * \since QtDataVisualization 1.1
 *
 * \sa selectedElement
 */
int QAbstract3DGraph::selectedCustomItemIndex() const
{
    return d_ptr->m_visualController->selectedCustomItemIndex();
}

/*!
 * Can be used to get the selected custom item after receiving \c selectedElementChanged signal with
 * QAbstract3DGraph::ElementCustomItem type. Ownership of the item remains with the graph.
 * Selection is valid until the next \c selectedElementChanged signal.
 *
 * \return pointer to the selected custom item, or null.
 *
 * \since QtDataVisualization 1.1
 *
 * \sa selectedElement
 */
QCustom3DItem *QAbstract3DGraph::selectedCustomItem() const
{
    return d_ptr->m_visualController->selectedCustomItem();
}

/*!
 * \property QAbstract3DGraph::selectedElement
 *
 * Can be used to query the selected element type.
 * Type is valid until the next \c selectedElementChanged signal.
 *
 * \c selectedElementChanged signal is emitted when a selection is made in the graph.
 *
 * Signal can be used for example for implementing custom input handlers, as demonstrated in this
 * \l {Axis Range Dragging With Labels Example}{example}.
 *
 * \sa selectedLabelIndex(), selectedAxis(), selectedCustomItemIndex(), selectedCustomItem(),
 * Q3DBars::selectedSeries(), Q3DScatter::selectedSeries(), Q3DSurface::selectedSeries(),
 * Q3DScene::setSelectionQueryPosition()
 *
 * \since QtDataVisualization 1.1
 */
QAbstract3DGraph::ElementType QAbstract3DGraph::selectedElement() const
{
    return d_ptr->m_visualController->selectedElement();
}

/*!
 * Renders current frame to an image of \a imageSize. Default size is the window size. Image is
 * rendered with antialiasing level given in \a msaaSamples. Default level is \c{0}.
 *
 * \since QtDataVisualization 1.1
 *
 * \return rendered image.
 */
QImage QAbstract3DGraph::renderToImage(int msaaSamples, const QSize &imageSize)
{
    QSize renderSize = imageSize;
    if (renderSize.isEmpty())
        renderSize = size();
    return d_ptr->renderToImage(msaaSamples, renderSize);
}

/*!
 * \property QAbstract3DGraph::measureFps
 * \since QtDataVisualization 1.1
 *
 * If \c {true}, the rendering is done continuously instead of on demand, and currentFps property
 * is updated. Defaults to \c{false}.
 *
 * \sa currentFps
 */
void QAbstract3DGraph::setMeasureFps(bool enable)
{
    d_ptr->m_visualController->setMeasureFps(enable);
}

bool QAbstract3DGraph::measureFps() const
{
    return d_ptr->m_visualController->measureFps();
}

/*!
 * \property QAbstract3DGraph::currentFps
 * \since QtDataVisualization 1.1
 *
 * When fps measuring is enabled, the results for the last second are stored in this read-only
 * property. It takes at least a second before this value updates after measurement is activated.
 *
 * \sa measureFps
 */
qreal QAbstract3DGraph::currentFps() const
{
    return d_ptr->m_visualController->currentFps();
}

/*!
 * \property QAbstract3DGraph::orthoProjection
 * \since QtDataVisualization 1.1
 *
 * If \c {true}, orthographic projection will be used for displaying the graph.
 * \note Orthographic projection can be used to create 2D graphs by replacing the default input
 * handler with one that doesn't allow rotating the graph and setting the camera to view the graph
 * directly from the side or from the top. Also, axis labels typically need to be rotated when
 * viewing the graph from the sides.
 * Defaults to \c{false}.
 * \note Shadows will be disabled when set to \c{true}.
 *
 * \sa QAbstract3DAxis::labelAutoRotation, Q3DCamera::cameraPreset
 */
void QAbstract3DGraph::setOrthoProjection(bool enable)
{
    d_ptr->m_visualController->setOrthoProjection(enable);
}

bool QAbstract3DGraph::isOrthoProjection() const
{
    return d_ptr->m_visualController->isOrthoProjection();
}

/*!
 * \property QAbstract3DGraph::aspectRatio
 * \since QtDataVisualization 1.1
 *
 * The aspect ratio is the ratio of the graph scaling between the longest axis on the horizontal
 * plane and the Y-axis. Defaults to \c{2.0}.
 *
 * \note Has no effect on Q3DBars.
 *
 * \sa horizontalAspectRatio
 */
void QAbstract3DGraph::setAspectRatio(qreal ratio)
{
    d_ptr->m_visualController->setAspectRatio(ratio);
}

qreal QAbstract3DGraph::aspectRatio() const
{
    return d_ptr->m_visualController->aspectRatio();
}

/*!
 * \property QAbstract3DGraph::optimizationHints
 *
 * Defines if the rendering optimization is default or static. Default mode provides the full feature set at
 * reasonable performance. Static optimizes graph rendering and is ideal for large non-changing data sets.
 * It is slower with dynamic data changes and item rotations. Selection is not optimized, so using it with
 * massive data sets is not advisable. Static works only on the Scatter graph and lacks the range gradient
 * color style for points.
 * Defaults to \c{OptimizationDefault}.
 */
void QAbstract3DGraph::setOptimizationHints(OptimizationHints hints)
{
    d_ptr->m_visualController->setOptimizationHints(hints);
}

QAbstract3DGraph::OptimizationHints QAbstract3DGraph::optimizationHints() const
{
    return d_ptr->m_visualController->optimizationHints();
}

/*!
 * \property QAbstract3DGraph::polar
 * \since QtDataVisualization 1.2
 *
 * If \c {true}, the horizontal axes are changed into polar axes. The X axis becomes the
 * angular axis and the Z axis becomes the radial axis.
 * Polar mode is not available for bar graphs.
 *
 * Defaults to \c{false}.
 *
 * \sa orthoProjection, radialLabelOffset
 */
void QAbstract3DGraph::setPolar(bool enable)
{
    d_ptr->m_visualController->setPolar(enable);
}

bool QAbstract3DGraph::isPolar() const
{
    return d_ptr->m_visualController->isPolar();
}

/*!
 * \property QAbstract3DGraph::radialLabelOffset
 * \since QtDataVisualization 1.2
 *
 * This property specifies the normalized horizontal offset for the axis labels of the radial
 * polar axis. The value 0.0 indicates the labels should be drawn next to the 0-angle angular
 * axis grid line. The value 1.0 indicates the labels are drawn on their normal place at the edge
 * of the graph background.
 * This property is ignored if polar property value is \c{false}. Defaults to 1.0.
 *
 * \sa polar
 */
void QAbstract3DGraph::setRadialLabelOffset(float offset)
{
    d_ptr->m_visualController->setRadialLabelOffset(offset);
}

float QAbstract3DGraph::radialLabelOffset() const
{
    return d_ptr->m_visualController->radialLabelOffset();
}

/*!
 * \property QAbstract3DGraph::horizontalAspectRatio
 * \since QtDataVisualization 1.2
 *
 * The horizontal aspect ratio is the ratio of the graph scaling between the X and Z axes.
 * Value of 0.0 indicates automatic scaling according to axis ranges.
 * Defaults to \c{0.0}.
 *
 * \note Has no effect on Q3DBars, which handles scaling on the horizontal plane via
 * \l{Q3DBars::barThickness}{barThickness} and \l{Q3DBars::barSpacing}{barSpacing} properties.
 * Polar graphs also ignore this property.
 *
 * \sa aspectRatio, polar, Q3DBars::barThickness, Q3DBars::barSpacing
 */
void QAbstract3DGraph::setHorizontalAspectRatio(qreal ratio)
{
    d_ptr->m_visualController->setHorizontalAspectRatio(ratio);
}

qreal QAbstract3DGraph::horizontalAspectRatio() const
{
    return d_ptr->m_visualController->horizontalAspectRatio();
}

/*!
 * \property QAbstract3DGraph::reflection
 * \since QtDataVisualization 1.2
 *
 * Sets floor reflections on or off. Defaults to \c{false}.
 *
 * \note Affects only Q3DBars.
 *
 * \sa reflectivity
 */
void QAbstract3DGraph::setReflection(bool enable)
{
    d_ptr->m_visualController->setReflection(enable);
}

bool QAbstract3DGraph::isReflection() const
{
    return d_ptr->m_visualController->reflection();
}

/*!
 * \property QAbstract3DGraph::reflectivity
 * \since QtDataVisualization 1.2
 *
 * Adjusts floor reflectivity, larger number being more reflective. Valid range is \c{[0...1]}.
 * Defaults to \c{0.5}.
 *
 * \note Affects only Q3DBars.
 *
 * \sa reflection
 */
void QAbstract3DGraph::setReflectivity(qreal reflectivity)
{
    d_ptr->m_visualController->setReflectivity(reflectivity);
}

qreal QAbstract3DGraph::reflectivity() const
{
    return d_ptr->m_visualController->reflectivity();
}

/*!
 * \internal
 */
bool QAbstract3DGraph::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        d_ptr->renderNow();
        return true;
    case QEvent::TouchBegin:
    case QEvent::TouchCancel:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        d_ptr->m_visualController->touchEvent(static_cast<QTouchEvent *>(event));
        return true;
    default:
        return QWindow::event(event);
    }
}

/*!
 * \internal
 */
void QAbstract3DGraph::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    Q3DScene *scene = d_ptr->m_visualController->scene();
    scene->d_ptr->setWindowSize(QSize(width(), height()));
    scene->d_ptr->setViewport(QRect(0, 0, width(), height()));
}

/*!
 * \internal
 */
void QAbstract3DGraph::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        d_ptr->renderNow();
}

/*!
 * \internal
 */
void QAbstract3DGraph::mouseDoubleClickEvent(QMouseEvent *event)
{
    d_ptr->m_visualController->mouseDoubleClickEvent(event);
}

/*!
 * \internal
 */
void QAbstract3DGraph::touchEvent(QTouchEvent *event)
{
    d_ptr->m_visualController->touchEvent(event);
}

/*!
 * \internal
 */
void QAbstract3DGraph::mousePressEvent(QMouseEvent *event)
{
    d_ptr->m_visualController->mousePressEvent(event, event->pos());
}

/*!
 * \internal
 */
void QAbstract3DGraph::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->m_visualController->mouseReleaseEvent(event, event->pos());
}

/*!
 * \internal
 */
void QAbstract3DGraph::mouseMoveEvent(QMouseEvent *event)
{
    d_ptr->m_visualController->mouseMoveEvent(event, event->pos());
}

/*!
 * \internal
 */
void QAbstract3DGraph::wheelEvent(QWheelEvent *event)
{
    d_ptr->m_visualController->wheelEvent(event);
}

QAbstract3DGraphPrivate::QAbstract3DGraphPrivate(QAbstract3DGraph *q)
    : QObject(0),
      q_ptr(q),
      m_updatePending(false),
      m_visualController(0),
      m_devicePixelRatio(1.f),
      m_offscreenSurface(0)
{
}

QAbstract3DGraphPrivate::~QAbstract3DGraphPrivate()
{
    if (m_offscreenSurface) {
        m_offscreenSurface->destroy();
        delete m_offscreenSurface;
    }
    if (m_context)
        m_context->makeCurrent(q_ptr);

    delete m_visualController;
}

void QAbstract3DGraphPrivate::setVisualController(Abstract3DController *controller)
{
    m_visualController = controller;

    QObject::connect(m_visualController, &Abstract3DController::activeInputHandlerChanged, q_ptr,
                     &QAbstract3DGraph::activeInputHandlerChanged);
    QObject::connect(m_visualController, &Abstract3DController::activeThemeChanged, q_ptr,
                     &QAbstract3DGraph::activeThemeChanged);
    QObject::connect(m_visualController, &Abstract3DController::selectionModeChanged, q_ptr,
                     &QAbstract3DGraph::selectionModeChanged);
    QObject::connect(m_visualController, &Abstract3DController::shadowQualityChanged, q_ptr,
                     &QAbstract3DGraph::shadowQualityChanged);
    QObject::connect(m_visualController, &Abstract3DController::optimizationHintsChanged, q_ptr,
                     &QAbstract3DGraph::optimizationHintsChanged);
    QObject::connect(m_visualController, &Abstract3DController::elementSelected, q_ptr,
                     &QAbstract3DGraph::selectedElementChanged);

    QObject::connect(m_visualController, &Abstract3DController::needRender, this,
                     &QAbstract3DGraphPrivate::renderLater);

    QObject::connect(m_visualController, &Abstract3DController::axisXChanged, this,
                     &QAbstract3DGraphPrivate::handleAxisXChanged);
    QObject::connect(m_visualController, &Abstract3DController::axisYChanged, this,
                     &QAbstract3DGraphPrivate::handleAxisYChanged);
    QObject::connect(m_visualController, &Abstract3DController::axisZChanged, this,
                     &QAbstract3DGraphPrivate::handleAxisZChanged);

    QObject::connect(m_visualController, &Abstract3DController::measureFpsChanged, q_ptr,
                     &QAbstract3DGraph::measureFpsChanged);
    QObject::connect(m_visualController, &Abstract3DController::currentFpsChanged, q_ptr,
                     &QAbstract3DGraph::currentFpsChanged);

    QObject::connect(m_visualController, &Abstract3DController::orthoProjectionChanged, q_ptr,
                     &QAbstract3DGraph::orthoProjectionChanged);

    QObject::connect(m_visualController, &Abstract3DController::aspectRatioChanged, q_ptr,
                     &QAbstract3DGraph::aspectRatioChanged);
    QObject::connect(m_visualController, &Abstract3DController::polarChanged, q_ptr,
                     &QAbstract3DGraph::polarChanged);
    QObject::connect(m_visualController, &Abstract3DController::radialLabelOffsetChanged, q_ptr,
                     &QAbstract3DGraph::radialLabelOffsetChanged);
    QObject::connect(m_visualController, &Abstract3DController::horizontalAspectRatioChanged, q_ptr,
                     &QAbstract3DGraph::horizontalAspectRatioChanged);

    QObject::connect(m_visualController, &Abstract3DController::reflectionChanged, q_ptr,
                     &QAbstract3DGraph::reflectionChanged);
    QObject::connect(m_visualController, &Abstract3DController::reflectivityChanged, q_ptr,
                     &QAbstract3DGraph::reflectivityChanged);
}

void QAbstract3DGraphPrivate::handleDevicePixelRatioChange()
{
    if (q_ptr->devicePixelRatio() == m_devicePixelRatio || !m_visualController)
        return;

    m_devicePixelRatio = q_ptr->devicePixelRatio();
    m_visualController->scene()->setDevicePixelRatio(m_devicePixelRatio);
}

void QAbstract3DGraphPrivate::render()
{
    handleDevicePixelRatioChange();
    m_visualController->synchDataToRenderer();
    m_visualController->render();
}

void QAbstract3DGraphPrivate::renderLater()
{
    if (!m_updatePending) {
        m_updatePending = true;
        QCoreApplication::postEvent(q_ptr, new QEvent(QEvent::UpdateRequest));
    }
}

void QAbstract3DGraphPrivate::renderNow()
{
    if (!q_ptr->isExposed())
        return;

    m_updatePending = false;

    m_context->makeCurrent(q_ptr);

    render();

    m_context->swapBuffers(q_ptr);
}

QImage QAbstract3DGraphPrivate::renderToImage(int msaaSamples, const QSize &imageSize)
{
    QImage image;
    QOpenGLFramebufferObject *fbo;
    QOpenGLFramebufferObjectFormat fboFormat;
    if (!m_offscreenSurface) {
        // Create an offscreen surface for rendering to images without rendering on screen
        m_offscreenSurface = new QOffscreenSurface(q_ptr->screen());
        m_offscreenSurface->setFormat(q_ptr->requestedFormat());
        m_offscreenSurface->create();
    }
    // Render the wanted frame offscreen
    m_context->makeCurrent(m_offscreenSurface);
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    fboFormat.setInternalTextureFormat(GL_RGB);
    fboFormat.setSamples(msaaSamples);
    fbo = new QOpenGLFramebufferObject(imageSize, fboFormat);
    if (fbo->isValid()) {
        QRect originalViewport = m_visualController->m_scene->viewport();
        m_visualController->m_scene->d_ptr->setWindowSize(imageSize);
        m_visualController->m_scene->d_ptr->setViewport(QRect(0, 0,
                                                              imageSize.width(),
                                                              imageSize.height()));
        m_visualController->synchDataToRenderer();
        fbo->bind();
        m_context->swapBuffers(m_offscreenSurface);
        m_visualController->requestRender(fbo);
        image = fbo->toImage();
        fbo->release();
        m_visualController->m_scene->d_ptr->setWindowSize(originalViewport.size());
        m_visualController->m_scene->d_ptr->setViewport(originalViewport);
    }
    delete fbo;
    m_context->makeCurrent(q_ptr);

    return image;
}

QT_END_NAMESPACE_DATAVISUALIZATION

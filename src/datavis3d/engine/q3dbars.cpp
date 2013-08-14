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

#include "q3dbars.h"
#include "q3dbars_p.h"
#include "bars3dcontroller_p.h"
#include "qvalueaxis.h"

#include <QMouseEvent>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

/*!
 * \class Q3DBars
 * \inmodule QtDataVis3D
 * \brief The Q3DBars class provides methods for rendering 3D bar graphs.
 * \since 1.0.0
 *
 * This class enables developers to render bar graphs in 3D and to view them by rotating the scene
 * freely. Rotation is done by holding down the right mouse button and moving the mouse. Zooming
 * is done by mouse wheel. Selection, if enabled, is done by left mouse button. The scene can be
 * reset to default camera view by clicking mouse wheel. In touch devices rotation is done
 * by tap-and-move, selection by tap-and-hold and zoom by pinch.
 *
 * Methods are provided for changing bar types, themes, bar selection modes and so on. See the
 * methods for more detailed descriptions.
 *
 * \section1 How to construct a minimal Q3DBars chart
 *
 * After constructing Q3DBars, you need to set up sample space using setupSampleSpace(). Let's
 * set the sample space to 5 rows and 5 columns:
 *
 * \snippet doc_src_q3dbars_construction.cpp 0
 *
 * Now Q3DBars is ready to receive data to be rendered. Add one row of 5 floats into the data
 * set:
 *
 * \snippet doc_src_q3dbars_construction.cpp 1
 *
 * \note We set the sample space to 5 x 5, but we are inserting only one row of data. This is ok,
 * the rest of the rows will just be blank.
 *
 * Finally you will need to set it visible:
 *
 * \snippet doc_src_q3dbars_construction.cpp 2
 *
 * The complete code needed to create and display this chart is:
 *
 * \snippet doc_src_q3dbars_construction.cpp 3
 *
 * And this is what those few lines of code produce:
 *
 * \image q3dbars-minimal.png
 *
 * The scene can be rotated and zoomed into, but no other interaction is included in this minimal
 * code example. You can learn more by familiarizing yourself with the examples provided, like
 * the \l{Rainfall Example} or the \l{Widget Example}.
 *
 * \sa Q3DMaps, {Qt Data Visualization 3D C++ Classes}
 */

/*!
 * Constructs a new 3D bar window.
 */
Q3DBars::Q3DBars()
    : d_ptr(new Q3DBarsPrivate(this, geometry()))
{
    d_ptr->m_shared->initializeOpenGL();
    QObject::connect(d_ptr->m_shared, &Bars3dController::shadowQualityChanged, this,
                     &Q3DBars::handleShadowQualityUpdate);
}

/*!
 *  Destroys the 3d bar window.
 */
Q3DBars::~Q3DBars()
{
}

/*!
 * \internal
 */
void Q3DBars::render()
{
    d_ptr->m_shared->render();
}

void Q3DBars::handleShadowQualityUpdate(QDataVis::ShadowQuality quality)
{
    emit shadowQualityChanged(quality);
}

#if defined(Q_OS_ANDROID)
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
#endif

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

// TODO: Document
// Size
void Q3DBars::setWidth(const int width)
{
    d_ptr->m_shared->setWidth(width);
    QWindow::setWidth(width);
}

void Q3DBars::setHeight(const int height)
{
    d_ptr->m_shared->setHeight(height);
    QWindow::setHeight(height);
}

/*!
 * \a thickness Thickness of a bar in x and z axes.
 *
 * \a spacing Spacing between bars in x and z axes. If relative -flag is true, value of 0.0f
 * means the bars are side-to-side and for example 1.0f means there is one thickness in between the
 * bars.
 *
 * \a relative A flag to indicate if spacing is meant to be absolute or relative. \c true by
 * default.
 *
 * Sets bar specifications. Bar thickness is relative, as scene is automatically scaled to fit into
 * the view.
 */
void Q3DBars::setBarSpecs(QSizeF thickness, QSizeF spacing, bool relative)
{
    d_ptr->m_shared->setBarSpecs(thickness, spacing, relative);
}

/*!
 * \a style One of the values in \c BarStyle. \c Bars by default.
 *
 * \a smooth A flag to set shading to smooth. \c false by default.
 *
 * Sets the bar type to one of the supplied ones.
 *
 * \sa setMeshFileName()
 */
void Q3DBars::setBarType(QDataVis::MeshStyle style, bool smooth)
{
    d_ptr->m_shared->setBarType(style, smooth);
}

/*!
 * \a samplesRow How many rows of data there will be.
 *
 * \a samplesColumn How many items there are per row.
 *
 * Set up sample space. This must be called to initialize the sample space before adding data to the
 * Q3DBars.
 *
 * \sa addDataRow(), addDataSet()
 */
void Q3DBars::setupSampleSpace(int samplesRow, int samplesColumn)
{
    d_ptr->m_shared->setupSampleSpace(samplesRow, samplesColumn);
}

QSize Q3DBars::sampleSpace() const
{
    return QSize(d_ptr->m_shared->rowCount(), d_ptr->m_shared->columnCount());
}

/*!
 * \a preset Move camera to a predefined position from \c QDataVis::CameraPreset.
 *
 * Moves camera to a predefined position.
 */
void Q3DBars::setCameraPreset(QDataVis::CameraPreset preset)
{
    d_ptr->m_shared->setCameraPreset(preset);
}

/*!
 * \a horizontal Horizontal angle for camera.
 *
 * \a vertical Vertical angle for camera.
 *
 * \a distance Distance from the center. \c 100 by default.
 *
 * Move camera to a wanted position based on horizontal and veritcal angles. Angles are limited
 * to -180...180 in horizontal direction and either -90...90 or 0...90 in vertical, depending
 * on data values. Negative vertical angles are allowed only if there are negative bar values.
 * Distance is adjustable between 10 and 500.
 */
void Q3DBars::setCameraPosition(qreal horizontal, qreal vertical, int distance)
{
    d_ptr->m_shared->setCameraPosition(GLfloat(horizontal), GLfloat(vertical), GLint(distance));
}

/*!
 * \a theme Apply a predefined theme from \c QDataVis::ColorTheme.
 *
 * Sets a predefined theme. Theme affects bar colors, label colors, text color, background color,
 * window color and grid color. Lighting is also adjusted by themes.
 */
void Q3DBars::setTheme(QDataVis::ColorTheme theme)
{
    d_ptr->m_shared->setColorTheme(theme);
}

/*!
 * \a baseColor The base color of a bar. If all other colors are black, this sets the final color of
 * the bar.
 *
 * \a heightColor This color is added to the bar based on its height. The higher the bar, the more
 * prominent this color becomes. Setting this black keeps the color unchanged regardless of height.
 *
 * \a depthColor This color becomes more prominent the further away from the first row the bar is.
 * Setting this black keeps bars the same color regardless of "depth" in the set.
 *
 * \a uniform A flag to define if color needs to be uniform throughout bar's length, or will the
 * colors be applied by height. \c true by default.
 *
 * Set bar color using your own colors. This overrides colors from theme.
 */
void Q3DBars::setBarColor(QColor baseColor, QColor heightColor, QColor depthColor, bool uniform)
{
    d_ptr->m_shared->setObjectColor(baseColor, heightColor, depthColor, uniform);
}

/*!
 * \property Q3DBars::selectionMode
 *
 * \a mode Set bar selection mode from \c QDataVis::SelectionMode. \c ModeItem by default.
 *
 * Sets bar selection mode to be used.
 */
void Q3DBars::setSelectionMode(QDataVis::SelectionMode mode)
{
    d_ptr->m_shared->setSelectionMode(mode);
}

QDataVis::SelectionMode Q3DBars::selectionMode() const
{
    return d_ptr->m_shared->selectionMode();
}

/*!
 * \property Q3DBars::windowTitle
 *
 * \a title QString label to be used as window title.
 *
 * Sets the window title. The default is application executable name.
 */
void Q3DBars::setWindowTitle(const QString &title)
{
    setTitle(title);
}

QString Q3DBars::windowTitle() const
{
    return title();
}

/*!
 * \a objFileName File name of a mesh object. Object needs to be in Wavefront obj format
 * and include vertices, normals and UVs. It also needs to be in triangles.
 *
 * Override bar type with an object mesh. \sa setBarType()
 */
void Q3DBars::setMeshFileName(const QString &objFileName)
{
    d_ptr->m_shared->setMeshFileName(objFileName);
}

/*!
 * \property Q3DBars::fontSize
 *
 * \a fontsize Size of the font.
 *
 * Sets font size.
 */
void Q3DBars::setFontSize(float fontsize)
{
    d_ptr->m_shared->setFontSize(fontsize);
}

float Q3DBars::fontSize()
{
    return d_ptr->m_shared->fontSize();
}

/*!
 * \property Q3DBars::font
 *
 * \a font QFont to be used for labels. \c Arial by default.
 *
 * Sets the font for labels.
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
 * \property Q3DBars::labelTransparency
 *
 * \a transparency Transparency level of labels from \c QDataVis::LabelTransparency.
 * \c TransparencyFromTheme by default.
 *
 * Sets label transparency.
 */
void Q3DBars::setLabelTransparency(QDataVis::LabelTransparency transparency)
{
    d_ptr->m_shared->setLabelTransparency(transparency);
}

QDataVis::LabelTransparency Q3DBars::labelTransparency() const
{
    return d_ptr->m_shared->labelTransparency();
}

/*!
 * \property Q3DBars::gridVisible
 *
 * \a visible Flag to enable or disable grid. \c true by default.
 *
 * Sets grid drawing on or off.
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
 * \a visible Flag to enable or disable background. \c true by default.
 *
 * Sets backround rendering on or off.
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
 * \a quality Shadow quality from \c QDataVis::ShadowQuality. \c ShadowLow by default.
 *
 * Sets shadow quality. If setting QDataVis::ShadowQuality of a certain level fails, a level is lowered
 * until it is successful and shadowQualityChanged signal is emitted for each time the change is done.
 */
void Q3DBars::setShadowQuality(QDataVis::ShadowQuality quality)
{
    return d_ptr->m_shared->setShadowQuality(quality);
}

QDataVis::ShadowQuality Q3DBars::shadowQuality() const
{
    return d_ptr->m_shared->shadowQuality();
}

QCategoryAxis *Q3DBars::rowAxis()
{
    return reinterpret_cast<QCategoryAxis *>(d_ptr->m_shared->axisX());
}

QCategoryAxis *Q3DBars::columnAxis()
{
    return reinterpret_cast<QCategoryAxis *>(d_ptr->m_shared->axisZ());
}

void Q3DBars::setValueAxis(QValueAxis *axis)
{
    Q_ASSERT(axis);

    return d_ptr->m_shared->setAxisY(axis);
}

QValueAxis *Q3DBars::valueAxis()
{
    return static_cast<QValueAxis *>(d_ptr->m_shared->axisY());
}

void Q3DBars::setDataProxy(QBarDataProxy *proxy)
{
    d_ptr->m_shared->setDataProxy(proxy);
}

QBarDataProxy *Q3DBars::dataProxy()
{
    return d_ptr->m_shared->dataProxy();
}

Q3DBarsPrivate::Q3DBarsPrivate(Q3DBars *q, QRect rect)
    : q_ptr(q),
      m_shared(new Bars3dController(rect))
{
}

Q3DBarsPrivate::~Q3DBarsPrivate()
{
    qDebug() << "Destroying Q3DBarsPrivate";
    delete m_shared;
}

QT_DATAVIS3D_END_NAMESPACE

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "q3dbars.h"
#include "q3dbars_p.h"
#include "bars3dcontroller_p.h"

#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QScreen>
#include <QMouseEvent>

#include <qmath.h>

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
    \enum BarStyle

    Predefined bar types.

    \value Bars
           Basic cubic bar.
    \value Pyramids
           Four -sided pyramid.
    \value Cones
           Basic cone.
    \value Cylinders
           Basic cylinder.
    \value BevelBars
           Slilghtly beveled (rounded) cubic bar.
    \value Spheres
           Sphere. Not usable in Q3DBars.
*/

/*!
    \enum CameraPreset

    Predefined positions for camera.

    \value PresetFrontLow
    \value PresetFront
    \value PresetFrontHigh
    \value PresetLeftLow
    \value PresetLeft
    \value PresetLeftHigh
    \value PresetRightLow
    \value PresetRight
    \value PresetRightHigh
    \value PresetBehindLow
    \value PresetBehind
    \value PresetBehindHigh
    \value PresetIsometricLeft
    \value PresetIsometricLeftHigh
    \value PresetIsometricRight
    \value PresetIsometricRightHigh
    \value PresetDirectlyAbove
    \value PresetDirectlyAboveCW45
    \value PresetDirectlyAboveCCW45
    \value PresetFrontBelow
           From PresetFrontBelow onward these only  work for graphs including negative values.
           They act as Preset...Low for positive-only values.
    \value PresetLeftBelow
    \value PresetRightBelow
    \value PresetBehindBelow
    \value PresetDirectlyBelow
           Acts as PresetFrontLow for positive -only bars.
*/

/*!
    \enum ColorTheme

    Predefined color themes.

    \value ThemeSystem
    \value ThemeBlueCerulean
    \value ThemeBlueIcy
    \value ThemeBlueNcs
    \value ThemeBrownSand
    \value ThemeDark
    \value ThemeHighContrast
    \value ThemeLight
*/

/*!
    \enum SelectionMode

    Bar selection modes.

    \value ModeNone
           Selection mode disabled.
    \value ModeBar
           Selection selects a single bar.
    \value ModeBarAndRow
           Selection selects a single bar and highlights the row it is on.
    \value ModeBarAndColumn
           Selection selects a single bar and highlights the column it is on.
    \value ModeBarRowAndColumn
           Selection selects a single bar and highlights the row and the column it is on.
    \value ModeZoomRow
           Selection selects a single bar and displays the row it is on in a separate view. The
           original view is shrunk into upper left corner. Original view is restored by clicking
           on it.
    \value ModeZoomColumn
           Selection selects a single bar and displays the column it is on in a separate view. The
           original view is shrunk into upper left corner. Original view is restored by clicking
           on it.
*/

/*!
    \enum ShadowQuality

    Quality of shadows.

    \value ShadowNone
           Shadows are disabled.
    \value ShadowLow
           Shadows are rendered in low quality.
    \value ShadowMedium
           Shadows are rendered in medium quality.
    \value ShadowHigh
           Shadows are rendered in high quality.
*/

/*!
    \enum LabelTransparency

    Label transparencies.

    \value TransparencyNone
           Full solid, using colors from theme.
    \value TransparencyFromTheme
           Use colors and transparencies from theme.
    \value TransparencyNoBackground
           Draw just text on transparent background.
*/

/*!
 * Constructs a new 3D bar window.
 */
Q3DBars::Q3DBars()
    : d_ptr(new Q3DBarsPrivate(this, geometry()))
{
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
void Q3DBars::initialize()
{
    d_ptr->m_shared->setWidth(width());
    d_ptr->m_shared->setHeight(height());
    d_ptr->m_shared->initializeOpenGL();
}

/*!
 * \internal
 */
void Q3DBars::render()
{
    d_ptr->m_shared->render();
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
    d_ptr->m_shared->mousePressEvent(event);
}

/*!
 * \internal
 */
void Q3DBars::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseReleaseEvent(event);
}

/*!
 * \internal
 */
void Q3DBars::mouseMoveEvent(QMouseEvent *event)
{
    d_ptr->m_shared->mouseMoveEvent(event);
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
    d_ptr->m_shared->setWidth(width());
    d_ptr->m_shared->setHeight(height());
    d_ptr->m_shared->resizeNotify();
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
void Q3DBars::setBarType(BarStyle style, bool smooth)
{
    d_ptr->m_shared->setBarType(style, smooth);
}

/*!
 * \a samplesRow How many rows of data there will be.
 *
 * \a samplesColumn How many items there are per row.
 *
 * \a labelRow QString label for the rows, ie. x -axis label.
 *
 * \a labelColumn QString label for the columns, ie. z -axis label.
 *
 * \a labelHeight QString label for height, ie. y -axis label.
 *
 * Set up sample space. This must be called to initialize the sample space before adding data to the
 * Q3DBars.
 *
 * \sa addDataRow(), addDataSet()
 */
void Q3DBars::setupSampleSpace(int samplesRow, int samplesColumn, const QString &labelRow,
                               const QString &labelColumn, const QString &labelHeight)
{
    d_ptr->m_shared->setupSampleSpace(samplesRow, samplesColumn, labelRow, labelColumn,
                                      labelHeight);
}

/*!
 * \a preset Move camera to a predefined position from \c CameraPreset.
 *
 * Moves camera to a predefined position.
 */
void Q3DBars::setCameraPreset(CameraPreset preset)
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
void Q3DBars::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    d_ptr->m_shared->setCameraPosition(horizontal, vertical, distance);
}

/*!
 * \a theme Apply a predefined theme from \c ColorTheme.
 *
 * Sets a predefined theme. Theme affects bar colors, label colors, text color, background color,
 * window color and grid color. Lighting is also adjusted by themes.
 */
void Q3DBars::setTheme(ColorTheme theme)
{
    d_ptr->m_shared->setTheme(theme);
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
    d_ptr->m_shared->setBarColor(baseColor, heightColor, depthColor, uniform);
}

/*!
 * \property Q3DBars::selectionMode
 *
 * \a mode Set bar selection mode from \c SelectionMode. \c ModeBar by default.
 *
 * Sets bar selection mode to be used.
 */
void Q3DBars::setSelectionMode(SelectionMode mode)
{
    d_ptr->m_shared->setSelectionMode(mode);
}

SelectionMode Q3DBars::selectionMode()
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

QString Q3DBars::windowTitle()
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

QFont Q3DBars::font()
{
    return d_ptr->m_shared->font();
}

/*!
 * \property Q3DBars::labelTransparency
 *
 * \a transparency Transparency level of labels from \c LabelTransparency.
 * \c TransparencyFromTheme by default.
 *
 * Sets label transparency.
 */
void Q3DBars::setLabelTransparency(LabelTransparency transparency)
{
    d_ptr->m_shared->setLabelTransparency(transparency);
}

LabelTransparency Q3DBars::labelTransparency()
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

bool Q3DBars::isGridVisible()
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

bool Q3DBars::isBackgroundVisible()
{
    return d_ptr->m_shared->backgroundEnabled();
}

/*!
 * \property Q3DBars::shadowQuality
 *
 * \a quality Shadow quality from \c ShadowQuality. \c ShadowLow by default.
 *
 * Sets shadow quality.
 */
void Q3DBars::setShadowQuality(ShadowQuality quality)
{
    d_ptr->m_shared->setShadowQuality(quality);
}

ShadowQuality Q3DBars::shadowQuality()
{
    return d_ptr->m_shared->shadowQuality();
}

/*!
 * \a tickCount How many ticks will be drawn. \c 5 by default.
 *
 * \a step How large a step each tick is.
 *
 * \a minimum Minimum value a bar in data set can have. Setting this correctly is especially
 * important if values can be negative, or autoscaling won't work correctly.
 *
 * Sets tick count and step. Note; tickCount * step should be the maximum possible value of data
 * set.
 */
void Q3DBars::setTickCount(GLint tickCount, GLfloat step, GLfloat minimum)
{
    d_ptr->m_shared->setTickCount(tickCount, step, minimum);
}

/*!
 * \a dataRow A vector of floats representing a single row of data. Sample space must be large
 * enough to hold the row.
 *
 * \a labelRow A QString label for the row.
 *
 * \a labelsColumn A vector of strings, one for each item in the row.
 *
 * Add a row of data. Each new row is added to the front of the sample space, moving previous
 * rows back (if sample space is more than one row deep).
 */
void Q3DBars::addDataRow(const QVector<float> &dataRow, const QString &labelRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_shared->addDataRow(dataRow, labelRow, labelsColumn);
}

/*!
 * \a dataRow A vector of QDataItems representing a single row of data. Sample space must be
 * large enough to hold the row. Ownership of QDataItems is transferred to Q3DBars.
 *
 * \a labelRow A QString label for the row.
 *
 * \a labelsColumn A vector of strings, one for each item in the row.
 *
 * Add a row of data. Each new row is added to the front of the sample space, moving previous
 * rows back (if sample space is more than one row deep).
 */
void Q3DBars::addDataRow(const QVector<QDataItem*> &dataRow, const QString &labelRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_shared->addDataRow(dataRow, labelRow, labelsColumn);
}

/*!
 * \a dataRow A QDataRow instance representing a single row of data. Sample space must be
 * large enough to hold the row. Ownership of QDataRow is transferred to Q3DBars.
 *
 * Add a row of data. Each new row is added to the front of the sample space, moving previous
 * rows back (if sample space is more than one row deep).
 */
void Q3DBars::addDataRow(QDataRow *dataRow)
{
    d_ptr->m_shared->addDataRow(dataRow);
}

/*!
 * \a data A vector of vector of floats representing the whole data set. Sample space must be
 * large enough to hold the set.
 *
 * \a labelsRow A vector of strings, one for each column in the row.
 *
 * \a labelsColumn A vector of strings, one for each row in the column.
 *
 * Adds a whole data set at once. If an old data set exists, it is deleted and replaced with the
 * new one.
 */
void Q3DBars::addDataSet(const QVector< QVector<float> > &data, const QVector<QString> &labelsRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_shared->addDataSet(data, labelsRow, labelsColumn);
}

/*!
 * \a data A vector of vector of QDataItems representing the whole data set. Sample space must
 * be large enough to hold the set. Ownership of QDataItems is transferred to Q3DBars.
 *
 * \a labelsRow A vector of strings, one for each column in the row.
 *
 * \a labelsColumn A vector of strings, one for each row in the column.
 *
 * Adds a whole data set at once. If an old data set exists, it is deleted and replaced with the
 * new one.
 */
void Q3DBars::addDataSet(const QVector< QVector<QDataItem*> > &data,
                         const QVector<QString> &labelsRow,
                         const QVector<QString> &labelsColumn)
{
    d_ptr->m_shared->addDataSet(data, labelsRow, labelsColumn);
}

/*!
 * \a dataSet A QDataSet instance holding the whole data set. Sample space must
 * be large enough to hold the set. Ownership of QDataSet is transferred to Q3DBars.
 *
 * Adds a whole data set at once. If an old data set exists, it is deleted and replaced with the
 * new one.
 */
void Q3DBars::addDataSet(QDataSet *dataSet)
{
    d_ptr->m_shared->addDataSet(dataSet);
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

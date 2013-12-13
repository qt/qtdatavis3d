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

#include "qabstract3dseries.h"
#include "qabstract3dseries_p.h"
#include "qabstractdataproxy_p.h"
#include "abstract3dcontroller_p.h"

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

/*!
 * \class QAbstract3DSeries
 * \inmodule QtDataVisualization
 * \brief Base class for all QtDataVisualization series.
 * \since Qt Data Visualization 1.0
 *
 * You use the visualization type specific inherited classes instead of the base class.
 * \sa QBar3DSeries, QScatter3DSeries, QSurface3DSeries, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmltype Abstract3DSeries
 * \inqmlmodule QtDataVisualization
 * \since QtDataVisualization 1.0
 * \ingroup datavisualization_qml
 * \instantiates QAbstract3DSeries
 * \brief Base type for all QtDataVisualization series.
 *
 * This type is uncreatable, but contains properties that are exposed via subtypes.
 * \sa Bar3DSeries, Scatter3DSeries, Surface3DSeries, {Qt Data Visualization Data Handling}
 */

/*!
 * \qmlproperty Abstract3DSeries.SeriesType Abstract3DSeries::type
 * The type of the series.
 */

/*!
 * \qmlproperty string Abstract3DSeries::itemLabelFormat
 *
 * Label format for data items in this series. This format is used for single item labels,
 * for example, when an item is selected. How the format is interpreted depends on series type. See
 * each series class documentation for more information.
 */

/*!
 * \qmlproperty bool Abstract3DSeries::visible
 * Sets the visibility of the series. If false, the series is not rendered.
 */

/*!
 * \qmlproperty Abstract3DSeries.Mesh Abstract3DSeries::mesh
 *
 * Sets the mesh of the items in the series, or the selection pointer in case of
 * Surface3DSeries. If the \a mesh is \l{QAbstract3DSeries::MeshUserDefined}{Abstract3DSeries.MeshUserDefined},
 * then the userDefinedMesh property must also be set for items to render properly.
 * The default value depends on the graph type.
 */

/*!
 * \qmlproperty bool Abstract3DSeries::meshSmooth
 *
 * If \a enable is \c true, smooth versions of predefined meshes set via mesh property are used.
 * This property doesn't affect custom meshes used when mesh is
 * \l{QAbstract3DSeries::MeshUserDefined}{Abstract3DSeries.MeshUserDefined}.
 * Defaults to \c false.
 */

/*!
 * \qmlproperty string Abstract3DSeries::userDefinedMesh
 *
 * Sets the \a fileName for user defined custom mesh for objects that is used when mesh
 * is \l{QAbstract3DSeries::MeshUserDefined}{Abstract3DSeries.MeshUserDefined}.
 * \note The file specified by \a fileName needs to be in Wavefront obj format and include
 * vertices, normals and UVs. It also needs to be in triangles.
 */

/*!
 * \qmlproperty Theme3D.ColorStyle Abstract3DSeries::colorStyle
 *
 * Sets the color \a style for the series.
 * See \l{Theme3D::colorStyle}{Theme3D.colorStyle}
 * documentation for more information.
 */

/*!
 * \qmlproperty Color Abstract3DSeries::baseColor
 *
 * Sets the base \c color of the series.
 * See \l{Theme3D::baseColors}{Theme3D.baseColors}
 * documentation for more information.
 *
 * \sa colorStyle
 */

/*!
 * \qmlproperty ColorGradient Abstract3DSeries::baseGradient
 *
 * Sets the base \c gradient of the series.
 * See \l{Theme3D::baseGradients}{Theme3D.baseGradients}
 * documentation for more information.
 *
 * \sa colorStyle
 */

/*!
 * \qmlproperty Color Abstract3DSeries::singleHighlightColor
 *
 * Sets the single item highlight \c color of the series.
 * See \l{Theme3D::singleHighlightColor}{Theme3D.singleHighlightColor}
 * documentation for more information.
 *
 * \sa colorStyle
 */

/*!
 * \qmlproperty ColorGradient Abstract3DSeries::singleHighlightGradient
 *
 * Sets the single item highlight \c gradient of the series.
 * See \l{Theme3D::singleHighlightGradient}{Theme3D.singleHighlightGradient}
 * documentation for more information.
 *
 * \sa colorStyle
 */

/*!
 * \qmlproperty Color Abstract3DSeries::multiHighlightColor
 *
 * Sets the multiple item highlight \c color of the series.
 * See \l{Theme3D::multiHighlightColor}{Theme3D.multiHighlightColor}
 * documentation for more information.
 *
 * \sa colorStyle
 */

/*!
 * \qmlproperty ColorGradient Abstract3DSeries::multiHighlightGradient
 *
 * Sets the multiple item highlight \c gradient of the series.
 * See \l{Theme3D::multiHighlightGradient}{Theme3D.multiHighlightGradient}
 * documentation for more information.
 *
 * \sa colorStyle
 */

/*!
 * \enum QAbstract3DSeries::SeriesType
 *
 * Type of the series.
 *
 * \value SeriesTypeNone
 *        No series type.
 * \value SeriesTypeBar
 *        Series type for Q3DBars.
 * \value SeriesTypeScatter
 *        Series type for Q3DScatter.
 * \value SeriesTypeSurface
 *        Series type for Q3DSurface.
 */

/*!
 *  \enum QAbstract3DSeries::Mesh
 *
 *  Predefined mesh types. All styles are not usable with all visualization types.
 *
 *  \value MeshUserDefined
 *         User defined mesh, set via QAbstract3DSeries::userDefinedMesh property.
 *  \value MeshBar
 *         Basic rectangular bar.
 *  \value MeshCube
 *         Basic cube.
 *  \value MeshPyramid
 *         Four-sided pyramid.
 *  \value MeshCone
 *         Basic cone.
 *  \value MeshCylinder
 *         Basic cylinder.
 *  \value MeshBevelBar
 *         Slightly beveled (rounded) rectangular bar.
 *  \value MeshBevelCube
 *         Slightly beveled (rounded) cube.
 *  \value MeshSphere
 *         Sphere.
 *  \value MeshMinimal
 *         The minimal 3D mesh: a triangular pyramid. Usable only with Q3DScatter.
 *  \value MeshPoint
 *         2D point. Usable only with Q3DScatter.
 *         \b Note: Shadows and color gradients do not affect this style.
 */

/*!
 * \internal
 */
QAbstract3DSeries::QAbstract3DSeries(QAbstract3DSeriesPrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/*!
 * Destroys QAbstract3DSeries.
 */
QAbstract3DSeries::~QAbstract3DSeries()
{
}

/*!
 * \property QAbstract3DSeries::type
 *
 * The type of the series.
 */
QAbstract3DSeries::SeriesType QAbstract3DSeries::type() const
{
    return d_ptr->m_type;
}

/*!
 * \property QAbstract3DSeries::itemLabelFormat
 *
 * Sets label \a format for data items in this series. This format is used for single item labels,
 * for example, when an item is selected. How the format is interpreted depends on series type. See
 * each series class documentation for more information.
 *
 * \sa QBar3DSeries, QScatter3DSeries, QSurface3DSeries
 */
void QAbstract3DSeries::setItemLabelFormat(const QString &format)
{
    if (d_ptr->m_itemLabelFormat != format) {
        d_ptr->setItemLabelFormat(format);
        emit itemLabelFormatChanged(format);
    }
}

QString QAbstract3DSeries::itemLabelFormat() const
{
    return d_ptr->m_itemLabelFormat;
}

/*!
 * \property QAbstract3DSeries::visible
 *
 * Sets the visibility of the series. If \a visible is false, the series is not rendered.
 * Defaults to true.
 */
void QAbstract3DSeries::setVisible(bool visible)
{
    if (d_ptr->m_visible != visible) {
        d_ptr->setVisible(visible);
        emit visibilityChanged(visible);
    }
}

bool QAbstract3DSeries::isVisible() const
{
    return d_ptr->m_visible;
}

/*!
 * \property QAbstract3DSeries::mesh
 *
 * Sets the mesh of the items in the series, or the selection pointer in case of
 * QSurface3DSeries. If the \a mesh is MeshUserDefined, then the userDefinedMesh property
 * must also be set for items to render properly. The default value depends on the graph type.
 */
void QAbstract3DSeries::setMesh(QAbstract3DSeries::Mesh mesh)
{
    if ((mesh == QAbstract3DSeries::MeshPoint || mesh == QAbstract3DSeries::MeshMinimal)
            && type() != QAbstract3DSeries::SeriesTypeScatter) {
        qWarning() << "Specified style is only supported for QScatter3DSeries.";
    } else if (d_ptr->m_mesh != mesh) {
        d_ptr->setMesh(mesh);
        emit meshChanged(mesh);
    }
}

QAbstract3DSeries::Mesh QAbstract3DSeries::mesh() const
{
    return d_ptr->m_mesh;
}

/*!
 * \property QAbstract3DSeries::meshSmooth
 *
 * If \a enable is \c true, smooth versions of predefined meshes set via mesh property are used.
 * This property doesn't affect custom meshes used when mesh is MeshUserDefined.
 * Defaults to \c false.
 */
void QAbstract3DSeries::setMeshSmooth(bool enable)
{
    if (d_ptr->m_meshSmooth != enable) {
        d_ptr->setMeshSmooth(enable);
        emit meshSmoothChanged(enable);
    }
}

bool QAbstract3DSeries::isMeshSmooth() const
{
    return d_ptr->m_meshSmooth;
}

/*!
 * \property QAbstract3DSeries::userDefinedMesh
 *
 * Sets the \a fileName for user defined custom mesh for objects that is used when mesh
 * is MeshUserDefined.
 * \note The file specified by \a fileName needs to be in Wavefront obj format and include
 * vertices, normals and UVs. It also needs to be in triangles.
 */
void QAbstract3DSeries::setUserDefinedMesh(const QString &fileName)
{
    if (d_ptr->m_userDefinedMesh != fileName) {
        d_ptr->setUserDefinedMesh(fileName);
        emit userDefinedMeshChanged(fileName);
    }
}

QString QAbstract3DSeries::userDefinedMesh() const
{
    return d_ptr->m_userDefinedMesh;
}

/*!
 * \property QAbstract3DSeries::colorStyle
 *
 * Sets the color \a style for the series.
 * See Q3DTheme::ColorStyle documentation for more information.
 */
void QAbstract3DSeries::setColorStyle(Q3DTheme::ColorStyle style)
{
    if (d_ptr->m_colorStyle != style) {
        d_ptr->setColorStyle(style);
        emit colorStyleChanged(style);
    }
    d_ptr->m_themeTracker.colorStyleOverride = true;
}

Q3DTheme::ColorStyle QAbstract3DSeries::colorStyle() const
{
    return d_ptr->m_colorStyle;
}

/*!
 * \property QAbstract3DSeries::baseColor
 *
 * Sets the base \c color of the series.
 * See Q3DTheme::baseColor documentation for more information.
 *
 * \sa colorStyle
 */
void QAbstract3DSeries::setBaseColor(const QColor &color)
{
    if (d_ptr->m_baseColor != color) {
        d_ptr->setBaseColor(color);
        emit baseColorChanged(color);
    }
    d_ptr->m_themeTracker.baseColorOverride = true;
}

QColor QAbstract3DSeries::baseColor() const
{
    return d_ptr->m_baseColor;
}

/*!
 * \property QAbstract3DSeries::baseGradient
 *
 * Sets the base \c gradient of the series.
 * See Q3DTheme::baseGradient documentation for more information.
 *
 * \sa colorStyle
 */
void QAbstract3DSeries::setBaseGradient(const QLinearGradient &gradient)
{
    if (d_ptr->m_baseGradient != gradient) {
        d_ptr->setBaseGradient(gradient);
        emit baseGradientChanged(gradient);
    }
    d_ptr->m_themeTracker.baseGradientOverride = true;
}

QLinearGradient QAbstract3DSeries::baseGradient() const
{
    return d_ptr->m_baseGradient;
}

/*!
 * \property QAbstract3DSeries::singleHighlightColor
 *
 * Sets the single item highlight \c color of the series.
 * See Q3DTheme::singleHighlightColor documentation for more information.
 *
 * \sa colorStyle
 */
void QAbstract3DSeries::setSingleHighlightColor(const QColor &color)
{
    if (d_ptr->m_singleHighlightColor != color) {
        d_ptr->setSingleHighlightColor(color);
        emit singleHighlightColorChanged(color);
    }
    d_ptr->m_themeTracker.singleHighlightColorOverride = true;
}

QColor QAbstract3DSeries::singleHighlightColor() const
{
    return d_ptr->m_singleHighlightColor;
}

/*!
 * \property QAbstract3DSeries::singleHighlightGradient
 *
 * Sets the single item highlight \c gradient of the series.
 * See Q3DTheme::singleHighlightGradient documentation for more information.
 *
 * \sa colorStyle
 */
void QAbstract3DSeries::setSingleHighlightGradient(const QLinearGradient &gradient)
{
    if (d_ptr->m_singleHighlightGradient != gradient) {
        d_ptr->setSingleHighlightGradient(gradient);
        emit singleHighlightGradientChanged(gradient);
    }
    d_ptr->m_themeTracker.singleHighlightGradientOverride = true;
}

QLinearGradient QAbstract3DSeries::singleHighlightGradient() const
{
    return d_ptr->m_singleHighlightGradient;
}

/*!
 * \property QAbstract3DSeries::multiHighlightColor
 *
 * Sets the multiple item highlight \c color of the series.
 * See Q3DTheme::multiHighlightColor documentation for more information.
 *
 * \sa colorStyle
 */
void QAbstract3DSeries::setMultiHighlightColor(const QColor &color)
{
    if (d_ptr->m_multiHighlightColor != color) {
        d_ptr->setMultiHighlightColor(color);
        emit multiHighlightColorChanged(color);
    }
    d_ptr->m_themeTracker.multiHighlightColorOverride = true;
}

QColor QAbstract3DSeries::multiHighlightColor() const
{
    return d_ptr->m_multiHighlightColor;
}

/*!
 * \property QAbstract3DSeries::multiHighlightGradient
 *
 * Sets the multiple item highlight \c gradient of the series.
 * See Q3DTheme::multiHighlightGradient documentation for more information.
 *
 * \sa colorStyle
 */
void QAbstract3DSeries::setMultiHighlightGradient(const QLinearGradient &gradient)
{
    if (d_ptr->m_multiHighlightGradient != gradient) {
        d_ptr->setMultiHighlightGradient(gradient);
        emit multiHighlightGradientChanged(gradient);
    }
    d_ptr->m_themeTracker.multiHighlightGradientOverride = true;
}

QLinearGradient QAbstract3DSeries::multiHighlightGradient() const
{
    return d_ptr->m_multiHighlightGradient;
}

// QAbstract3DSeriesPrivate

QAbstract3DSeriesPrivate::QAbstract3DSeriesPrivate(QAbstract3DSeries *q, QAbstract3DSeries::SeriesType type)
    : QObject(0),
      q_ptr(q),
      m_type(type),
      m_dataProxy(0),
      m_visible(true),
      m_controller(0),
      m_mesh(QAbstract3DSeries::MeshCube),
      m_meshSmooth(false),
      m_colorStyle(Q3DTheme::ColorStyleUniform)
{
}

QAbstract3DSeriesPrivate::~QAbstract3DSeriesPrivate()
{
    delete m_dataProxy;
}

QAbstractDataProxy *QAbstract3DSeriesPrivate::dataProxy() const
{
    return m_dataProxy;
}

void QAbstract3DSeriesPrivate::setDataProxy(QAbstractDataProxy *proxy)
{
    Q_ASSERT(proxy && proxy != m_dataProxy && !proxy->d_ptr->series());

    delete m_dataProxy;
    m_dataProxy = proxy;

    proxy->d_ptr->setSeries(q_ptr); // also sets parent

    if (m_controller) {
        connectControllerAndProxy(m_controller);
        m_controller->markDataDirty();
    }
}

void QAbstract3DSeriesPrivate::setController(Abstract3DController *controller)
{
    connectControllerAndProxy(controller);
    m_controller = controller;
    q_ptr->setParent(controller);
}

void QAbstract3DSeriesPrivate::setItemLabelFormat(const QString &format)
{
    m_itemLabelFormat = format;
    m_changeTracker.itemLabelFormatChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setVisible(bool visible)
{
    m_visible = visible;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setMesh(QAbstract3DSeries::Mesh mesh)
{
    m_mesh = mesh;
    m_changeTracker.meshChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setMeshSmooth(bool enable)
{
    m_meshSmooth = enable;
    m_changeTracker.meshSmoothChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setUserDefinedMesh(const QString &meshFile)
{
    m_userDefinedMesh = meshFile;
    m_changeTracker.userDefinedMeshChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setColorStyle(Q3DTheme::ColorStyle style)
{
    m_colorStyle = style;
    m_changeTracker.colorStyleChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setBaseColor(const QColor &color)
{
    m_baseColor = color;
    m_changeTracker.baseColorChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setBaseGradient(const QLinearGradient &gradient)
{
    m_baseGradient = gradient;
    m_changeTracker.baseGradientChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setSingleHighlightColor(const QColor &color)
{
    m_singleHighlightColor = color;
    m_changeTracker.singleHighlightColorChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setSingleHighlightGradient(const QLinearGradient &gradient)
{
    m_singleHighlightGradient = gradient;
    m_changeTracker.singleHighlightGradientChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setMultiHighlightColor(const QColor &color)
{
    m_multiHighlightColor = color;
    m_changeTracker.multiHighlightColorChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::setMultiHighlightGradient(const QLinearGradient &gradient)
{
    m_multiHighlightGradient = gradient;
    m_changeTracker.multiHighlightGradientChanged = true;
    if (m_controller)
        m_controller->markSeriesVisualsDirty();
}

void QAbstract3DSeriesPrivate::resetToTheme(const Q3DTheme &theme, int seriesIndex, bool force)
{
    int themeIndex = seriesIndex;
    if (force || !m_themeTracker.colorStyleOverride) {
        q_ptr->setColorStyle(theme.colorStyle());
        m_themeTracker.colorStyleOverride = false;
    }
    if (force || !m_themeTracker.baseColorOverride) {
        if (theme.baseColors().size() <= seriesIndex)
            themeIndex = seriesIndex % theme.baseColors().size();
        q_ptr->setBaseColor(theme.baseColors().at(themeIndex));
        m_themeTracker.baseColorOverride = false;
    }
    if (force || !m_themeTracker.baseGradientOverride) {
        if (theme.baseGradients().size() <= seriesIndex)
            themeIndex = seriesIndex % theme.baseGradients().size();
        q_ptr->setBaseGradient(theme.baseGradients().at(themeIndex));
        m_themeTracker.baseGradientOverride = false;
    }
    if (force || !m_themeTracker.singleHighlightColorOverride) {
        q_ptr->setSingleHighlightColor(theme.singleHighlightColor());
        m_themeTracker.singleHighlightColorOverride = false;
    }
    if (force || !m_themeTracker.singleHighlightGradientOverride) {
        q_ptr->setSingleHighlightGradient(theme.singleHighlightGradient());
        m_themeTracker.singleHighlightGradientOverride = false;
    }
    if (force || !m_themeTracker.multiHighlightColorOverride) {
        q_ptr->setMultiHighlightColor(theme.multiHighlightColor());
        m_themeTracker.multiHighlightColorOverride = false;
    }
    if (force || !m_themeTracker.multiHighlightGradientOverride) {
        q_ptr->setMultiHighlightGradient(theme.multiHighlightGradient());
        m_themeTracker.multiHighlightGradientOverride = false;
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE

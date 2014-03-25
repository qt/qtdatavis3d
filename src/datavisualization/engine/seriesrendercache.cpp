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

#include "seriesrendercache_p.h"
#include "objecthelper_p.h"
#include "abstract3drenderer_p.h"
#include "texturehelper_p.h"
#include "utils_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

const QString smoothString(QStringLiteral("Smooth"));

SeriesRenderCache::SeriesRenderCache()
    : m_series(0),
      m_object(0),
      m_mesh(QAbstract3DSeries::MeshCube),
      m_baseUniformTexture(0),
      m_baseGradientTexture(0),
      m_gradientImage(0),
      m_singleHighlightGradientTexture(0),
      m_multiHighlightGradientTexture(0)
{
}

SeriesRenderCache::~SeriesRenderCache()
{
}

void SeriesRenderCache::populate(QAbstract3DSeries *series, Abstract3DRenderer *renderer)
{
    Q_ASSERT(series);

    bool seriesChanged = false;

    if (m_series != series) {
        m_series = series;
        seriesChanged = true;
    }

    QAbstract3DSeriesChangeBitField &changeTracker = series->d_ptr->m_changeTracker;

    if (seriesChanged || changeTracker.itemLabelFormatChanged) {
        m_itemLabelFormat = series->itemLabelFormat();
        changeTracker.itemLabelFormatChanged = false;
    }

    if (seriesChanged || changeTracker.meshChanged  || changeTracker.meshSmoothChanged
            || changeTracker.userDefinedMeshChanged) {
        m_mesh = series->mesh();
        changeTracker.meshChanged = false;
        changeTracker.meshSmoothChanged = false;
        changeTracker.userDefinedMeshChanged = false;

        QString meshFileName;

        // Compose mesh filename
        if (m_mesh == QAbstract3DSeries::MeshUserDefined) {
            // Always use the supplied mesh directly
            meshFileName = series->userDefinedMesh();
        } else {
            switch (m_mesh) {
            case QAbstract3DSeries::MeshBar:
            case QAbstract3DSeries::MeshCube:
                meshFileName = QStringLiteral(":/defaultMeshes/bar");
                break;
            case QAbstract3DSeries::MeshPyramid:
                meshFileName = QStringLiteral(":/defaultMeshes/pyramid");
                break;
            case QAbstract3DSeries::MeshCone:
                meshFileName = QStringLiteral(":/defaultMeshes/cone");
                break;
            case QAbstract3DSeries::MeshCylinder:
                meshFileName = QStringLiteral(":/defaultMeshes/cylinder");
                break;
            case QAbstract3DSeries::MeshBevelBar:
            case QAbstract3DSeries::MeshBevelCube:
                meshFileName = QStringLiteral(":/defaultMeshes/bevelbar");
                break;
            case QAbstract3DSeries::MeshSphere:
                meshFileName = QStringLiteral(":/defaultMeshes/sphere");
                break;
            case QAbstract3DSeries::MeshMinimal:
                meshFileName = QStringLiteral(":/defaultMeshes/minimal");
                break;
            case QAbstract3DSeries::MeshArrow:
                meshFileName = QStringLiteral(":/defaultMeshes/arrow");
                break;
            case QAbstract3DSeries::MeshPoint:
#if defined(QT_OPENGL_ES_2)
                qWarning("QAbstract3DSeries::MeshPoint is not fully supported on OpenGL ES2");
#endif
                break;
            default:
                // Default to cube
                meshFileName = QStringLiteral(":/defaultMeshes/bar");
                break;
            }

            if (series->isMeshSmooth() && m_mesh != QAbstract3DSeries::MeshPoint)
                meshFileName += smoothString;

            // Give renderer an opportunity to customize the mesh
            renderer->fixMeshFileName(meshFileName, m_mesh);
        }

        delete m_object;
        if (meshFileName.isEmpty()) {
            m_object = 0;
        } else {
            m_object = new ObjectHelper(meshFileName);
            m_object->load();
        }
    }

    if (seriesChanged || changeTracker.meshRotationChanged) {
        m_meshRotation = series->meshRotation().normalized();
        if (m_series->type() == QAbstract3DSeries::SeriesTypeBar
                && (m_meshRotation.x() || m_meshRotation.z())) {
            m_meshRotation = identityQuaternion;
        }
        changeTracker.meshRotationChanged = false;
    }

    if (seriesChanged || changeTracker.colorStyleChanged) {
        m_colorStyle = series->colorStyle();
        changeTracker.colorStyleChanged = false;
    }

    if (seriesChanged || changeTracker.baseColorChanged) {
        m_baseColor = Utils::vectorFromColor(series->baseColor());
        if (m_series->type() == QAbstract3DSeries::SeriesTypeSurface)
            renderer->generateBaseColorTexture(series->baseColor(), &m_baseUniformTexture);
        changeTracker.baseColorChanged = false;
    }

    if (seriesChanged || changeTracker.baseGradientChanged) {
        QLinearGradient gradient = series->baseGradient();
        m_gradientImage = Utils::getGradientImage(gradient);
        renderer->fixGradientAndGenerateTexture(&gradient, &m_baseGradientTexture);
        changeTracker.baseGradientChanged = false;
    }

    if (seriesChanged || changeTracker.singleHighlightColorChanged) {
        m_singleHighlightColor = Utils::vectorFromColor(series->singleHighlightColor());
        changeTracker.singleHighlightColorChanged = false;
    }

    if (seriesChanged || changeTracker.singleHighlightGradientChanged) {
        QLinearGradient gradient = series->singleHighlightGradient();
        renderer->fixGradientAndGenerateTexture(&gradient, &m_singleHighlightGradientTexture);
        changeTracker.singleHighlightGradientChanged = false;
    }

    if (seriesChanged || changeTracker.multiHighlightColorChanged) {
        m_multiHighlightColor = Utils::vectorFromColor(series->multiHighlightColor());
        changeTracker.multiHighlightColorChanged = false;
    }

    if (seriesChanged || changeTracker.multiHighlightGradientChanged) {
        QLinearGradient gradient = series->multiHighlightGradient();
        renderer->fixGradientAndGenerateTexture(&gradient, &m_multiHighlightGradientTexture);
        changeTracker.multiHighlightGradientChanged = false;
    }

    if (seriesChanged || changeTracker.nameChanged) {
        m_name = series->name();
        changeTracker.nameChanged = false;
    }
}

void SeriesRenderCache::cleanup(TextureHelper *texHelper)
{
    delete m_object;
    if (QOpenGLContext::currentContext()) {
        texHelper->deleteTexture(&m_baseUniformTexture);
        texHelper->deleteTexture(&m_baseGradientTexture);
        texHelper->deleteTexture(&m_singleHighlightGradientTexture);
        texHelper->deleteTexture(&m_multiHighlightGradientTexture);
    }
}

QT_END_NAMESPACE_DATAVISUALIZATION

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

#include "surfaceobject_p.h"
#include "abstractobjecthelper_p.h"

#include <QVector3D>
#include <QVector2D>

#include <QDebug>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

SurfaceObject::SurfaceObject()
{
    m_indicesType = GL_UNSIGNED_INT;
    initializeOpenGLFunctions();
    glGenBuffers(1, &m_vertexbuffer);
    glGenBuffers(1, &m_normalbuffer);
    glGenBuffers(1, &m_uvbuffer);
    glGenBuffers(1, &m_elementbuffer);
    glGenBuffers(1, &m_gridElementbuffer);
}

SurfaceObject::~SurfaceObject()
{
    glDeleteBuffers(1, &m_gridElementbuffer);
}

void SurfaceObject::setUpSmoothData(const QSurfaceDataArray &dataArray, const QRect &space,
                                    GLfloat yRange, GLfloat yMin, bool changeGeometry)
{
    int columns = space.width();
    int rows = space.height();
    int totalSize = rows * columns;
    GLfloat xMin = dataArray.at(0)->at(0).x();
    GLfloat zMin = dataArray.at(0)->at(0).z();
    GLfloat xNormalizer = (dataArray.at(0)->last().x() - xMin) / 2.0f;
    GLfloat yNormalizer = yRange / 2.0f;
    GLfloat zNormalizer = (dataArray.last()->at(0).z() - zMin) / -2.0f;
    GLfloat uvX = 1.0 / GLfloat(columns - 1);
    GLfloat uvY = 1.0 / GLfloat(rows - 1);

    // Create/populate vertice table
    if (changeGeometry)
        m_vertices.resize(totalSize);

    QVector<QVector2D> uvs;
    if (changeGeometry)
        uvs.resize(totalSize);
    int totalIndex = 0;
    for (int i = 0; i < rows; i++) {
        const QSurfaceDataRow &p = *dataArray.at(i);
        for (int j = 0; j < columns; j++) {
            const QSurfaceDataItem &data = p.at(j);
            float normalizedX = ((data.x() - xMin) / xNormalizer);
            float normalizedY = ((data.y() - yMin) / yNormalizer);
            float normalizedZ = ((data.z() - zMin) / zNormalizer);
            m_vertices[totalIndex] = QVector3D(normalizedX - 1.0f, normalizedY - 1.0f, normalizedZ + 1.0f);
            if (changeGeometry)
                uvs[totalIndex] = QVector2D(GLfloat(j) * uvX, GLfloat(i) * uvY);
            totalIndex++;
        }
    }

    // Create normals
    int rowLimit = rows - 1;
    int colLimit = columns - 1;
    int rowColLimit = rowLimit * columns;
    int totalLimit = totalSize - 1;
    if (changeGeometry)
        m_normals.resize(totalSize);

    totalIndex = 0;
    for (int row = 0; row < rowColLimit; row += columns) {
        for (int j = 0; j < colLimit; j++) {
            m_normals[totalIndex++] = normal(m_vertices.at(row + j),
                                             m_vertices.at(row + j + 1),
                                             m_vertices.at(row + columns + j));
        }
        int p = row + colLimit;
        m_normals[totalIndex++] = normal(m_vertices.at(p),
                                         m_vertices.at(p + columns),
                                         m_vertices.at(p - 1));
    }
    for (int j = rowColLimit; j < totalLimit; j++) {
        m_normals[totalIndex++] = normal(m_vertices.at(j),
                                         m_vertices.at(j - columns),
                                         m_vertices.at(j + 1));
    }
    int p = rows * colLimit;
    m_normals[totalIndex++] = normal(m_vertices.at(p),
                                     m_vertices.at(p - 1),
                                     m_vertices.at(p - columns - 1));

    // Create indices table
    GLint *indices = 0;
    if (changeGeometry) {
        m_indexCount = 6 * colLimit * rowLimit;
        indices = new GLint[m_indexCount];
        p = 0;
        for (int row = 0; row < rowLimit * columns; row += columns) {
            for (int j = 0; j < colLimit; j++) {
                // Left triangle
                indices[p++] = row + j + 1;
                indices[p++] = row + columns + j;
                indices[p++] = row + j;

                // Right triangle
                indices[p++] = row + columns + j + 1;
                indices[p++] = row + columns + j;
                indices[p++] = row + j + 1;
            }
        }
    }

    // Create line element indices
    GLint *gridIndices = 0;
    if (changeGeometry) {
        m_gridIndexCount = 2 * columns * rowLimit + 2 * rows * colLimit;
        gridIndices = new GLint[m_gridIndexCount];
        p = 0;
        for (int i = 0, row = 0; i < rows; i++, row += columns) {
            for (int j = 0; j < colLimit; j++) {
                gridIndices[p++] = row + j;
                gridIndices[p++] = row + j + 1;
            }
        }
        for (int i = 0, row = 0; i < rowLimit; i++, row += columns) {
            for (int j = 0; j < columns; j++) {
                gridIndices[p++] = row + j;
                gridIndices[p++] = row + j + columns;
            }
        }
    }

    createBuffers(m_vertices, uvs, m_normals, indices, gridIndices, changeGeometry);

    delete[] indices;
    delete[] gridIndices;
}


void SurfaceObject::setUpData(const QSurfaceDataArray &dataArray, const QRect &space,
                              GLfloat yRange, GLfloat yMin, bool changeGeometry)
{
    int columns = space.width();
    int rows = space.height();
    int totalSize = rows * columns * 2;
    GLfloat xMin = dataArray.at(0)->at(0).x();
    GLfloat zMin = dataArray.at(0)->at(0).z();
    GLfloat xNormalizer = (dataArray.at(0)->last().x() - xMin) / 2.0f;
    GLfloat yNormalizer = yRange / 2.0f;
    GLfloat zNormalizer = (dataArray.last()->at(0).z() - zMin) / -2.0f;
    GLfloat uvX = 1.0 / GLfloat(columns - 1);
    GLfloat uvY = 1.0 / GLfloat(rows - 1);

    // Create vertice table
    if (changeGeometry)
        m_vertices.resize(totalSize);

    QVector<QVector2D> uvs;
    if (changeGeometry)
        uvs.resize(totalSize);

    int totalIndex = 0;
    int rowLimit = rows - 1;
    int colLimit = columns - 1;
    int doubleColumns = columns * 2 - 2;
    int rowColLimit = rowLimit * doubleColumns;

    for (int i = 0; i < rows; i++) {
        const QSurfaceDataRow &row = *dataArray.at(i);
        for (int j = 0; j < columns; j++) {
            const QSurfaceDataItem &data = row.at(j);
            float normalizedX = ((data.x() - xMin) / xNormalizer);
            float normalizedY = ((data.y() - yMin) / yNormalizer);
            float normalizedZ = ((data.z() - zMin) / zNormalizer);
            m_vertices[totalIndex] = QVector3D(normalizedX - 1.0f, normalizedY - 1.0f, normalizedZ + 1.0f);
            if (changeGeometry)
                uvs[totalIndex] = QVector2D(GLfloat(j) * uvX, GLfloat(i) * uvY);

            totalIndex++;

            if (j > 0 && j < colLimit) {
                m_vertices[totalIndex] = m_vertices[totalIndex - 1];
                if (changeGeometry)
                    uvs[totalIndex] = uvs[totalIndex - 1];
                totalIndex++;
            }
        }
    }

    // Create normals & indices table
    GLint *indices = 0;
    int p = 0;
    if (changeGeometry) {
        int normalCount = 2 * colLimit * rowLimit;
        m_indexCount = 3 * normalCount;
        indices = new GLint[m_indexCount];
        m_normals.resize(normalCount);
    }

    totalIndex = 0;
    for (int row = 0, upperRow = doubleColumns;
         row < rowColLimit;
         row += doubleColumns, upperRow += doubleColumns) {
        for (int j = 0; j < doubleColumns; j += 2) {
            // Normal for the left triangle
            m_normals[totalIndex++] = normal(m_vertices.at(row + j),
                                             m_vertices.at(row + j + 1),
                                             m_vertices.at(upperRow + j));

            // Normal for the right triangle
            m_normals[totalIndex++] = normal(m_vertices.at(row + j + 1),
                                             m_vertices.at(upperRow + j + 1),
                                             m_vertices.at(upperRow + j));

            if (changeGeometry) {
                // Left triangle
                indices[p++] = row + j + 1;
                indices[p++] = upperRow + j;
                indices[p++] = row + j;

                // Right triangle
                indices[p++] = upperRow + j + 1;
                indices[p++] = upperRow + j;
                indices[p++] = row + j + 1;
            }
        }
    }

    // Create grid line element indices
    GLint *gridIndices = 0;
    if (changeGeometry) {
        m_gridIndexCount = 2 * columns * rowLimit + 2 * rows * colLimit;
        gridIndices = new GLint[m_gridIndexCount];
        p = 0;
        int fullRowLimit = rows * doubleColumns;
        for (int row = 0; row < fullRowLimit; row += doubleColumns) {
            for (int j = 0; j < doubleColumns; j += 2) {
                gridIndices[p++] = row + j;
                gridIndices[p++] = row + j + 1;

                if (row < rowColLimit) {
                    gridIndices[p++] = row + j;
                    gridIndices[p++] = row + j + doubleColumns;
                }
            }
        }
        for (int i = doubleColumns - 1; i < rowColLimit; i += doubleColumns) {
            gridIndices[p++] = i;
            gridIndices[p++] = i  + doubleColumns;
        }
    }

    createBuffers(m_vertices, uvs, m_normals, indices, gridIndices, changeGeometry);

    delete[] indices;
    delete[] gridIndices;
}

void SurfaceObject::createBuffers(const QVector<QVector3D> &vertices, const QVector<QVector2D> &uvs,
                                  const QVector<QVector3D> &normals, const GLint *indices,
                                  const GLint *gridIndices, bool changeGeometry)
{
    // Move to buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D),
                 &vertices.at(0), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(QVector3D),
                 &normals.at(0), GL_DYNAMIC_DRAW);

    if (changeGeometry) {
        if (uvs.size()) {
            glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(QVector2D),
                         &uvs.at(0), GL_STATIC_DRAW);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(GLint),
                     indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gridElementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_gridIndexCount * sizeof(GLint),
                     gridIndices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_meshDataLoaded = true;
}

GLuint SurfaceObject::gridElementBuf()
{
    if (!m_meshDataLoaded)
        qFatal("No loaded object");
    return m_gridElementbuffer;
}

GLuint SurfaceObject::gridIndexCount()
{
    return m_gridIndexCount;
}

QVector3D SurfaceObject::normal(const QVector3D &a, const QVector3D &b, const QVector3D &c)
{
    QVector3D v1 = b - a;
    QVector3D v2 = c - a;
    return QVector3D::crossProduct(v1, v2);
}

QT_DATAVISUALIZATION_END_NAMESPACE

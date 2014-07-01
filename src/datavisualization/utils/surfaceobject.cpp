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

#include "surfaceobject_p.h"
#include "surface3drenderer_p.h"

#include <QtGui/QVector2D>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

SurfaceObject::SurfaceObject(Surface3DRenderer *renderer)
    : m_surfaceType(Undefined),
      m_columns(0),
      m_rows(0),
      m_gridIndexCount(0),
      m_axisCacheX(renderer->m_axisCacheX),
      m_axisCacheY(renderer->m_axisCacheY),
      m_axisCacheZ(renderer->m_axisCacheZ),
      m_renderer(renderer)

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
    if (QOpenGLContext::currentContext())
        glDeleteBuffers(1, &m_gridElementbuffer);
}

void SurfaceObject::setUpSmoothData(const QSurfaceDataArray &dataArray, const QRect &space,
                                    bool changeGeometry, bool polar, bool flipXZ)
{
    m_columns = space.width();
    m_rows = space.height();
    int totalSize = m_rows * m_columns;
    GLfloat uvX = 1.0f / GLfloat(m_columns - 1);
    GLfloat uvY = 1.0f / GLfloat(m_rows - 1);

    m_surfaceType = SurfaceSmooth;

    // Create/populate vertix table
    if (changeGeometry)
        m_vertices.resize(totalSize);

    QVector<QVector2D> uvs;
    if (changeGeometry)
        uvs.resize(totalSize);
    int totalIndex = 0;

    // Init min and max to ridiculous values
    m_minY = 10000000.0;
    m_maxY = -10000000.0f;

    for (int i = 0; i < m_rows; i++) {
        const QSurfaceDataRow &p = *dataArray.at(i);
        for (int j = 0; j < m_columns; j++) {
            getNormalizedVertex(p.at(j), m_vertices[totalIndex], polar, flipXZ);
            if (changeGeometry)
                uvs[totalIndex] = QVector2D(GLfloat(j) * uvX, GLfloat(i) * uvY);
            totalIndex++;
        }
    }

    if (flipXZ) {
        for (int i = 0; i < m_vertices.size(); i++) {
            m_vertices[i].setX(-m_vertices.at(i).x());
            m_vertices[i].setZ(-m_vertices.at(i).z());
        }
    }

    // Create normals
    int rowLimit = m_rows - 1;
    int colLimit = m_columns - 1;
    int rowColLimit = rowLimit * m_columns;
    int totalLimit = totalSize - 1;
    if (changeGeometry)
        m_normals.resize(totalSize);

    totalIndex = 0;
    const bool flipNormal = checkFlipNormal(dataArray);
    for (int row = 0; row < rowColLimit; row += m_columns) {
        for (int j = 0; j < colLimit; j++) {
            m_normals[totalIndex++] = normal(m_vertices.at(row + j),
                                             m_vertices.at(row + j + 1),
                                             m_vertices.at(row + m_columns + j),
                                             flipNormal);
        }
        int p = row + colLimit;
        m_normals[totalIndex++] = normal(m_vertices.at(p),
                                         m_vertices.at(p + m_columns),
                                         m_vertices.at(p - 1),
                                         flipNormal);
    }
    for (int j = rowColLimit; j < totalLimit; j++) {
        m_normals[totalIndex++] = normal(m_vertices.at(j),
                                         m_vertices.at(j - m_columns),
                                         m_vertices.at(j + 1),
                                         flipNormal);
    }
    m_normals[totalIndex++] = normal(m_vertices.at(totalLimit),
                                     m_vertices.at(totalLimit - 1),
                                     m_vertices.at(totalLimit - m_columns),
                                     flipNormal);

    // Create indices table
    if (changeGeometry)
        createSmoothIndices(0, 0, colLimit, rowLimit);

    // Create line element indices
    if (changeGeometry)
        createSmoothGridlineIndices(0, 0, colLimit, rowLimit);

    createBuffers(m_vertices, uvs, m_normals, 0, changeGeometry);
}

void SurfaceObject::updateSmoothRow(const QSurfaceDataArray &dataArray, int rowIndex, bool polar)
{
    // Update vertices
    int p = rowIndex * m_columns;
    const QSurfaceDataRow &dataRow = *dataArray.at(rowIndex);

    for (int j = 0; j < m_columns; j++)
        getNormalizedVertex(dataRow.at(j), m_vertices[p++], polar, false);

    // Create normals
    int colLimit = m_columns - 1;
    int startRow = rowIndex;
    if (startRow > 0)
        startRow--;
    int totalIndex = startRow * m_columns;
    int rowLimit = (rowIndex + 1) * m_columns;
    if (rowIndex == m_rows - 1)
        rowLimit = rowIndex * m_columns; // The rowIndex is top most row, special handling

    const bool flipNormal = checkFlipNormal(dataArray);
    for (int row = totalIndex; row < rowLimit; row += m_columns) {
        for (int j = 0; j < colLimit; j++) {
            // One right and one up
            m_normals[totalIndex++] = normal(m_vertices.at(row + j),
                                             m_vertices.at(row + j + 1),
                                             m_vertices.at(row + m_columns + j),
                                             flipNormal);
        }
        int p = row + colLimit;
        // One up and one left
        m_normals[totalIndex++] = normal(m_vertices.at(p),
                                         m_vertices.at(p + m_columns),
                                         m_vertices.at(p - 1),
                                         flipNormal);
    }
    if (rowIndex == m_rows - 1) {
        // Top most line, nothing above, must have different handling.
        // Take from one down and one right. Read till second-to-last
        rowLimit = (rowIndex + 1) * m_columns - 1;
        for (int j = rowIndex * m_columns; j < rowLimit; j++) {
            m_normals[totalIndex++] = normal(m_vertices.at(j),
                                             m_vertices.at(j - m_columns),
                                             m_vertices.at(j + 1),
                                             flipNormal);
        }

        // Top left corner. Take from one left and one down
        m_normals[totalIndex++] = normal(m_vertices.at(rowLimit),
                                         m_vertices.at(rowLimit - 1),
                                         m_vertices.at(rowLimit - m_columns),
                                         flipNormal);
    }
}

void SurfaceObject::updateSmoothItem(const QSurfaceDataArray &dataArray, int row, int column,
                                     bool polar)
{
    // Update a vertice
    getNormalizedVertex(dataArray.at(row)->at(column),
                        m_vertices[row * m_columns + column], polar, false);

    // Create normals
    int startRow = row;
    if (startRow > 0)
        startRow--; // Change the normal for previous row also
    int startCol = column;
    if (startCol > 0)
        startCol--;
    int rightCol = m_columns - 1;
    int topRow = m_rows - 1;

    const bool flipNormal = checkFlipNormal(dataArray);
    for (int i = startRow; i <= row; i++) {
        for (int j = startCol; j <= column; j++) {
            int p = i * m_columns + j;
            if (i < topRow) {
                if (j < rightCol) {
                    // One right and one up
                    m_normals[p] = normal(m_vertices.at(p),
                                          m_vertices.at(p + 1),
                                          m_vertices.at(p + m_columns),
                                          flipNormal);
                } else {
                    // Last item, nothing on the right. One up and one left
                    m_normals[p] = normal(m_vertices.at(p),
                                          m_vertices.at(p + m_columns),
                                          m_vertices.at(p - 1),
                                          flipNormal);
                }
            } else {
                // Top most line, nothing above, must have different handling.
                if (j < rightCol) {
                    // Take from one down and one right. Read till second-to-last
                    m_normals[p] = normal(m_vertices.at(p),
                                          m_vertices.at(p - m_columns),
                                          m_vertices.at(p + 1),
                                          flipNormal);
                } else {
                    // Top left corner. Take from one left and one down
                    m_normals[p] = normal(m_vertices.at(p),
                                          m_vertices.at(p - 1),
                                          m_vertices.at(p - m_columns),
                                          flipNormal);
                }
            }
        }
    }
}


void SurfaceObject::createSmoothIndices(int x, int y, int endX, int endY)
{
    if (endX >= m_columns)
        endX = m_columns - 1;
    if (endY >= m_rows)
        endY = m_rows - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    m_indexCount = 6 * (endX - x) * (endY - y);
    GLint *indices = new GLint[m_indexCount];
    int p = 0;
    int rowEnd = endY * m_columns;
    for (int row = y * m_columns; row < rowEnd; row += m_columns) {
        for (int j = x; j < endX; j++) {
            // Left triangle
            indices[p++] = row + j + 1;
            indices[p++] = row + m_columns + j;
            indices[p++] = row + j;

            // Right triangle
            indices[p++] = row + m_columns + j + 1;
            indices[p++] = row + m_columns + j;
            indices[p++] = row + j + 1;
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(GLint),
                 indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    delete[] indices;
}

void SurfaceObject::createSmoothGridlineIndices(int x, int y, int endX, int endY)
{
    if (endX >= m_columns)
        endX = m_columns - 1;
    if (endY >= m_rows)
        endY = m_rows - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int nColumns = endX - x + 1;
    int nRows = endY - y + 1;
    m_gridIndexCount = 2 * nColumns * (nRows - 1) + 2 * nRows * (nColumns - 1);
    GLint *gridIndices = new GLint[m_gridIndexCount];
    int p = 0;
    for (int i = y, row = m_columns * y; i <= endY; i++, row += m_columns) {
        for (int j = x; j < endX; j++) {
            gridIndices[p++] = row + j;
            gridIndices[p++] = row + j + 1;
        }
    }
    for (int i = y, row = m_columns * y; i < endY; i++, row += m_columns) {
        for (int j = x; j <= endX; j++) {
            gridIndices[p++] = row + j;
            gridIndices[p++] = row + j + m_columns;
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gridElementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_gridIndexCount * sizeof(GLint),
                 gridIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    delete[] gridIndices;
}

void SurfaceObject::setUpData(const QSurfaceDataArray &dataArray, const QRect &space,
                              bool changeGeometry, bool polar, bool flipXZ)
{
    m_columns = space.width();
    m_rows = space.height();
    int totalSize = m_rows * m_columns * 2;
    GLfloat uvX = 1.0f / GLfloat(m_columns - 1);
    GLfloat uvY = 1.0f / GLfloat(m_rows - 1);

    m_surfaceType = SurfaceFlat;

    // Create vertix table
    if (changeGeometry)
        m_vertices.resize(totalSize);

    QVector<QVector2D> uvs;
    if (changeGeometry)
        uvs.resize(totalSize);

    int totalIndex = 0;
    int rowLimit = m_rows - 1;
    int colLimit = m_columns - 1;
    int doubleColumns = m_columns * 2 - 2;
    int rowColLimit = rowLimit * doubleColumns;

    // Init min and max to ridiculous values
    m_minY = 10000000.0;
    m_maxY = -10000000.0f;

    for (int i = 0; i < m_rows; i++) {
        const QSurfaceDataRow &row = *dataArray.at(i);
        for (int j = 0; j < m_columns; j++) {
            getNormalizedVertex(row.at(j), m_vertices[totalIndex], polar, flipXZ);
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

    if (flipXZ) {
        for (int i = 0; i < m_vertices.size(); i++) {
            m_vertices[i].setX(-m_vertices.at(i).x());
            m_vertices[i].setZ(-m_vertices.at(i).z());
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
    const bool flipNormal = checkFlipNormal(dataArray);
    for (int row = 0, upperRow = doubleColumns;
         row < rowColLimit;
         row += doubleColumns, upperRow += doubleColumns) {
        for (int j = 0; j < doubleColumns; j += 2) {
            // Normal for the left triangle
            m_normals[totalIndex++] = normal(m_vertices.at(row + j),
                                             m_vertices.at(row + j + 1),
                                             m_vertices.at(upperRow + j),
                                             flipNormal);

            // Normal for the right triangle
            m_normals[totalIndex++] = normal(m_vertices.at(row + j + 1),
                                             m_vertices.at(upperRow + j + 1),
                                             m_vertices.at(upperRow + j),
                                             flipNormal);

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
    if (changeGeometry)
        createCoarseGridlineIndices(0, 0, colLimit, rowLimit);

    createBuffers(m_vertices, uvs, m_normals, indices, changeGeometry);

    delete[] indices;
}

void SurfaceObject::updateCoarseRow(const QSurfaceDataArray &dataArray, int rowIndex, bool polar)
{
    int colLimit = m_columns - 1;
    int doubleColumns = m_columns * 2 - 2;

    int p = rowIndex * doubleColumns;
    const QSurfaceDataRow &dataRow = *dataArray.at(rowIndex);

    for (int j = 0; j < m_columns; j++) {
        getNormalizedVertex(dataRow.at(j), m_vertices[p++], polar, false);
        if (j > 0 && j < colLimit) {
            m_vertices[p] = m_vertices[p - 1];
            p++;
        }
    }

    // Create normals
    p = rowIndex * doubleColumns;
    if (p > 0)
        p -= doubleColumns;
    int rowLimit = (rowIndex + 1) * doubleColumns;
    if (rowIndex == m_rows - 1)
        rowLimit = rowIndex * doubleColumns; //Topmost row, no normals
    const bool flipNormal = checkFlipNormal(dataArray);
    for (int row = p, upperRow = p + doubleColumns;
         row < rowLimit;
         row += doubleColumns, upperRow += doubleColumns) {
        for (int j = 0; j < doubleColumns; j += 2) {
            // Normal for the left triangle
            m_normals[p++] = normal(m_vertices.at(row + j),
                                    m_vertices.at(row + j + 1),
                                    m_vertices.at(upperRow + j),
                                    flipNormal);

            // Normal for the right triangle
            m_normals[p++] = normal(m_vertices.at(row + j + 1),
                                    m_vertices.at(upperRow + j + 1),
                                    m_vertices.at(upperRow + j),
                                    flipNormal);
        }
    }
}

void SurfaceObject::updateCoarseItem(const QSurfaceDataArray &dataArray, int row, int column,
                                     bool polar)
{
    int colLimit = m_columns - 1;
    int doubleColumns = m_columns * 2 - 2;

    // Update a vertice
    int p = row * doubleColumns + column * 2 - (column > 0);
    getNormalizedVertex(dataArray.at(row)->at(column), m_vertices[p++], polar, false);

    if (column > 0 && column < colLimit)
        m_vertices[p] = m_vertices[p - 1];

    // Create normals
    int startRow = row;
    if (startRow > 0)
        startRow--; // Change the normal for previous row also
    int startCol = column;
    if (startCol > 0)
        startCol--;
    if (row == m_rows - 1)
        row--;
    if (column == m_columns - 1)
        column--;

    const bool flipNormal = checkFlipNormal(dataArray);
    for (int i = startRow; i <= row; i++) {
        for (int j = startCol; j <= column; j++) {
            p = i * doubleColumns + j * 2;
            // Normal for the left triangle
            m_normals[p] = normal(m_vertices.at(p),
                                  m_vertices.at(p + 1),
                                  m_vertices.at(p + doubleColumns),
                                  flipNormal);
            p++;

            // Normal for the right triangle
            m_normals[p] = normal(m_vertices.at(p),
                                  m_vertices.at(p + doubleColumns),
                                  m_vertices.at(p + doubleColumns - 1),
                                  flipNormal);
        }
    }
}

void SurfaceObject::createCoarseIndices(int x, int y, int columns, int rows)
{
    if (columns > m_columns)
        columns = m_columns;
    if (rows > m_rows)
        rows = m_rows;
    if (x > columns)
        x = columns - 1;
    if (y > rows)
        y = rows - 1;

    int rowLimit = rows - 1;
    int doubleColumns = m_columns * 2 - 2;
    int doubleColumnsLimit = columns * 2 - 2;
    int rowColLimit = rowLimit * doubleColumns;
    m_indexCount = 6 * (columns - 1 - x) * (rowLimit - y);

    int p = 0;
    GLint *indices = new GLint[m_indexCount];
    for (int row = y * doubleColumns, upperRow = (y + 1) * doubleColumns;
         row < rowColLimit;
         row += doubleColumns, upperRow += doubleColumns) {
        for (int j = 2 * x; j < doubleColumnsLimit; j += 2) {
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(GLint),
                 indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    delete[] indices;
}

void SurfaceObject::createCoarseGridlineIndices(int x, int y, int endX, int endY)
{
    if (endX >= m_columns)
        endX = m_columns - 1;
    if (endY >= m_rows)
        endY = m_rows - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int nColumns = endX - x + 1;
    int nRows = endY - y + 1;
    int doubleEndX = endX * 2;
    int doubleColumns = m_columns * 2 - 2;
    int rowColLimit = endY * doubleColumns;

    m_gridIndexCount = 2 * nColumns * (nRows - 1) + 2 * nRows * (nColumns - 1);
    GLint *gridIndices = new GLint[m_gridIndexCount];
    int p = 0;

    for (int row = y * doubleColumns; row <= rowColLimit; row += doubleColumns) {
        for (int j = x * 2; j < doubleEndX; j += 2) {
            // Horizontal line
            gridIndices[p++] = row + j;
            gridIndices[p++] = row + j + 1;

            if (row < rowColLimit) {
                // Vertical line
                gridIndices[p++] = row + j;
                gridIndices[p++] = row + j + doubleColumns;
            }
        }
    }
    // The rightmost line separately, since there isn't double vertice
    for (int i = y * doubleColumns + doubleEndX - 1; i < rowColLimit; i += doubleColumns) {
        gridIndices[p++] = i;
        gridIndices[p++] = i  + doubleColumns;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gridElementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_gridIndexCount * sizeof(GLint),
                 gridIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    delete[] gridIndices;
}

void SurfaceObject::uploadBuffers()
{
    QVector<QVector2D> uvs; // Empty dummy
    createBuffers(m_vertices, uvs, m_normals, 0, false);
}

void SurfaceObject::createBuffers(const QVector<QVector3D> &vertices, const QVector<QVector2D> &uvs,
                                  const QVector<QVector3D> &normals, const GLint *indices,
                                  bool changeGeometry)
{
    // Move to buffers
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D),
                 &vertices.at(0), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(QVector3D),
                 &normals.at(0), GL_DYNAMIC_DRAW);

    if (changeGeometry) {
        glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(QVector2D),
                     &uvs.at(0), GL_STATIC_DRAW);

        if (indices) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(GLint),
                         indices, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_meshDataLoaded = true;
}

bool SurfaceObject::checkFlipNormal(const QSurfaceDataArray &array)
{
    const bool ascendingX = array.at(0)->at(0).x() < array.at(0)->at(array.at(0)->size() - 1).x();
    const bool ascendingZ = array.at(0)->at(0).z() < array.at(array.size() - 1)->at(0).z();
    return ascendingX != ascendingZ;
}

void SurfaceObject::getNormalizedVertex(const QSurfaceDataItem &data, QVector3D &vertex,
                                        bool polar, bool flipXZ)
{
    float normalizedX;
    float normalizedZ;
    if (polar) {
        // Slice don't use polar, so don't care about flip
        m_renderer->calculatePolarXZ(data.position(), normalizedX, normalizedZ);
    } else {
        if (flipXZ) {
            normalizedX = m_axisCacheZ.positionAt(data.x());
            normalizedZ = m_axisCacheX.positionAt(data.z());
        } else {
            normalizedX = m_axisCacheX.positionAt(data.x());
            normalizedZ = m_axisCacheZ.positionAt(data.z());
        }
    }
    float normalizedY = m_axisCacheY.positionAt(data.y());
    m_minY = qMin(normalizedY, m_minY);
    m_maxY = qMax(normalizedY, m_maxY);
    vertex.setX(normalizedX);
    vertex.setY(normalizedY);
    vertex.setZ(normalizedZ);
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

QVector3D SurfaceObject::vertexAt(int column, int row)
{
    int pos = 0;
    if (m_surfaceType == Undefined || !m_vertices.size())
        return zeroVector;

    if (m_surfaceType == SurfaceFlat)
        pos = row * (m_columns * 2 - 2) + column * 2 - (column > 0);
    else
        pos = row * m_columns + column;
    return m_vertices.at(pos);
}

void SurfaceObject::clear()
{
    m_gridIndexCount = 0;
    m_indexCount = 0;
    m_surfaceType = Undefined;
    m_vertices.clear();
    m_normals.clear();
}

QVector3D SurfaceObject::normal(const QVector3D &a, const QVector3D &b, const QVector3D &c,
                                bool flipNormal)
{
    QVector3D v1 = b - a;
    QVector3D v2 = c - a;
    QVector3D normal = QVector3D::crossProduct(v1, v2);
    if (flipNormal)
        normal *= -1;
    return normal;
}

QT_END_NAMESPACE_DATAVISUALIZATION

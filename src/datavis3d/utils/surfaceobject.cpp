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

#include "surfaceobject_p.h"
#include "abstractobjecthelper_p.h"

#include <QVector3D>
#include <QVector2D>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

SurfaceObject::SurfaceObject()
{
    m_indicesType = GL_UNSIGNED_INT;
}

SurfaceObject::~SurfaceObject()
{
}

void SurfaceObject::setUpSmoothData(QList<qreal> series, int columns, int rows, GLfloat yRange, bool changeGeometry)
{
    GLfloat width = (GLfloat(columns) - 1.0f) / 2.0f;
    GLfloat depth = (GLfloat(rows) - 1.0f) / -2.0f;
    GLfloat height = yRange / 2.0f;

    // Create vertice table
    QVector<QVector3D> vertices;
    QVector<QVector2D> uvs;
    for (int i = 0, row = 0; i < rows; i++, row += columns) {
        for (int j = 0; j < columns; j++) {
            vertices.append(QVector3D(float(j) / width - 1.0f,
                                      series.at(row + j) / height - 1.0f,
                                      float(i) / depth + 1.0f));
            uvs.append(QVector2D(1.0f / float(j), 1.0f / float(i)));
        }
    }

    // Create normals
    QVector<QVector3D> normals;
    for (int row = 0; row < (rows - 1) * columns; row += columns) {
        for (int j = 0; j < columns - 1; j++) {
            normals.append(normal(vertices.at(row + j),
                                  vertices.at(row + j + 1),
                                  vertices.at(row + columns + j)));
        }
        int p = row + columns - 1;
        normals.append(normal(vertices.at(p),
                              vertices.at(p + columns),
                              vertices.at(p - 1)));
    }
    for (int j = (rows - 1) * columns ; j < rows * columns - 1; j++) {
        normals.append(normal(vertices.at(j),
                              vertices.at(j - columns),
                              vertices.at(j + 1)));
    }
    int p = rows * columns - 1;
    normals.append(normal(vertices.at(p),
                          vertices.at(p - 1),
                          vertices.at(p - columns - 1)));

    // Create indice table
    GLint *indices = 0;
    if (changeGeometry) {
        m_indexCount = 6 * (columns - 1) * (rows - 1);
        indices = new GLint[m_indexCount];
        p = 0;
        for (int row = 0; row < (rows - 1) * columns; row += columns) {
            for (int j = 0; j < columns - 1; j++) {
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
        m_gridIndexCount = 2 * columns * (rows - 1) + 2 * rows * (columns - 1);
        gridIndices = new GLint[m_gridIndexCount];
        p = 0;
        for (int i = 0, row = 0; i < rows; i++, row += columns) {
            for (int j = 0; j < columns - 1; j++) {
                gridIndices[p++] = row + j;
                gridIndices[p++] = row + j + 1;
            }
        }
        for (int i = 0, row = 0; i < rows - 1; i++, row += columns) {
            for (int j = 0; j < columns; j++) {
                gridIndices[p++] = row + j;
                gridIndices[p++] = row + j + columns;
            }
        }
    }

    createBuffers(vertices, uvs, normals, indices, gridIndices, changeGeometry);

    if (indices)
        delete indices;
    if (gridIndices)
        delete gridIndices;
}


void SurfaceObject::setUpData(QList<qreal> series, int columns, int rows, GLfloat yRange, bool changeGeometry)
{
    GLfloat width = (GLfloat(columns) - 1.0f) / 2.0f;
    GLfloat depth = (GLfloat(rows) - 1.0f) / -2.0f;
    GLfloat height = yRange / 2.0f;

    // Create vertice table
    QVector<QVector3D> vertices;
    QVector<QVector2D> uvs;
    for (int i = 0, row = 0; i < rows; i++, row += columns) {
        for (int j = 0; j < columns; j++) {
            vertices.append(QVector3D(float(j) / width - 1.0f,
                                      series.at(row + j) / height - 1.0f,
                                      float(i) / depth + 1.0f));
            uvs.append(QVector2D(1.0f / float(j), 1.0f / float(i)));
            if (j > 0 && j < columns - 1) {
                vertices.append(vertices.last());
                uvs.append(QVector2D(1.0f / float(j), 1.0f / float(i)));
            }
        }
    }

    // Create normals & indice table
    QVector<QVector3D> normals;
    int doubleColumns = columns * 2 - 2;

    GLint *indices = 0;
    int p = 0;
    if (changeGeometry) {
        m_indexCount = 6 * (columns - 1) * (rows - 1);
        indices = new GLint[m_indexCount];
    }

    for (int row = 0, upperRow = doubleColumns;
         row < (rows - 1) * doubleColumns;
         row += doubleColumns, upperRow += doubleColumns) {
        for (int j = 0; j < doubleColumns; j += 2) {
            // Normal for the left triangle
            normals.append(normal(vertices.at(row + j),
                                  vertices.at(row + j + 1),
                                  vertices.at(upperRow + j)));

            // Normal for the right triangle
            normals.append(normal(vertices.at(row + j + 1),
                                  vertices.at(upperRow + j + 1),
                                  vertices.at(upperRow + j)));

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
    m_gridIndexCount = 2 * columns * (rows - 1) + 2 * rows * (columns - 1);
    GLint *gridIndices = new GLint[m_gridIndexCount];
    p = 0;
    int rowLimit = (rows - 1) * doubleColumns;
    for (int row = 0; row < rows * doubleColumns; row += doubleColumns) {
        for (int j = 0; j < doubleColumns; j += 2) {
            gridIndices[p++] = row + j;
            gridIndices[p++] = row + j + 1;

            if (row < rowLimit) {
                gridIndices[p++] = row + j;
                gridIndices[p++] = row + j + doubleColumns;
            }
        }
    }
    for (int i = doubleColumns - 1; i < rowLimit; i += doubleColumns) {
        gridIndices[p++] = i;
        gridIndices[p++] = i  + doubleColumns;
    }

    createBuffers(vertices, uvs, normals, indices, gridIndices, changeGeometry);

    if (indices)
        delete indices;
    if (gridIndices)
        delete gridIndices;
}


void SurfaceObject::createBuffers(const QVector<QVector3D> &vertices, const QVector<QVector2D> &uvs,
                   const QVector<QVector3D> &normals, const GLint *indices,
                   const GLint *gridIndices, bool changeGeometry)
{
    initializeOpenGLFunctions();
    if (m_meshDataLoaded) {
        // Delete old data
        glDeleteBuffers(1, &m_vertexbuffer);
        glDeleteBuffers(1, &m_normalbuffer);
        if (changeGeometry) {
            glDeleteBuffers(1, &m_uvbuffer);
            glDeleteBuffers(1, &m_elementbuffer);
            glDeleteBuffers(1, &m_gridElementbuffer);
        }
    }

    // Move to buffers
    glGenBuffers(1, &m_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D),
                 &vertices.at(0), GL_STATIC_DRAW);

    glGenBuffers(1, &m_normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(QVector3D),
                 &normals.at(0), GL_STATIC_DRAW);

    if (changeGeometry) {
        glGenBuffers(1, &m_uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(QVector2D),
                     &uvs.at(0), GL_STATIC_DRAW);

        glGenBuffers(1, &m_elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(GLint),
                     indices, GL_STATIC_DRAW);

        glGenBuffers(1, &m_gridElementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gridElementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_gridIndexCount * sizeof(GLint),
                     gridIndices, GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // We're done. Set the flag ON
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

QT_DATAVIS3D_END_NAMESPACE



// For rainy days

//    QVector3D vertices[] = {
//        QVector3D(-0.5f, 0.0f, 0.1f),
//        QVector3D(0.5f, 0.0f, 0.1f),
//        QVector3D(0.0f, 1.0f, -0.5f)
//    };

//    QVector3D normals[] = {
//        QVector3D(0.5, 0.0, 1.0),
//        QVector3D(0.5, 0.0, 1.0),
//        QVector3D(0.5, 0.0, 1.0)
//    };

//    vertices.append(QVector3D(-1.0f, 0.0f, 0.1f));
//    vertices.append(QVector3D(0.0f, 0.0f, 0.1f));
//    vertices.append(QVector3D(0.0f, 0.5f, -0.5f));

//    normals.append(QVector3D(0.5, 0.0, 1.0));
//    normals.append(QVector3D(0.5, 0.0, 1.0));
//    normals.append(QVector3D(0.5, 0.0, 1.0));

//GLushort indices[] = {0, 1, 2, 1, 3, 2};
//GLushort indices[] = {1, 3, 2};

//qDebug() << indices[p + 0] << ", " << indices[p + 1] << ", " << indices[p + 2];
//qDebug() << indices[p + 3] << ", " << indices[p + 4] << ", " << indices[p + 5];

//qDebug() << "(" << float(j) / width << ", 0.0, " << float(i) / depth * -1.0f << ")";

//normals.append(QVector3D(1,0,0));
//normals.append(QVector3D(0,1,0));
//normals.append(QVector3D(0,0,1));
//normals.append(QVector3D(1,0,1));

//normals.append(QVector3D(1,0,0));
//normals.append(QVector3D(0,1,0));
//normals.append(QVector3D(0,0,1));
//normals.append(QVector3D(1,0,1));

//normals.append(QVector3D(1,0,0));
//normals.append(QVector3D(0,1,0));
//normals.append(QVector3D(0,0,1));
//normals.append(QVector3D(1,0,1));


//qDebug() << "Left normal from (" << row + j << ", " << row + j + 1 << ", " << row + doubleColumns + j << ")";

//qDebug() << "right normal from (" << row + j +1  << ", " << row + doubleColumns + j + 1 << ", " << row + doubleColumns + j << ")";


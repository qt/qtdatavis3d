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

#include "drawer_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "abstractobjecthelper_p.h"
#include "surfaceobject_p.h"
#include "q3dcamera.h"
#include "utils_p.h"
#include "texturehelper_p.h"

#include <QtGui/QMatrix4x4>
#include <QtCore/qmath.h>

// Resources need to be explicitly initialized when building as static library
class StaticLibInitializer
{
public:
    StaticLibInitializer()
    {
        Q_INIT_RESOURCE(engine);
    }
};
StaticLibInitializer staticLibInitializer;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

// Vertex array buffer for point
const GLfloat point_data[] = {0.0f, 0.0f, 0.0f};

Drawer::Drawer(Q3DTheme *theme)
    : m_theme(theme),
      m_textureHelper(0),
      m_pointbuffer(0)
{
}

Drawer::~Drawer()
{
    delete m_textureHelper;
    if (QOpenGLContext::currentContext())
        glDeleteBuffers(1, &m_pointbuffer);
}

void Drawer::initializeOpenGL()
{
    if (!m_textureHelper) {
        initializeOpenGLFunctions();
        m_textureHelper = new TextureHelper();
    }
}

void Drawer::setTheme(Q3DTheme *theme)
{
    m_theme = theme;
    emit drawerChanged();
}

Q3DTheme *Drawer::theme() const
{
    return m_theme;
}

QFont Drawer::font() const
{
    return m_theme->font();
}

void Drawer::drawObject(ShaderHelper *shader, AbstractObjectHelper *object, GLuint textureId,
                        GLuint depthTextureId)
{
    if (textureId) {
        // Activate texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        shader->setUniformValue(shader->texture(), 0);
    }

    if (depthTextureId) {
        // Activate depth texture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTextureId);
        shader->setUniformValue(shader->shadow(), 1);
    }

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(shader->posAtt());
    glBindBuffer(GL_ARRAY_BUFFER, object->vertexBuf());
    glVertexAttribPointer(shader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 2nd attribute buffer : normals
    glEnableVertexAttribArray(shader->normalAtt());
    glBindBuffer(GL_ARRAY_BUFFER, object->normalBuf());
    glVertexAttribPointer(shader->normalAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 3rd attribute buffer : UVs
    glEnableVertexAttribArray(shader->uvAtt());
    glBindBuffer(GL_ARRAY_BUFFER, object->uvBuf());
    glVertexAttribPointer(shader->uvAtt(), 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->elementBuf());

    // Draw the triangles
    glDrawElements(GL_TRIANGLES, object->indexCount(), object->indicesType(), (void*)0);

    // Free buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(shader->uvAtt());
    glDisableVertexAttribArray(shader->normalAtt());
    glDisableVertexAttribArray(shader->posAtt());

    // Release textures
    if (depthTextureId) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    if (textureId) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Drawer::drawSurfaceGrid(ShaderHelper *shader, SurfaceObject *object)
{
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(shader->posAtt());
    glBindBuffer(GL_ARRAY_BUFFER, object->vertexBuf());
    glVertexAttribPointer(shader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->gridElementBuf());

    // Draw the lines
    glDrawElements(GL_LINES, object->gridIndexCount(), object->indicesType(), (void*)0);

    // Free buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(shader->posAtt());
}

void Drawer::drawPoint(ShaderHelper *shader)
{
    // Generate vertex buffer for point if it does not exist
    if (!m_pointbuffer) {
        glGenBuffers(1, &m_pointbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_pointbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(point_data), point_data, GL_STATIC_DRAW);
    }

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(shader->posAtt());
    glBindBuffer(GL_ARRAY_BUFFER, m_pointbuffer);
    glVertexAttribPointer(shader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Draw the point
    glDrawArrays(GL_POINTS, 0, 1);

    // Free buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(shader->posAtt());
}

void Drawer::drawLabel(const AbstractRenderItem &item, const LabelItem &labelItem,
                       const QMatrix4x4 &viewmatrix, const QMatrix4x4 &projectionmatrix,
                       const QVector3D &positionComp, const QVector3D &rotation,
                       GLfloat itemHeight, QAbstract3DGraph::SelectionFlags mode,
                       ShaderHelper *shader, ObjectHelper *object,
                       const Q3DCamera *camera, bool useDepth, bool rotateAlong,
                       LabelPosition position, Qt::AlignmentFlag alignment, bool isSlicing)
{
    // Draw label
    if (!labelItem.textureId())
        return; // No texture, skip

    QSize textureSize = labelItem.size();
    QMatrix4x4 modelMatrix;
    QMatrix4x4 MVPMatrix;
    GLfloat xPosition = 0.0f;
    GLfloat yPosition = 0.0f;
    GLfloat zPosition = positionComp.z();

    switch (position) {
    case LabelBelow: {
        yPosition = item.translation().y() - (positionComp.y() / 2.0f) + itemHeight - 0.1f;
        break;
    }
    case LabelLow: {
        yPosition = -positionComp.y();
        break;
    }
    case LabelMid: {
        yPosition = item.translation().y();
        break;
    }
    case LabelHigh: {
        yPosition = item.translation().y() + itemHeight / 2.0f;
        break;
    }
    case LabelOver: {
        yPosition = item.translation().y() - (positionComp.y() / 2.0f) + itemHeight + 0.1f;
        break;
    }
    case LabelBottom: {
        yPosition = -2.75f + positionComp.y();
        xPosition = 0.0f;
        break;
    }
    case LabelTop: {
        yPosition = 2.75f - positionComp.y();
        xPosition = 0.0f;
        break;
    }
    case LabelLeft: {
        yPosition = 0.0f;
        xPosition = -2.75f;
        break;
    }
    case LabelRight: {
        yPosition = 0.0f;
        xPosition = 2.75f;
        break;
    }
    }

    // Calculate scale factor to get uniform font size
    GLfloat scaledFontSize = 0.05f + m_theme->font().pointSizeF() / 500.0f;
    GLfloat scaleFactor = scaledFontSize / (GLfloat)textureSize.height();

    // Apply alignment
    GLfloat xAlignment = 0.0f;
    GLfloat yAlignment = 0.0f;
    GLfloat zAlignment = 0.0f;
    GLfloat sinRotY = qFabs(qSin(qDegreesToRadians(rotation.y())));
    GLfloat cosRotY = qFabs(qCos(qDegreesToRadians(rotation.y())));
    GLfloat sinRotZ = 0.0f;
    GLfloat cosRotZ = 0.0f;
    if (rotation.z()) {
        sinRotZ = qFabs(qSin(qDegreesToRadians(rotation.z())));
        cosRotZ = qFabs(qCos(qDegreesToRadians(rotation.z())));
    }
    switch (alignment) {
    case Qt::AlignLeft: {
        if (rotation.z() && rotation.z() != 180.0f && !rotation.y()) {
            xAlignment = ((-(GLfloat)textureSize.width() * scaleFactor) * cosRotZ
                          - ((GLfloat)textureSize.height() * scaleFactor) * sinRotZ) / 2.0f;
            yAlignment = (((GLfloat)textureSize.width() * scaleFactor) * sinRotZ
                          + ((GLfloat)textureSize.height() * scaleFactor) * cosRotZ) / 2.0f;
        } else {
            xAlignment = (-(GLfloat)textureSize.width() * scaleFactor) * cosRotY;
            zAlignment = ((GLfloat)textureSize.width() * scaleFactor) * sinRotY;
        }
        break;
    }
    case Qt::AlignRight: {
        if (rotation.z() && rotation.z() != 180.0f && !rotation.y()) {
            xAlignment = (((GLfloat)textureSize.width() * scaleFactor) * cosRotZ
                          + ((GLfloat)textureSize.height() * scaleFactor) * sinRotZ) / 2.0f;
            yAlignment = (((GLfloat)textureSize.width() * scaleFactor) * sinRotZ
                          + ((GLfloat)textureSize.height() * scaleFactor) * cosRotZ) / 2.0f;
        } else {
            xAlignment = ((GLfloat)textureSize.width() * scaleFactor) * cosRotY;
            zAlignment = (-(GLfloat)textureSize.width() * scaleFactor) * sinRotY;
        }
        break;
    }
    case Qt::AlignTop: {
        yAlignment = ((GLfloat)textureSize.width() * scaleFactor) * cosRotY;
        break;
    }
    case Qt::AlignBottom: {
        yAlignment = (-(GLfloat)textureSize.width() * scaleFactor) * cosRotY;
        break;
    }
    case Qt::AlignHCenter: {
        xAlignment = (-(GLfloat)textureSize.width() * scaleFactor) * cosRotZ
                - ((GLfloat)textureSize.height() * scaleFactor) * sinRotZ;
        break;
    }
    case Qt::AlignVCenter: {
        yAlignment = ((GLfloat)textureSize.width() * scaleFactor) * cosRotZ
                + ((GLfloat)textureSize.height() * scaleFactor) * sinRotZ;
        break;
    }
    default: {
        break;
    }
    }

    if (position < LabelBottom) {
        xPosition = item.translation().x();
        if (useDepth)
            zPosition = item.translation().z();
        else if (mode.testFlag(QAbstract3DGraph::SelectionColumn) && isSlicing)
            xPosition = -(item.translation().z()) + positionComp.z(); // flip first to left
    }

    // Position label
    modelMatrix.translate(xPosition + xAlignment, yPosition + yAlignment, zPosition + zAlignment);

    // Rotate
    QQuaternion rotQuatX = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, rotation.x());
    QQuaternion rotQuatY = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, rotation.y());
    QQuaternion rotQuatZ = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, rotation.z());
    QQuaternion rotQuaternion = rotQuatY * rotQuatZ * rotQuatX;
    modelMatrix.rotate(rotQuaternion);

    if (useDepth && !rotateAlong) {
        float yComp = float(qRadiansToDegrees(qTan(positionComp.y() / cameraDistance)));
        // Apply negative camera rotations to keep labels facing camera
        float camRotationX = camera->xRotation();
        float camRotationY = camera->yRotation();
        modelMatrix.rotate(-camRotationX, 0.0f, 1.0f, 0.0f);
        modelMatrix.rotate(-camRotationY - yComp, 1.0f, 0.0f, 0.0f);
    }

    // Scale label based on text size
    modelMatrix.scale(QVector3D((GLfloat)textureSize.width() * scaleFactor,
                                scaledFontSize,
                                0.0f));

    MVPMatrix = projectionmatrix * viewmatrix * modelMatrix;

    // Set shader bindings
    shader->setUniformValue(shader->MVP(), MVPMatrix);

    // Draw the object
    drawObject(shader, object, labelItem.textureId());
}

void Drawer::generateSelectionLabelTexture(AbstractRenderItem *item)
{
    LabelItem &labelItem = item->selectionLabelItem();
    generateLabelItem(labelItem, item->selectionLabel());
}

void Drawer::generateLabelItem(LabelItem &item, const QString &text, int widestLabel)
{
    initializeOpenGL();

    item.clear();

    if (!text.isEmpty()) {
        // Create labels
        // Print label into a QImage using QPainter
        QImage label = Utils::printTextToImage(m_theme->font(),
                                               text,
                                               m_theme->labelBackgroundColor(),
                                               m_theme->labelTextColor(),
                                               m_theme->isLabelBackgroundEnabled(),
                                               m_theme->isLabelBorderEnabled(),
                                               widestLabel);

        // Set label size
        item.setSize(label.size());
        // Insert text texture into label (also deletes the old texture)
        item.setTextureId(m_textureHelper->create2DTexture(label, true, true));
    }
}

QT_END_NAMESPACE_DATAVISUALIZATION

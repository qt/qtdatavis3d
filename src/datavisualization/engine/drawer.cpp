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

#include "qdatavisualizationenums.h"
#include "drawer_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "abstractobjecthelper_p.h"
#include "surfaceobject_p.h"
#include "q3dcamera.h"
#include "utils_p.h"
#include "texturehelper_p.h"
#include <QMatrix4x4>
#include <qmath.h>

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

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

Drawer::Drawer(const Theme &theme, const QFont &font, QDataVis::LabelStyle style)
    : m_theme(theme),
      m_font(font),
      m_style(style),
      m_textureHelper(0)
{
}

Drawer::~Drawer()
{
    delete m_textureHelper;
}

void Drawer::initializeOpenGL()
{
    if (!m_textureHelper) {
        initializeOpenGLFunctions();
        m_textureHelper = new TextureHelper();
    }
}

void Drawer::setTheme(const Theme &theme)
{
    m_theme = theme;
    emit drawerChanged();
}

Theme Drawer::theme() const
{
    return m_theme;
}

void Drawer::setFont(const QFont &font)
{
    m_font = font;
    emit drawerChanged();
}

QFont Drawer::font() const
{
    return m_font;
}

void Drawer::setStyle(QDataVis::LabelStyle style)
{
    m_style = style;
    emit drawerChanged();
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

    // Restore the GL state
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

void Drawer::drawLabel(const AbstractRenderItem &item, const LabelItem &labelItem,
                       const QMatrix4x4 &viewmatrix, const QMatrix4x4 &projectionmatrix,
                       const QVector3D &positionComp, const QVector3D &rotation,
                       GLfloat itemHeight, QDataVis::SelectionMode mode,
                       ShaderHelper *shader, ObjectHelper *object,
                       const Q3DCamera *camera,
                       bool useDepth, bool rotateAlong,
                       LabelPosition position, Qt::AlignmentFlag alignment)
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
        yPosition = -2.6f + positionComp.y(); // minus maximum negative height (+ some extra for axis title label)
        break;
    }
    case LabelLow: {
        yPosition = -positionComp.y();
        break;
    }
    case LabelMid: {
        // Use this for positioning with absolute item y position value
        yPosition = item.translation().y();
        break;
    }
    case LabelHigh: {
        // TODO: Fix this. Can't seem to get it right (if ok with positive-only bars, doesn't look good on +- and vice versa)
        yPosition = item.translation().y() + itemHeight / 2.0f;
        break;
    }
    case LabelOver: {
        float mod = 0.3f;
        if (itemHeight < 0)
            mod = 0.15f;
        yPosition = item.translation().y() - (positionComp.y() / 2.0f) + itemHeight + mod;
        break;
    }
    case LabelBottom: {
        yPosition = -2.95f + positionComp.y();
        xPosition = 0.0f;
        break;
    }
    case LabelTop: {
        yPosition = 2.95f - positionComp.y();
        xPosition = 0.0f;
        break;
    }
    case LabelLeft: {
        yPosition = 0.0f;
        xPosition = -2.95f;
        break;
    }
    case LabelRight: {
        yPosition = 0.0f;
        xPosition = 2.95f;
        break;
    }
    }

    // Calculate scale factor to get uniform font size
    GLfloat scaledFontSize = 0.05f + m_font.pointSizeF() / 500.0f;
    GLfloat scaleFactor = scaledFontSize / (GLfloat)textureSize.height();

    // Apply alignment
    GLfloat xAlignment = 0.0f;
    GLfloat yAlignment = 0.0f;
    GLfloat zAlignment = 0.0f;
    switch (alignment) {
    case Qt::AlignLeft: {
        xAlignment = (-(GLfloat)textureSize.width() * scaleFactor)
                * qFabs(qCos(qDegreesToRadians(rotation.y())));
        zAlignment = ((GLfloat)textureSize.width() * scaleFactor)
                * qFabs(qSin(qDegreesToRadians(rotation.y())));
        break;
    }
    case Qt::AlignRight: {
        xAlignment = ((GLfloat)textureSize.width() * scaleFactor)
                * qFabs(qCos(qDegreesToRadians(rotation.y())));
        zAlignment = (-(GLfloat)textureSize.width() * scaleFactor)
                * qFabs(qSin(qDegreesToRadians(rotation.y())));
        break;
    }
    case Qt::AlignTop: {
        yAlignment = ((GLfloat)textureSize.width() * scaleFactor)
                * qFabs(qCos(qDegreesToRadians(rotation.y())));
        if (itemHeight < 0)
            yAlignment = -yAlignment;
        break;
    }
    case Qt::AlignBottom: {
        yAlignment = (-(GLfloat)textureSize.width() * scaleFactor)
                * qFabs(qCos(qDegreesToRadians(rotation.y())));
        if (itemHeight < 0)
            yAlignment = -yAlignment;
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
        else if (QDataVis::SelectionModeSliceColumn == mode)
            xPosition = -(item.translation().z()) + positionComp.z(); // flip first to left
    }

    // Position label
    modelMatrix.translate(xPosition + xAlignment, yPosition + yAlignment, zPosition + zAlignment);

    // Rotate
    // TODO: We should convert rotations to use quaternions to avoid rotation order problems
    //QQuaternion rotQuatX = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, rotation.x());
    //QQuaternion rotQuatY = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, rotation.y());
    //QQuaternion rotQuatZ = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, rotation.z());
    //QQuaternion rotQuaternion = rotQuatX + rotQuatY + rotQuatZ;
    //modelMatrix.rotate(rotQuaternion);
    modelMatrix.rotate(rotation.y(), 0.0f, 1.0f, 0.0f);
    modelMatrix.rotate(rotation.z(), 0.0f, 0.0f, 1.0f);
    modelMatrix.rotate(rotation.x(), 1.0f, 0.0f, 0.0f);

    if (useDepth && !rotateAlong) {
        qreal yComp = qreal(qRadiansToDegrees(qTan(positionComp.y() / cameraDistance)));
        // Apply negative camera rotations to keep labels facing camera
        qreal camRotationX = camera->xRotation();
        qreal camRotationY = camera->yRotation();
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
        QImage label = Utils::printTextToImage(m_font,
                                               text,
                                               m_theme.m_textBackgroundColor,
                                               m_theme.m_textColor,
                                               m_style,
                                               m_theme.m_labelBorders,
                                               widestLabel);

        // Set label size
        item.setSize(label.size());
        // Insert text texture into label (also deletes the old texture)
        item.setTextureId(m_textureHelper->create2DTexture(label, true, true));
    }
}

QT_DATAVISUALIZATION_END_NAMESPACE

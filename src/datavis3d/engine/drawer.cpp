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

#include "qdatavis3dnamespace.h"
#include "drawer_p.h"
#include "shaderhelper_p.h"
#include "objecthelper_p.h"
#include "camerahelper_p.h"
#include "qdataitem.h"
#include "qdataitem_p.h"
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

QT_DATAVIS3D_BEGIN_NAMESPACE

Drawer::Drawer(const Theme &theme, const QFont &font, LabelTransparency transparency)
    : m_theme(theme),
      m_font(font),
      m_transparency(transparency),
      m_textureHelper(0)
{
}

Drawer::~Drawer()
{
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

void Drawer::setFont(const QFont &font)
{
    m_font = font;
    emit drawerChanged();
}

void Drawer::setTransparency(LabelTransparency transparency)
{
    m_transparency = transparency;
    emit drawerChanged();
}

void Drawer::drawObject(ShaderHelper *shader, ObjectHelper *object, GLuint textureId,
                        GLuint depthTextureId)
{
    // Store the GL state before changing
    GLint oldActiveTex[1];
    glGetIntegerv(GL_ACTIVE_TEXTURE, oldActiveTex);
    GLint oldTexId[1];
    glGetIntegerv(GL_TEXTURE_BINDING_2D, oldTexId);

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

    if (textureId || depthTextureId) {
        // 3rd attribute buffer : UVs
        glEnableVertexAttribArray(shader->uvAtt());
        glBindBuffer(GL_ARRAY_BUFFER, object->uvBuf());
        glVertexAttribPointer(shader->uvAtt(), 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->elementBuf());

    // Draw the triangles
    glDrawElements(GL_TRIANGLES, object->indexCount(), GL_UNSIGNED_SHORT, (void*)0);

    // Free buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if (textureId || depthTextureId)
        glDisableVertexAttribArray(shader->uvAtt());

    glDisableVertexAttribArray(shader->normalAtt());
    glDisableVertexAttribArray(shader->posAtt());

    // Restore the GL state
    glActiveTexture(*oldActiveTex);
    glBindTexture(GL_TEXTURE_2D, *oldTexId);
}

void Drawer::drawLabel(const QDataItem &item, const LabelItem &label,
                       const QMatrix4x4 &viewmatrix, const QMatrix4x4 &projectionmatrix,
                       const QVector3D &positionComp, const QVector3D &rotation,
                       GLfloat maxHeight, SelectionMode mode,
                       ShaderHelper *shader, ObjectHelper *object,
                       CameraHelper *camera,
                       bool useDepth, bool rotateAlong,
                       LabelPosition position, Qt::AlignmentFlag alignment)
{
    // Draw label
    LabelItem labelItem = label;
    if (!labelItem.textureId())
        return; // No texture, skip

    QSize textureSize = labelItem.size();
    QMatrix4x4 modelMatrix;
    QMatrix4x4 MVPMatrix;
    GLfloat xPosition;
    GLfloat yPosition;
    GLfloat zPosition = positionComp.z();

    switch (position) {
    case LabelBelow: {
        yPosition = -1.6f; // minus maximum negative height (+ some extra for label)
        break;
    }
    case LabelLow: {
        yPosition = -positionComp.y();
        break;
    }
    case LabelMid: {
        // Use this for positioning with absolute item y position value
        yPosition = item.d_ptr->translation().y();
        break;
    }
    case LabelHigh: {
        // TODO: Fix this. Can't seem to get it right (if ok with positive-only bars, doesn't look good on +- and vice versa)
        yPosition = item.d_ptr->translation().y() + (item.d_ptr->value() / maxHeight) / 2.0f;
        break;
    }
    case LabelOver: {
        float mod = 0.1f;
        if (item.d_ptr->value() < 0)
            mod = -0.1f;
        yPosition = item.d_ptr->translation().y() - (positionComp.y() / 2.0f - 0.2f)
                + (item.d_ptr->value() / maxHeight) + mod;
        break;
    }
    case LabelBottom: {
        yPosition = -1.95f; // TODO: Calculate from scene
        xPosition = 0.0f;
        break;
    }
    case LabelTop: {
        yPosition = 1.95f; // TODO: Calculate from scene
        xPosition = 0.0f;
        break;
    }
    case LabelLeft: {
        yPosition = 0.0f;
        xPosition = -2.5f; // TODO: Calculate from scene
        break;
    }
    case LabelRight: {
        yPosition = 0.0f;
        xPosition = 2.5f; // TODO: Calculate from scene
        break;
    }
    }

    // Calculate scale factor to get uniform font size
    GLfloat scaledFontSize = 0.05f + m_font.pointSizeF() / 500.0f;
    GLfloat scaleFactor = scaledFontSize / (GLfloat)textureSize.height();

    // Apply alignment
    GLfloat xAlignment = 0.0f;
    GLfloat zAlignment = 0.0f;
    switch (alignment) {
    case Qt::AlignLeft: {
        xAlignment = (-(GLfloat)textureSize.width() * scaleFactor)
                * qFabs(cos(qDegreesToRadians(rotation.y())));
        zAlignment = ((GLfloat)textureSize.width() * scaleFactor)
                * qFabs(sin(qDegreesToRadians(rotation.y())));
        break;
    }
    case Qt::AlignRight: {
        xAlignment = ((GLfloat)textureSize.width() * scaleFactor)
                * qFabs(cos(qDegreesToRadians(rotation.y())));
        zAlignment = (-(GLfloat)textureSize.width() * scaleFactor)
                * qFabs(sin(qDegreesToRadians(rotation.y())));
        break;
    }
    default: {
        break;
    }
    }

    if (position < LabelBottom) {
        xPosition = item.d_ptr->translation().x();
        if (useDepth)
            zPosition = item.d_ptr->translation().z();
        else if (ModeZoomColumn == mode)
            xPosition = -(item.d_ptr->translation().z()) + positionComp.z(); // flip first to left
    }

    // Position label
    modelMatrix.translate(xPosition + xAlignment, yPosition, zPosition + zAlignment);

    // Rotate
    modelMatrix.rotate(rotation.z(), 0.0f, 0.0f, 1.0f);
    modelMatrix.rotate(rotation.y(), 0.0f, 1.0f, 0.0f);
    modelMatrix.rotate(rotation.x(), 1.0f, 0.0f, 0.0f);

    if (useDepth && !rotateAlong) {
        // Apply negative camera rotations to keep labels facing camera
        QPointF camRotations = camera->getCameraRotations();
        modelMatrix.rotate(-camRotations.x(), 0.0f, 1.0f, 0.0f);
        modelMatrix.rotate(-camRotations.y(), 1.0f, 0.0f, 0.0f);
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

void Drawer::generateLabelTexture(QDataItem *item)
{
    LabelItem labelItem = item->d_ptr->label();
    generateLabelItem(&labelItem, item->d_ptr->valueStr());
    item->d_ptr->setLabel(labelItem);
}

void Drawer::generateLabelItem(LabelItem *item, const QString &text)
{
    initializeOpenGL();

    // Delete previous texture, if there is one
    GLuint labelTexture = item->textureId();
    if (labelTexture)
        glDeleteTextures(1, &labelTexture);

    // Create labels
    // Print label into a QImage using QPainter
    QImage label = Utils::printTextToImage(m_font,
                                           text,
                                           m_theme.m_textBackgroundColor,
                                           m_theme.m_textColor,
                                           m_transparency);

    // Set label size
    item->setSize(label.size());
    // Insert text texture into label
    item->setTextureId(m_textureHelper->create2DTexture(label, true, true));
}

QT_DATAVIS3D_END_NAMESPACE

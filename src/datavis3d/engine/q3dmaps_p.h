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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q3DMAPS_P_H
#define Q3DMAPS_P_H

#include "qdatavis3dglobal.h"
#include "qdatavis3namespace.h"
#include <QOpenGLFunctions>
#include <QFont>

class QOpenGLPaintDevice;

class QPoint;
class QSizeF;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class Q3DMaps;
class QDataItem;
class QDataRow;
class QDataSet;
class ShaderHelper;
class ObjectHelper;
class TextureHelper;
class Theme;
class Drawer;

class Q3DMapsPrivate : public QObject
{
public:
    enum SelectionType {
        SelectionNone = 0,
        SelectionBar,
        SelectionRow,
        SelectionColumn
    };

    enum MousePressType {
        MouseNone = 0,
        MouseOnScene,
        MouseOnOverview,
        MouseOnZoom,
        MouseRotating
    };

public:
    Q3DMapsPrivate(Q3DMaps *q);
    ~Q3DMapsPrivate();

    void loadBarMesh();
    void loadBackgroundMesh();
    void loadGridLineMesh();
    void loadLabelMesh();
    void initShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionShader();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initLabelShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionBuffer();
    void updateTextures();
    void calculateSceneScalingFactors(const QRect &areaRect);
    void calculateHeightAdjustment(const QPair<GLfloat, GLfloat> &limits);
    SelectionType isSelected(GLint bar, const QVector3D &selection);

    Q3DMaps *q_ptr;

    QOpenGLPaintDevice *m_paintDevice;
    ShaderHelper *m_barShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_backgroundShader;
    ShaderHelper *m_labelShader;
    ObjectHelper *m_barObj;
    ObjectHelper *m_backgroundObj;
    ObjectHelper *m_gridLineObj;
    ObjectHelper *m_labelObj;
    QString m_objFile;
    MousePressType m_mousePressed;
    QPoint m_mousePos;
    GLint m_zoomLevel;
    GLfloat m_zoomAdjustment;
    GLfloat m_horizontalRotation;
    GLfloat m_verticalRotation;
    QVector3D m_barThickness;
    GLfloat m_heightNormalizer;
    GLfloat m_yAdjustment;
    GLfloat m_scaleFactor;
    Theme *m_theme;
    bool m_isInitialized;
    SelectionMode m_selectionMode;
    QDataItem *m_selectedBar;
    QDataRow *m_data;
    QString m_axisLabelX;
    QString m_axisLabelZ;
    QString m_axisLabelY;
    QRect m_sceneViewPort;
    QRect m_zoomViewPort;
    bool m_zoomActivated;
    TextureHelper *m_textureHelper;
    LabelTransparency m_labelTransparency;
    GLfloat m_fontSize;
    QFont m_font;
    Drawer *m_drawer;
    bool m_xFlipped;
    bool m_zFlipped;
    QSizeF m_areaSize;
    GLuint m_bgrTexture;
    GLuint m_selectionTexture;
    //GLuint m_selectionDepthTexture;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    bool m_updateLabels;
    bool m_gridEnabled;
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif

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

#ifndef MAPS3DCONTROLLER_P_H
#define MAPS3DCONTROLLER_P_H

#include "qdatavis3dglobal.h"
#include "qdatavis3namespace.h"
#include "q3dmaps.h"

#include <QOpenGLFunctions>
#include <QFont>

//#define DISPLAY_RENDER_SPEED

class QPoint;
class QSizeF;

QT_DATAVIS3D_BEGIN_NAMESPACE

class QDataItem;
class QDataRow;
class ShaderHelper;
class ObjectHelper;
class TextureHelper;
class Theme;
class Drawer;
class Maps3DRenderer;
class CameraHelper;

class QT_DATAVIS3D_EXPORT Maps3DController : public QObject, public QOpenGLFunctions
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
        MouseRotating,
        MouseOnPinch
    };

public:
    Maps3DController(const QRect &rect);
    ~Maps3DController();

    void initializeOpenGL();
    void render(const GLuint defaultFboHandle = 0);

    // Add data item. New data item is appended to old data.
    // ownership of data is transferred
    bool addDataItem(QDataItem *dataItem);

    // Add data set. New data is appended to old data.
    // ownership of data is transferred
    bool addData(const QVector<QDataItem *> &data);
    // ownership of data is transferred
    bool addData(const QDataRow &data);

    // Add data set. Old data is deleted.
    // ownership of data is transferred
    bool setData(const QVector<QDataItem *> &data);
    // ownership of data is transferred
    bool setData(QDataRow *data);

    // bar specifications; base thickness in x, y and z, enum to indicate which direction is increased with value
    // TODO: Start using thickness also in adjustment direction; use it as a relative value.
    // For example, in AdjustAll mode setting thickness to (0.1f, 1.0f, 0.5f) would apply value to
    // x at 10%, y at 100% and z at 50%. If a dimension is not included, given thickness states its absolute value.
    void setBarSpecs(const QVector3D &thickness = QVector3D(1.0f, 1.0f, 1.0f),
                     Q3DMaps::AdjustmentDirection direction = Q3DMaps::AdjustHeight);

    // bar type; bars (=cubes), pyramids, cones, cylinders, balls, etc.
    void setBarType(BarStyle style, bool smooth = false);

    // override bar type with own mesh
    void setMeshFileName(const QString &objFileName);

    // Select preset camera placement
    void setCameraPreset(CameraPreset preset);

    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) angles and distance in percentage (10...500))
    void setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance = 100);

    // Set theme (bar colors, shaders, window color, background colors, light intensity and text colors are affected)
    void setTheme(ColorTheme theme);

    // Set color if you don't want to use themes. Set uniform to false if you want the (height) color to change from bottom to top
    void setBarColor(QColor baseColor, QColor heightColor, bool uniform = true);

    // Set area specs
    void setAreaSpecs(const QRect &areaRect, const QImage &image);

    // Set area image
    void setImage(const QImage &image);

    // TODO: light placement API

    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode();

    // Font size adjustment
    void setFontSize(float fontsize);
    float fontSize();

    // Set font
    void setFont(const QFont &font);
    QFont font();

    // Label transparency adjustment
    void setLabelTransparency(LabelTransparency transparency);
    LabelTransparency labelTransparency();

    // Adjust shadow quality
    void setShadowQuality(ShadowQuality quality);
    ShadowQuality shadowQuality();

    // Size
    const QSize size();
    const QRect boundingRect();
    void setBoundingRect(const QRect boundingRect);
    void setWidth(const int width);
    int width();
    void setHeight(const int height);
    int height();
    void setX(const int x);
    int x();
    void setY(const int y);
    int y();

#if defined(Q_OS_ANDROID)
    void mouseDoubleClickEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);
#endif
    void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
    void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
    void wheelEvent(QWheelEvent *event);
    void resizeNotify();

    void loadBarMesh();
    void loadBackgroundMesh();
    void loadGridLineMesh();
    void loadLabelMesh();
    void initShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionShader();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initLabelShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionBuffer();
#if !defined(QT_OPENGL_ES_2)
    void initDepthShader();
    void initDepthBuffer();
#endif
    void updateTextures();
    void calculateSceneScalingFactors(const QRect &areaRect);
    void calculateHeightAdjustment(const QPair<GLfloat, GLfloat> &limits);
    void calculateTranslation(QDataItem *item);
    SelectionType isSelected(GLint bar, const QVector3D &selection);
    bool isValid(const QDataItem &item);

private:
    void drawScene(const GLuint defaultFboHandle);

    Maps3DRenderer *m_renderer;
    CameraHelper *m_camera;

    ShaderHelper *m_barShader;
    ShaderHelper *m_depthShader;
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
    QFont m_font;
    Drawer *m_drawer;
    QSizeF m_areaSize;
    GLuint m_bgrTexture;
    GLuint m_depthTexture;
    GLuint m_selectionTexture;
    GLuint m_depthFrameBuffer;
    GLuint m_selectionFrameBuffer;
    GLuint m_selectionDepthBuffer;
    bool m_updateLabels;
    Q3DMaps::AdjustmentDirection m_adjustDirection;
    ShadowQuality m_shadowQuality;
    GLfloat m_shadowQualityToShader;
    bool m_bgrHasAlpha;
    QRect m_boundingRect;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

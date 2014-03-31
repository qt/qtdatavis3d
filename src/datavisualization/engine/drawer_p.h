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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DRAWER_P_H
#define DRAWER_P_H

#include "datavisualizationglobal_p.h"
#include "q3dbars.h"
#include "q3dtheme.h"
#include "labelitem_p.h"
#include "abstractrenderitem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class ShaderHelper;
class ObjectHelper;
class AbstractObjectHelper;
class SurfaceObject;
class TextureHelper;
class Q3DCamera;
class Abstract3DRenderer;

class Drawer : public QObject, public QOpenGLFunctions
{
    Q_OBJECT

public:
    enum LabelPosition {
        LabelBelow = 0,
        LabelLow,
        LabelMid,
        LabelHigh,
        LabelOver,
        LabelBottom,    // Absolute positions from here onward, used for axes (QDataItem is ignored)
        LabelTop,
        LabelLeft,
        LabelRight
    };

public:
    explicit Drawer(Q3DTheme *theme);
    ~Drawer();

    void initializeOpenGL();

    void setTheme(Q3DTheme *theme);
    Q3DTheme *theme() const;
    QFont font() const;

    void drawObject(ShaderHelper *shader, AbstractObjectHelper *object, GLuint textureId = 0,
                    GLuint depthTextureId = 0);
    void drawSelectionObject(ShaderHelper *shader, AbstractObjectHelper *object);
    void drawSurfaceGrid(ShaderHelper *shader, SurfaceObject *object);
    void drawPoint(ShaderHelper *shader);
    void drawLine(ShaderHelper *shader);
    void drawLabel(const AbstractRenderItem &item, const LabelItem &labelItem,
                   const QMatrix4x4 &viewmatrix, const QMatrix4x4 &projectionmatrix,
                   const QVector3D &positionComp, const QVector3D &rotation, GLfloat itemHeight,
                   QAbstract3DGraph::SelectionFlags mode, ShaderHelper *shader, ObjectHelper *object,
                   const Q3DCamera *camera, bool useDepth = false, bool rotateAlong = false,
                   LabelPosition position = LabelOver,
                   Qt::AlignmentFlag alignment = Qt::AlignCenter, bool isSlicing = false,
                   bool isSelecting = false);

    void generateSelectionLabelTexture(Abstract3DRenderer *item);
    void generateLabelItem(LabelItem &item, const QString &text, int widestLabel = 0);

Q_SIGNALS:
    void drawerChanged();

private:
    Q3DTheme *m_theme;
    TextureHelper *m_textureHelper;
    GLuint m_pointbuffer;
    GLuint m_linebuffer;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

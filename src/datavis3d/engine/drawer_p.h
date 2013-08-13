/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DRAWER_P_H
#define DRAWER_P_H

#include "datavis3dglobal_p.h"
#include "q3dbars.h"
#include "theme_p.h"
#include "labelitem_p.h"
#include "abstractrenderitem_p.h"
#include <QFont>

QT_DATAVIS3D_BEGIN_NAMESPACE

class ShaderHelper;
class ObjectHelper;
class AbstractObjectHelper;
class SurfaceObject;
class TextureHelper;
class CameraHelper;

class Drawer : public QObject, public QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Drawer(const Theme &theme, const QFont &font, LabelTransparency transparency);
    ~Drawer();

    void initializeOpenGL();

    void setTheme(const Theme &theme);
    void setFont(const QFont &font);
    void setTransparency(LabelTransparency transparency);

    void drawObject(ShaderHelper *shader, AbstractObjectHelper *object, GLuint textureId = 0,
                    GLuint depthTextureId = 0);
    void drawSurfaceGrid(ShaderHelper *shader, SurfaceObject *object);
    void drawLabel(const AbstractRenderItem &item, const LabelItem &labelItem,
                   const QMatrix4x4 &viewmatrix, const QMatrix4x4 &projectionmatrix,
                   const QVector3D &positionComp, const QVector3D &rotation, GLfloat itemHeight,
                   SelectionMode mode, ShaderHelper *shader, ObjectHelper *object,
                   CameraHelper *camera,
                   bool useDepth = false, bool rotateAlong = false,
                   LabelPosition position = LabelOver,
                   Qt::AlignmentFlag alignment = Qt::AlignCenter);

    void generateLabelTexture(AbstractRenderItem *item);
    void generateLabelItem(LabelItem &item, const QString &text);

Q_SIGNALS:
    void drawerChanged();

private:
    Theme m_theme;
    QFont m_font;
    LabelTransparency m_transparency;
    TextureHelper *m_textureHelper;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

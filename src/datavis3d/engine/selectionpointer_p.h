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

#ifndef NOTIFICATIONLABEL_P_H
#define NOTIFICATIONLABEL_P_H

#include <QtCore/QObject>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QFont>
#include <QWindow>
#include <QVector3D>

#include "datavis3dglobal_p.h"
#include "surface3dcontroller_p.h"

class QOpenGLShaderProgram;

QT_DATAVIS3D_BEGIN_NAMESPACE

class ShaderHelper;
class ObjectHelper;
class SurfaceObject;
class TextureHelper;
class Theme;
class Drawer;
class CameraHelper;

class QT_DATAVIS3D_EXPORT SelectionPointer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit SelectionPointer(Surface3DController *controller);
    ~SelectionPointer();

    void initializeOpenGL();
    void render(CameraHelper *camera, const GLuint defaultFboHandle = 0);
    void setPosition(QVector3D position);
    void setScaling(QVector3D scaling);
    void setLabel(QString label);
    void updateTheme(Theme theme);
    void updateBoundingRect(QRect rect);

private:
    void initShaders();
    void loadLabelMesh();
    void loadPointMesh();

private:
    Surface3DController *m_controller;
    ShaderHelper *m_labelShader;
    ShaderHelper *m_pointShader;
    ObjectHelper *m_labelObj;
    ObjectHelper *m_pointObj;
    TextureHelper *m_textureHelper;
    bool m_isInitialized;
    QFont m_font;
    Theme m_cachedTheme;
    QDataVis::LabelTransparency m_labelTransparency;
    LabelItem m_labelItem;
    Drawer *m_drawer;
    QRect m_mainViewPort;
    QVector3D m_position;
    QVector3D m_scale;
};

QT_DATAVIS3D_END_NAMESPACE

#endif // NOTIFICATIONLABEL_P_H

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

#ifndef DRAWER_P_H
#define DRAWER_P_H

#include "QtDataVis3D/qdatavis3dglobal.h"
#include "QtDataVis3D/qdatavis3namespace.h"
#include "q3dbars.h"
#include "theme_p.h"
#include "labelitem_p.h"
#include <QFont>

QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE

class QDataItem;
class ShaderHelper;
class ObjectHelper;
class TextureHelper;

class Drawer : public QObject
{
    Q_OBJECT

public:
    explicit Drawer(const Theme &theme, const QFont &font, LabelTransparency transparency);
    ~Drawer();

    void initializeOpenGL();

    void setTheme(const Theme &theme);
    void setFont(const QFont &font);
    void setTransparency(LabelTransparency transparency);

    void drawObject(ShaderHelper *shader, ObjectHelper *object, GLuint textureId = 0,
                    GLuint depthTextureId = 0);
    void drawLabel(const QDataItem &item, const LabelItem &label,
                   const QMatrix4x4 &viewmatrix, const QMatrix4x4 &projectionmatrix,
                   const QVector3D &positionComp, const QVector3D &rotation, GLfloat maxHeight,
                   SelectionMode mode, ShaderHelper *shader, ObjectHelper *object,
                   bool useDepth = false, bool rotateAlong = false,
                   LabelPosition position = LabelOver,
                   Qt::AlignmentFlag alignment = Qt::AlignCenter);

    void generateLabelTexture(QDataItem *item);
    void generateLabelItem(LabelItem *item, const QString &text);

Q_SIGNALS:
    void drawerChanged();

private:
    Theme m_theme;
    QFont m_font;
    LabelTransparency m_transparency;
    TextureHelper *m_textureHelper;
};

QTENTERPRISE_DATAVIS3D_END_NAMESPACE

#endif

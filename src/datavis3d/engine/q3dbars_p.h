/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef Q3DBARS_p_H
#define Q3DBARS_p_H

#include "qdatavis3dglobal.h"
#include <QOpenGLFunctions>

class QOpenGLPaintDevice;
class QPoint;
class QPointF;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class Q3DBars;
class QDataItem;
class QDataSet;
class ShaderHelper;
class ObjectHelper;

class Q3DBarsPrivate
{
public:
    enum SelectionType {
        None = 0,
        Bar,
        Row,
        Column
    };

public:
    Q3DBarsPrivate(Q3DBars *q);
    ~Q3DBarsPrivate();

    void findHighestValue(const QVector<QDataItem*> &row); // TODO: Move to QDataSet class when it's done
    void resizeDataSet(); // TODO: Move to QDataSet class when it's done
    void resizeDataRow(QVector<QDataItem*> *row); // TODO: Move to QDataSet class when it's done
    void clearDataSet(); // TODO: Move to QDataSet class when it's done
    void loadBarMesh();
    void loadBackgroundMesh();
    void initShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionShader();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionBuffer();
    void calculateSceneScalingFactors();
    SelectionType isSelected(int row, int bar, const QVector3D &selection);

    GLuint m_framebufferSelection;

    Q3DBars *q_ptr;

    QOpenGLPaintDevice *m_paintDevice;
    ShaderHelper *m_barShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_backgroundShader;
    ObjectHelper *m_barObj;
    ObjectHelper *m_backgroundObj;
    QPoint m_sampleCount;
    QString m_objFile;
    bool m_mousePressed;
    QPoint m_mousePos;
    int m_zoomLevel;
    float m_horizontalRotation;
    float m_verticalRotation;
    QPointF m_barThickness;
    QPointF m_barSpacing;
    QVector< QVector<QDataItem*> > m_dataSet;
    float m_heightNormalizer;
    float m_rowWidth;
    float m_columnDepth;
    float m_maxDimension;
    float m_scaleX;
    float m_scaleZ;
    float m_scaleFactorX;
    float m_scaleFactorZ;
    float m_sceneScale;
    float m_maxSceneSize;
    // TODO: move to themes/theme class..
    QColor m_baseColor;
    QColor m_heightColor;
    QColor m_depthColor;
    QColor m_backgroundColor;
    QColor m_windowColor;
    QColor m_textColor;
    QColor m_textBackgroundColor;
    QColor m_highlightBarColor;
    QColor m_highlightRowColor;
    QColor m_highlightColumnColor;
    float m_lightStrength;
    float m_ambientStrength;
    float m_highlightLightStrength;
    bool m_uniformColor;
    // TODO: Perhaps different shaders are required for different themes
    // ..
    bool m_isInitialized;
    Q3DBars::SelectionMode m_selectionMode;
    QDataItem *m_selectedBar;
    QDataSet *m_dataSetTest;
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif

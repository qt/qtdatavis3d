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
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef DECLARATIVEMAPS_P_H
#define DECLARATIVEMAPS_P_H

#include "datavis3dglobal_p.h"
#include "maps3dcontroller_p.h"
#include "qitemmodelmapdatamapping.h"
#include <QString>
#include <QAbstractItemModel>

QT_DATAVIS3D_BEGIN_NAMESPACE

class QDataRow;
class QDataItem;

class DeclarativeMapsCachedStatePrivate
{
public:
    explicit DeclarativeMapsCachedStatePrivate();
    ~DeclarativeMapsCachedStatePrivate();

    void appendData(QDataItem *item);

    QAbstractItemModel *m_model;
    QItemModelMapDataMapping *m_mapping;

    bool m_isImageSet;
    QImage m_image;

    bool m_isBarSpecsSet;
    QVector3D m_thickness;
    Q3DMaps::AdjustmentDirection m_direction;

    bool m_isAreaRectSet;
    QRect m_arearect;

    bool m_isSelectionModeSet;
    SelectionMode m_selectionMode;

    bool m_isLabelTransparencySet;
    LabelTransparency m_labelTransparency;

    bool m_isShadowQualitySet;
    ShadowQuality m_shadowQuality;

    float m_fontsize;

    bool m_isBarStyleSet;
    BarStyle m_barstyle;
    bool m_barsmooth;

    bool m_isMeshSet;
    QString m_meshfilename;

    bool m_isCameraPresetSet;
    CameraPreset m_camerapreset;

    float m_camhorizontal;
    float m_camvertical;
    float m_camdistance;

    bool m_isThemeSet;
    ColorTheme m_theme;

    bool m_isColorSet;
    QColor m_basecolor;
    QColor m_heightcolor;
    bool m_coloruniform;

    bool m_isFontSet;
    QFont m_font;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

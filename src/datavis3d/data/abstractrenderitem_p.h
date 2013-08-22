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

#ifndef ABSTRACTRENDERITEM_P_H
#define ABSTRACTRENDERITEM_P_H

#include "datavis3dglobal_p.h"
#include "labelitem_p.h"

#include <QOpenGLFunctions>
#include <QString>
#include <QVector3D>

QT_DATAVIS3D_BEGIN_NAMESPACE

class AbstractRenderItem
{
public:
    AbstractRenderItem();
    AbstractRenderItem(const AbstractRenderItem &other);
    virtual ~AbstractRenderItem();

    // Position in 3D scene
    inline void setTranslation(const QVector3D &translation) { m_translation = translation; }
    inline const QVector3D &translation() const {return m_translation; }

    // Selection label item (containing special selection texture, if mode is activated)
    LabelItem &selectionLabelItem();

    // Formatted selection label for item.
    void setSelectionLabel(const QString &label);
    QString &selectionLabel(); // Formats selection label if not previously formatted

protected:
    QString m_selectionLabel;
    QVector3D m_translation;
    LabelItem *m_selectionLabelItem;

    friend class QAbstractDataItem;
};

QT_DATAVIS3D_END_NAMESPACE

#endif

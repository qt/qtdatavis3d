/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
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

#ifndef QCUSTOMLABELITEM_P_H
#define QCUSTOMLABELITEM_P_H

#include "qcustom3dlabel.h"
#include "qcustom3ditem_p.h"

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class QCustom3DLabelPrivate : public QCustom3DItemPrivate
{
    Q_OBJECT

public:
    QCustom3DLabelPrivate(QCustom3DLabel *q);
    QCustom3DLabelPrivate(QCustom3DLabel *q, const QString &text, const QFont &font,
                          const QVector3D &position, const QVector3D &scaling,
                          const QQuaternion &rotation);
    virtual ~QCustom3DLabelPrivate();

    void resetDirtyBits();
    void createTextureImage();
    void createTextureImage(const QColor &bgrColor, const QColor &txtColor, bool background,
                            bool borders);
    void handleTextureChange();

public:
    QString m_text;
    QFont m_font;
    QColor m_bgrColor;
    QColor m_txtColor;
    bool m_background;
    bool m_borders;
    bool m_facingCamera;

    bool m_customVisuals;

    bool m_facingCameraDirty;

private:
    friend class QCustom3DLabel;
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

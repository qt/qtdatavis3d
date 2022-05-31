/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
******************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef ENUMTOSTRINGMAP_P_H
#define ENUMTOSTRINGMAP_P_H

//#define VERBOSE_STATE_STORE

#ifdef VERBOSE_STATE_STORE

#include <QtGui/qopengl.h>
#include <QMap>
#include <QString>

class EnumToStringMap
{
public:
    static EnumToStringMap *newInstance();
    static void deleteInstance();
    QString lookUp(const GLuint value) const;

private:
    EnumToStringMap();
    QMap<int, QString> m_map;
    QString m_unknown;
};
#endif
#endif

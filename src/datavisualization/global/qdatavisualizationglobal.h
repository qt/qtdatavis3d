/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDATAVISUALIZATIONGLOBAL_H
#define QDATAVISUALIZATIONGLOBAL_H

#include <QtCore/qglobal.h>

#define QT_DATAVISUALIZATION_VERSION_STR QT_VERSION_STR
/*
   QT_DATAVISUALIZATION_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_DATAVISUALIZATION_VERSION QT_VERSION
/*
   can be used like #if (QT_DATAVISUALIZATION_VERSION >= QT_DATAVISUALIZATION_VERSION_CHECK(1, 0, 0))
*/
#define QT_DATAVISUALIZATION_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#if defined(QT_DATAVISUALIZATION_LIBRARY)
#  define QT_DATAVISUALIZATION_EXPORT Q_DECL_EXPORT
#else
#  define QT_DATAVISUALIZATION_EXPORT Q_DECL_IMPORT
#endif

#if defined(BUILD_PRIVATE_UNIT_TESTS) && defined(QT_DATAVISUALIZATION_LIBRARY)
#  define QT_DATAVISUALIZATION_AUTOTEST_EXPORT Q_DECL_EXPORT
#elif defined(BUILD_PRIVATE_UNIT_TESTS) && !defined(QT_DATAVISUALIZATION_LIBRARY)
#  define QT_DATAVISUALIZATION_AUTOTEST_EXPORT Q_DECL_IMPORT
#else
#  define QT_DATAVISUALIZATION_AUTOTEST_EXPORT
#endif

#ifdef QT_DATAVISUALIZATION_STATICLIB
#  undef QT_DATAVISUALIZATION_EXPORT
#  undef QT_DATAVISUALIZATION_AUTOTEST_EXPORT
#  define QT_DATAVISUALIZATION_EXPORT
#  define QT_DATAVISUALIZATION_AUTOTEST_EXPORT
#endif

#define QT_BEGIN_NAMESPACE_DATAVISUALIZATION namespace QtDataVisualization {
#define QT_END_NAMESPACE_DATAVISUALIZATION }

#endif

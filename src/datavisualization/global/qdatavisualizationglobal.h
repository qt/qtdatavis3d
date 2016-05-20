/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef QDATAVISUALIZATIONGLOBAL_H
#define QDATAVISUALIZATIONGLOBAL_H

#include <QtCore/qglobal.h>

#define QT_DATAVISUALIZATION_VERSION_STR   "1.2.2"
/*
   QT_DATAVISUALIZATION_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_DATAVISUALIZATION_VERSION       0x010202
/*
   can be used like #if (QT_DATAVISUALIZATION_VERSION >= QT_DATAVISUALIZATION_VERSION_CHECK(1, 0, 0))
*/
#define QT_DATAVISUALIZATION_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#ifndef QT_STATIC
#  if defined(QT_BUILD_DATAVISUALIZATION_LIB)
#    define QT_DATAVISUALIZATION_EXPORT Q_DECL_EXPORT
#  else
#    define QT_DATAVISUALIZATION_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QT_DATAVISUALIZATION_EXPORT
#endif

#if defined(BUILD_PRIVATE_UNIT_TESTS) && defined(QT_BUILD_DATAVISUALIZATION_LIB)
#  define QT_DATAVISUALIZATION_AUTOTEST_EXPORT Q_DECL_EXPORT
#elif defined(BUILD_PRIVATE_UNIT_TESTS) && !defined(QT_BUILD_DATAVISUALIZATION_LIB)
#  define QT_DATAVISUALIZATION_AUTOTEST_EXPORT Q_DECL_IMPORT
#else
#  define QT_DATAVISUALIZATION_AUTOTEST_EXPORT
#endif

#define QT_BEGIN_NAMESPACE_DATAVISUALIZATION namespace QtDataVisualization {
#define QT_END_NAMESPACE_DATAVISUALIZATION }

#endif

/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
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

#ifndef QDATAVISUALIZATIONGLOBAL_H
#define QDATAVISUALIZATIONGLOBAL_H

#include <qglobal.h>

#define QT_DATAVISUALIZATION_VERSION_STR   "0.0.1"
/*
   QT_DATAVISUALIZATION_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_DATAVISUALIZATION_VERSION       0x000001
/*
   can be used like #if (QT_DATAVISUALIZATION_VERSION >= QT_DATAVISUALIZATION_VERSION_CHECK(1, 1, 0))
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

#define QT_DATAVISUALIZATION_NAMESPACE QtDataVisualization

#ifdef QT_DATAVISUALIZATION_NAMESPACE
#  define QT_DATAVISUALIZATION_BEGIN_NAMESPACE namespace QT_DATAVISUALIZATION_NAMESPACE {
#  define QT_DATAVISUALIZATION_END_NAMESPACE }
#  define QT_DATAVISUALIZATION_USE_NAMESPACE using namespace QT_DATAVISUALIZATION_NAMESPACE;
#else
#  define QT_DATAVISUALIZATION_BEGIN_NAMESPACE
#  define QT_DATAVISUALIZATION_END_NAMESPACE
#  define QT_DATAVISUALIZATION_USE_NAMESPACE
#endif

#endif // QVIS3DGLOBAL_H

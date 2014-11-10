/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef QDATAVISUALIZATIONGLOBAL_H
#define QDATAVISUALIZATIONGLOBAL_H

#include <QtCore/qglobal.h>

#define QT_DATAVISUALIZATION_VERSION_STR   "1.2.0"
/*
   QT_DATAVISUALIZATION_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_DATAVISUALIZATION_VERSION       0x010200
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

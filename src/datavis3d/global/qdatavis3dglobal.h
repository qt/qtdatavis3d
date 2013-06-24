/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of QtDataVis3D.
**
** $QT_BEGIN_LICENSE$
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDATAVIS3DGLOBAL_H
#define QDATAVIS3DGLOBAL_H

#include <qglobal.h>

#define QT_DATAVIS3D_VERSION_STR   "0.0.1"
/*
   QT_DATAVIS3D_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_DATAVIS3D_VERSION       0x000001
/*
   can be used like #if (QT_DATAVIS3D_VERSION >= QT_DATAVIS3D_VERSION_CHECK(1, 1, 0))
*/
#define QT_DATAVIS3D_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#if defined(QT_DATAVIS3D_LIBRARY)
#  define QT_DATAVIS3D_EXPORT Q_DECL_EXPORT
#else
#  define QT_DATAVIS3D_EXPORT Q_DECL_IMPORT
#endif

#if defined(BUILD_PRIVATE_UNIT_TESTS) && defined(QT_DATAVIS3D_LIBRARY)
#  define QT_DATAVIS3D_AUTOTEST_EXPORT Q_DECL_EXPORT
#elif defined(BUILD_PRIVATE_UNIT_TESTS) && !defined(QT_DATAVIS3D_LIBRARY)
#  define QT_DATAVIS3D_AUTOTEST_EXPORT Q_DECL_IMPORT
#else
#  define QT_DATAVIS3D_AUTOTEST_EXPORT
#endif

#ifdef QT_DATAVIS3D_STATICLIB
#  undef QT_DATAVIS3D_EXPORT
#  undef QT_DATAVIS3D_AUTOTEST_EXPORT
#  define QT_DATAVIS3D_EXPORT
#  define QT_DATAVIS3D_AUTOTEST_EXPORT
#endif

#define QT_DATAVIS3D_NAMESPACE QtDataVis3D

#ifdef QT_DATAVIS3D_NAMESPACE
#  define QT_DATAVIS3D_BEGIN_NAMESPACE namespace QT_DATAVIS3D_NAMESPACE {
#  define QT_DATAVIS3D_END_NAMESPACE }
#  define QT_DATAVIS3D_USE_NAMESPACE using namespace QT_DATAVIS3D_NAMESPACE;
#else
#  define QT_DATAVIS3D_BEGIN_NAMESPACE
#  define QT_DATAVIS3D_END_NAMESPACE
#  define QT_DATAVIS3D_USE_NAMESPACE
#endif

#endif // QVIS3DGLOBAL_H

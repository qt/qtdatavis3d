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

//#define ROTATE_ZOOM_SELECTION

// Uncomment this if you want to try hardcoded QML2 -integration h4x0r version of the engine
//#define USE_QML2_VERSION

#define QTENTERPRISE_DATAVIS3D_VERSION_STR   "0.0.1"
/*
   QTENTERPRISE_DATAVIS3D_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QTENTERPRISE_DATAVIS3D_VERSION       0x000001
/*
   can be used like #if (QTENTERPRISE_DATAVIS3D_VERSION >= QTENTERPRISE_DATAVIS3D_VERSION_CHECK(1, 1, 0))
*/
#define QTENTERPRISE_DATAVIS3D_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#if defined(QTENTERPRISE_DATAVIS3D_LIBRARY)
#  define QTENTERPRISE_DATAVIS3D_EXPORT Q_DECL_EXPORT
#else
#  define QTENTERPRISE_DATAVIS3D_EXPORT Q_DECL_IMPORT
#endif

#if defined(BUILD_PRIVATE_UNIT_TESTS) && defined(QTENTERPRISE_DATAVIS3D_LIBRARY)
#  define QTENTERPRISE_DATAVIS3D_AUTOTEST_EXPORT Q_DECL_EXPORT
#elif defined(BUILD_PRIVATE_UNIT_TESTS) && !defined(QTENTERPRISE_DATAVIS3D_LIBRARY)
#  define QTENTERPRISE_DATAVIS3D_AUTOTEST_EXPORT Q_DECL_IMPORT
#else
#  define QTENTERPRISE_DATAVIS3D_AUTOTEST_EXPORT
#endif

#ifdef QTENTERPRISE_DATAVIS3D_STATICLIB
#  undef QTENTERPRISE_DATAVIS3D_EXPORT
#  undef QTENTERPRISE_DATAVIS3D_AUTOTEST_EXPORT
#  define QTENTERPRISE_DATAVIS3D_EXPORT
#  define QTENTERPRISE_DATAVIS3D_AUTOTEST_EXPORT
#endif

#define QTENTERPRISE_DATAVIS3D_NAMESPACE QtDataVis3D

#ifdef QTENTERPRISE_DATAVIS3D_NAMESPACE
#  define QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE namespace QTENTERPRISE_DATAVIS3D_NAMESPACE {
#  define QTENTERPRISE_DATAVIS3D_END_NAMESPACE }
#  define QTENTERPRISE_DATAVIS3D_USE_NAMESPACE using namespace QTENTERPRISE_DATAVIS3D_NAMESPACE;
#else
#  define QTENTERPRISE_DATAVIS3D_BEGIN_NAMESPACE
#  define QTENTERPRISE_DATAVIS3D_END_NAMESPACE
#  define QTENTERPRISE_DATAVIS3D_USE_NAMESPACE
#endif

#if defined(DEVELOPMENT_BUILD) && !defined(QT_NO_DEBUG)
#include <stdarg.h>
#include <QDebug>

#define CHART_DEBUG chartDebug(3,__LINE__,__FILE__,__FUNCTION__)

static inline QDebug chartDebug(int numargs,...)
{
    va_list valist;
    va_start(valist, numargs);
    //for( int i = 0 ; i < numargs; i++ )
    int line = va_arg(valist, int);
    char *file = va_arg(valist, char *);
    char *function = va_arg(valist, char *);
    va_end(valist);
    return qDebug() << QString().append(function).append("(").append(file).append(":%1)").arg(line);
}
#endif

#endif // QVIS3DGLOBAL_H

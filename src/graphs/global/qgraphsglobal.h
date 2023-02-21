// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef GRAPHSGLOBAL_H
#define GRAPHSGLOBAL_H

#include <QtCore/qglobal.h>
#include <QtGraphs/qtgraphsexports.h>

QT_BEGIN_NAMESPACE

#define QT_GRAPHS_VERSION_STR QT_VERSION_STR
/*
   QT_GRAPHS_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_GRAPHS_VERSION QT_VERSION
/*
   can be used like #if (QT_GRAPHS_VERSION >= QT_GRAPHS_VERSION_CHECK(1, 0, 0))
*/
#define QT_GRAPHS_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

QT_END_NAMESPACE

#endif

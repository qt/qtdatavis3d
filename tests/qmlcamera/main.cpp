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

#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;

#if !defined(QT_OPENGL_ES_2)
    // Enable antialiasing
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setSamples(8);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    viewer.setFormat(surfaceFormat);
#endif

#ifdef Q_OS_ANDROID
    viewer.addImportPath(QString::fromLatin1("assets:/qml"));
    viewer.engine()->addPluginPath(QString::fromLatin1("%1/../%2").arg(QDir::homePath(),
                                                                       QString::fromLatin1("lib")));
#else
    viewer.addImportPath(QString::fromLatin1("%1/%2").arg(QCoreApplication::applicationDirPath(),
                                                          QString::fromLatin1("qml")));
#endif
    viewer.setMainQmlFile(QStringLiteral("qml/qmlcamera/main.qml"));
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    viewer.showExpanded();

    return app.exec();
}

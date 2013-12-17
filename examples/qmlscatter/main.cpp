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
#ifdef Q_OS_ANDROID
#include <QDir>
#include <QQmlEngine>
#endif
#include <QDebug>

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
#endif
    viewer.setTitle(QStringLiteral("QML scatter example"));
    //! [0]
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    //! [0]
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    //! [1]
    viewer.showMaximized();
    //! [1]

    return app.exec();
}

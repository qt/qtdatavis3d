// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtGui/QGuiApplication>
#include <QtCore/QDir>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>
#include <QtQuick3D/qquick3d.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());
    QQuickView viewer;

    // The following are needed to make examples run without having to install the module
    // in desktop environments.
#ifdef Q_OS_WIN
    QString extraImportPath(QStringLiteral("%1/../../../../%2"));
#else
    QString extraImportPath(QStringLiteral("%1/../../../%2"));
#endif
    viewer.engine()->addImportPath(extraImportPath.arg(QGuiApplication::applicationDirPath(),
                                      QString::fromLatin1("qml")));
    //! [4]
    QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QWindow::close);
    //! [4]

    viewer.setTitle(QStringLiteral("QML scatter example"));

    //! [3]
    viewer.setSource(QUrl("qrc:/qml/qmlscatter/main.qml"));
    //! [3]

    viewer.setResizeMode(QQuickView::SizeRootObjectToView);

    //! [2]
    viewer.showMaximized();
    //! [2]

    return app.exec();
}
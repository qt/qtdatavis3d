// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "customformatter.h"

#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQml>
#include <QtQuick3D/qquick3d.h>

//! [0]
Q_DECLARE_METATYPE(CustomFormatter *)
//! [0]

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());

    //! [1]
    qmlRegisterType<CustomFormatter>("CustomFormatter", 1, 0, "CustomFormatter");
    //! [1]

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
    QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QWindow::close);

    viewer.setTitle(QStringLiteral("Axis formatter example"));

    viewer.setSource(QUrl("qrc:/qml/qmlaxisformatter/main.qml"));
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    viewer.show();

    return app.exec();
}

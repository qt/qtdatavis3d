/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
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

#include "datasource.h"
#include <QtDataVisualization/qutils.h>
#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#ifdef Q_OS_ANDROID
#include <QtCore/QDir>
#include <QtQml/QQmlEngine>
#endif
#include <QtCore/QDebug>
#include <QtQml/QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;

    // Enable antialiasing
    viewer.setFormat(QtDataVisualization::qDefaultSurfaceFormat());

#ifdef Q_OS_ANDROID
    viewer.addImportPath(QString::fromLatin1("assets:/qml"));
    viewer.engine()->addPluginPath(QString::fromLatin1("%1/../%2").arg(QDir::homePath(),
                                                                       QString::fromLatin1("lib")));
#endif
    viewer.setTitle(QStringLiteral("Oscilloscope example"));

    //! [0]
    DataSource dataSource(&viewer);
    viewer.rootContext()->setContextProperty("dataSource", &dataSource);
    //! [0]

    viewer.setSource(QUrl("qrc:/qml/qmloscilloscope/main.qml"));
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    viewer.show();

    return app.exec();
}

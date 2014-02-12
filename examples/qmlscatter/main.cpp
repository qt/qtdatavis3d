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

#include "qtquick2applicationviewer.h"
#include <QtDataVisualization/qutils.h>

#include <QtGui/QGuiApplication>
#include <QtCore/QDir>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;

    // Enable antialiasing
    //! [2]
    viewer.setFormat(QtDataVisualization::qDefaultSurfaceFormat());
    //! [2]

    // The following are needed to make examples run without having to install the module
    // in desktop environments.
#ifdef Q_OS_WIN
    viewer.addImportPath(QString::fromLatin1("%1/../../../%2").arg(QGuiApplication::applicationDirPath(),
                                                               QString::fromLatin1("qml")));
#else
    viewer.addImportPath(QString::fromLatin1("%1/../../%2").arg(QGuiApplication::applicationDirPath(),
                                                               QString::fromLatin1("qml")));
#endif
    viewer.setTitle(QStringLiteral("QML scatter example"));
    //! [0]
    viewer.setSource(QUrl("qrc:/qml/qmlscatter/main.qml"));
    //! [0]
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    //! [1]
    viewer.showMaximized();
    //! [1]

    return app.exec();
}

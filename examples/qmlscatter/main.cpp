/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
#ifdef Q_OS_ANDROID
    viewer.addImportPath(QString::fromLatin1("assets:/qml"));
    viewer.engine()->addPluginPath(QString::fromLatin1("%1/../%2").arg(QDir::homePath(),
                                                                       QString::fromLatin1("lib")));
#else
    viewer.addImportPath(QString::fromLatin1("%1/%2").arg(QCoreApplication::applicationDirPath(),
                                                          QString::fromLatin1("qml")));
#endif
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    viewer.show();

    return app.exec();
}

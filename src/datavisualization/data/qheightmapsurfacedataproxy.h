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

#ifndef QHEIGHTMAPSURFACEDATAPROXY_H
#define QHEIGHTMAPSURFACEDATAPROXY_H

#include <QtDataVisualization/qsurfacedataproxy.h>

#include <QImage>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class QHeightMapSurfaceDataProxyPrivate;

class QT_DATAVISUALIZATION_EXPORT QHeightMapSurfaceDataProxy : public QSurfaceDataProxy
{
    Q_OBJECT

    Q_PROPERTY(QImage heightMap READ heightMap WRITE setHeightMap)
    Q_PROPERTY(QString heightMapFile READ heightMapFile WRITE setHeightMapFile)

public:
    explicit QHeightMapSurfaceDataProxy(QObject *parent = 0);
    explicit QHeightMapSurfaceDataProxy(const QImage &image, QObject *parent = 0);
    virtual ~QHeightMapSurfaceDataProxy();

    void setHeightMap(const QImage &image);
    QImage heightMap() const;

    void setHeightMapFile(const QString &filename);
    QString heightMapFile() const;

protected:
    explicit QHeightMapSurfaceDataProxy(QHeightMapSurfaceDataProxyPrivate *d, QObject *parent = 0);
    QHeightMapSurfaceDataProxyPrivate *dptr();
    const QHeightMapSurfaceDataProxyPrivate *dptrc() const;

private:
    Q_DISABLE_COPY(QHeightMapSurfaceDataProxy)

    friend class Surface3DController;
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

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

#include <QtTest/QtTest>

#include <QtDataVisualization/QHeightMapSurfaceDataProxy>

using namespace QtDataVisualization;

class tst_proxy: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void construct();

    void initialProperties();
    void initializeProperties();
    void invalidProperties();

private:
    QHeightMapSurfaceDataProxy *m_proxy;
};

void tst_proxy::initTestCase()
{
}

void tst_proxy::cleanupTestCase()
{
}

void tst_proxy::init()
{
    m_proxy = new QHeightMapSurfaceDataProxy();
}

void tst_proxy::cleanup()
{
    delete m_proxy;
}

void tst_proxy::construct()
{
    QHeightMapSurfaceDataProxy *proxy = new QHeightMapSurfaceDataProxy();
    QVERIFY(proxy);
    delete proxy;
}

void tst_proxy::initialProperties()
{
    QVERIFY(m_proxy);

    QCOMPARE(m_proxy->heightMap(), QImage());
    QCOMPARE(m_proxy->heightMapFile(), QString(""));
    QCOMPARE(m_proxy->maxXValue(), 10.0f);
    QCOMPARE(m_proxy->maxZValue(), 10.0f);
    QCOMPARE(m_proxy->minXValue(), 0.0f);
    QCOMPARE(m_proxy->minZValue(), 0.0f);

    QCOMPARE(m_proxy->columnCount(), 0);
    QCOMPARE(m_proxy->rowCount(), 0);
    QVERIFY(!m_proxy->series());

    QCOMPARE(m_proxy->type(), QAbstractDataProxy::DataTypeSurface);
}

void tst_proxy::initializeProperties()
{
    QVERIFY(m_proxy);

    m_proxy->setHeightMapFile(":/customtexture.jpg");
    m_proxy->setMaxXValue(11.0f);
    m_proxy->setMaxZValue(11.0f);
    m_proxy->setMinXValue(-10.0f);
    m_proxy->setMinZValue(-10.0f);

    QCoreApplication::processEvents();

    QCOMPARE(m_proxy->heightMapFile(), QString(":/customtexture.jpg"));
    QCOMPARE(m_proxy->maxXValue(), 11.0f);
    QCOMPARE(m_proxy->maxZValue(), 11.0f);
    QCOMPARE(m_proxy->minXValue(), -10.0f);
    QCOMPARE(m_proxy->minZValue(), -10.0f);

    QCOMPARE(m_proxy->columnCount(), 24);
    QCOMPARE(m_proxy->rowCount(), 24);

    m_proxy->setHeightMapFile("");

    QCoreApplication::processEvents();

    QCOMPARE(m_proxy->columnCount(), 0);
    QCOMPARE(m_proxy->rowCount(), 0);

    m_proxy->setHeightMap(QImage(":/customtexture.jpg"));

    QCoreApplication::processEvents();

    QCOMPARE(m_proxy->columnCount(), 24);
    QCOMPARE(m_proxy->rowCount(), 24);
}

void tst_proxy::invalidProperties()
{
    m_proxy->setMaxXValue(-10.0f);
    m_proxy->setMaxZValue(-10.0f);
    QCOMPARE(m_proxy->maxXValue(), -10.0f);
    QCOMPARE(m_proxy->maxZValue(), -10.0f);
    QCOMPARE(m_proxy->minXValue(), -11.0f);
    QCOMPARE(m_proxy->minZValue(), -11.0f);

    m_proxy->setMinXValue(10.0f);
    m_proxy->setMinZValue(10.0f);
    QCOMPARE(m_proxy->maxXValue(), 11.0f);
    QCOMPARE(m_proxy->maxZValue(), 11.0f);
    QCOMPARE(m_proxy->minXValue(), 10.0f);
    QCOMPARE(m_proxy->minZValue(), 10.0f);
}

QTEST_MAIN(tst_proxy)
#include "tst_proxy.moc"

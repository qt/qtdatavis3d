/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QtDataVisualization/QScatterDataProxy>

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

private:
    QScatterDataProxy *m_proxy;
};

void tst_proxy::initTestCase()
{
}

void tst_proxy::cleanupTestCase()
{
}

void tst_proxy::init()
{
    m_proxy = new QScatterDataProxy();
}

void tst_proxy::cleanup()
{
    delete m_proxy;
}

void tst_proxy::construct()
{
    QScatterDataProxy *proxy = new QScatterDataProxy();
    QVERIFY(proxy);
    delete proxy;
}

void tst_proxy::initialProperties()
{
    QVERIFY(m_proxy);

    QCOMPARE(m_proxy->itemCount(), 0);
    QVERIFY(!m_proxy->series());

    QCOMPARE(m_proxy->type(), QAbstractDataProxy::DataTypeScatter);
}

void tst_proxy::initializeProperties()
{
    QVERIFY(m_proxy);

    QScatterDataArray data;
    data << QVector3D(0.5f, 0.5f, 0.5f) << QVector3D(-0.3f, -0.5f, -0.4f);
    m_proxy->addItems(data);

    QCOMPARE(m_proxy->itemCount(), 2);
}

QTEST_MAIN(tst_proxy)
#include "tst_proxy.moc"

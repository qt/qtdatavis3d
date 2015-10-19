/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include <QtTest/QtTest>

#include <QtDataVisualization/QBarDataProxy>

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
    QBarDataProxy *m_proxy;
};

void tst_proxy::initTestCase()
{
}

void tst_proxy::cleanupTestCase()
{
}

void tst_proxy::init()
{
    m_proxy = new QBarDataProxy();
}

void tst_proxy::cleanup()
{
    delete m_proxy;
}

void tst_proxy::construct()
{
    QBarDataProxy *proxy = new QBarDataProxy();
    QVERIFY(proxy);
    delete proxy;
}

void tst_proxy::initialProperties()
{
    QVERIFY(m_proxy);

    QCOMPARE(m_proxy->columnLabels().count(), 0);
    QCOMPARE(m_proxy->rowCount(), 0);
    QCOMPARE(m_proxy->rowLabels().count(), 0);
    QVERIFY(!m_proxy->series());

    QCOMPARE(m_proxy->type(), QAbstractDataProxy::DataTypeBar);
}

void tst_proxy::initializeProperties()
{
    QVERIFY(m_proxy);

    m_proxy->setColumnLabels(QStringList() << "1" << "2" << "3");
    QBarDataRow *data = new QBarDataRow;
    *data << 1.0f << 3.0f << 7.5f;
    m_proxy->addRow(data);
    m_proxy->setRowLabels(QStringList() << "1");

    QCOMPARE(m_proxy->columnLabels().count(), 3);
    QCOMPARE(m_proxy->rowCount(), 1);
    QCOMPARE(m_proxy->rowLabels().count(), 1);
}

QTEST_MAIN(tst_proxy)
#include "tst_proxy.moc"

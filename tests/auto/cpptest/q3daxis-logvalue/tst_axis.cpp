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

#include <QtDataVisualization/QLogValue3DAxisFormatter>

using namespace QtDataVisualization;

class tst_axis: public QObject
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
    QLogValue3DAxisFormatter *m_formatter;
};

void tst_axis::initTestCase()
{
}

void tst_axis::cleanupTestCase()
{
}

void tst_axis::init()
{
    m_formatter = new QLogValue3DAxisFormatter();
}

void tst_axis::cleanup()
{
    delete m_formatter;
}

void tst_axis::construct()
{
    QLogValue3DAxisFormatter *formatter = new QLogValue3DAxisFormatter();
    QVERIFY(formatter);
    delete formatter;
}

void tst_axis::initialProperties()
{
    QVERIFY(m_formatter);

    QCOMPARE(m_formatter->autoSubGrid(), true);
    QCOMPARE(m_formatter->base(), 10.0);
    QCOMPARE(m_formatter->showEdgeLabels(), true);
}

void tst_axis::initializeProperties()
{
    QVERIFY(m_formatter);

    m_formatter->setAutoSubGrid(false);
    m_formatter->setBase(0.1);
    m_formatter->setShowEdgeLabels(false);

    QCOMPARE(m_formatter->autoSubGrid(), false);
    QCOMPARE(m_formatter->base(), 0.1);
    QCOMPARE(m_formatter->showEdgeLabels(), false);
}

void tst_axis::invalidProperties()
{
    m_formatter->setBase(-1.0);
    QCOMPARE(m_formatter->base(), 10.0);

    m_formatter->setBase(1.0);
    QCOMPARE(m_formatter->base(), 10.0);
}

QTEST_MAIN(tst_axis)
#include "tst_axis.moc"

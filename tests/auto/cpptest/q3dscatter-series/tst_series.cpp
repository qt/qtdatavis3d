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

#include <QtDataVisualization/QScatter3DSeries>

using namespace QtDataVisualization;

class tst_series: public QObject
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
    QScatter3DSeries *m_series;
};

void tst_series::initTestCase()
{
}

void tst_series::cleanupTestCase()
{
}

void tst_series::init()
{
    m_series = new QScatter3DSeries();
}

void tst_series::cleanup()
{
    delete m_series;
}

void tst_series::construct()
{
    QScatter3DSeries *series = new QScatter3DSeries();
    QVERIFY(series);
    delete series;

    QScatterDataProxy *proxy = new QScatterDataProxy();

    series = new QScatter3DSeries(proxy);
    QVERIFY(series);
    QCOMPARE(series->dataProxy(), proxy);
    delete series;
}

void tst_series::initialProperties()
{
    QVERIFY(m_series);

    QVERIFY(m_series->dataProxy());
    QCOMPARE(m_series->itemSize(), 0.0f);
    QCOMPARE(m_series->selectedItem(), m_series->invalidSelectionIndex());

    // Common properties. The ones identical between different series are tested in QBar3DSeries tests
    QCOMPARE(m_series->itemLabelFormat(), QString("@xLabel, @yLabel, @zLabel"));
    QCOMPARE(m_series->mesh(), QAbstract3DSeries::MeshSphere);
    QCOMPARE(m_series->type(), QAbstract3DSeries::SeriesTypeScatter);
}

void tst_series::initializeProperties()
{
    QVERIFY(m_series);

    m_series->setDataProxy(new QScatterDataProxy());
    m_series->setItemSize(0.5f);
    m_series->setSelectedItem(0);

    QCOMPARE(m_series->itemSize(), 0.5f);
    QCOMPARE(m_series->selectedItem(), 0);

    // Common properties. The ones identical between different series are tested in QBar3DSeries tests
    m_series->setMesh(QAbstract3DSeries::MeshPoint);
    m_series->setMeshRotation(QQuaternion(1, 1, 10, 20));

    QCOMPARE(m_series->mesh(), QAbstract3DSeries::MeshPoint);
    QCOMPARE(m_series->meshRotation(), QQuaternion(1, 1, 10, 20));
}

QTEST_MAIN(tst_series)
#include "tst_series.moc"

/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QtDataVisualization/QItemModelSurfaceDataProxy>
#include <QtDataVisualization/Q3DSurface>

#include "cpptestutil.h"

class tst_proxy: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void dataContainingNaNFirstRow();
    void dataContainingNaNLastRow();
    void dataContainingNaNFirstLastRow();
};

void tst_proxy::initTestCase()
{
}

void tst_proxy::cleanupTestCase()
{
}

void tst_proxy::init()
{
}

void tst_proxy::cleanup()
{
}

void tst_proxy::dataContainingNaNFirstRow()
{
    if (!CpptestUtil::isOpenGLSupported())
        QSKIP("OpenGL not supported on this platform");

    const int size = 10;
    const int missingRow = 0;

    QItemModelSurfaceDataProxy *proxy  = new QItemModelSurfaceDataProxy();
    QSurface3DSeries *series = new QSurface3DSeries(proxy);
    Q3DSurface *graph = new Q3DSurface();
    graph->addSeries(series);

    // X
    QSurfaceDataArray *array = new QSurfaceDataArray();
    array->reserve(size);
    for (int i = 0; i < size; i++) {
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D((i == missingRow) ? std::numeric_limits<float>::quiet_NaN()
                                                              : static_cast<float>(i),
                                            qSin(static_cast<float>(i)), static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));

    // Y
    for (int i = 0; i < size; i++) {
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D(static_cast<float>(i),
                                            (i == missingRow) ? std::numeric_limits<float>::quiet_NaN()
                                                              : qSin(static_cast<float>(i)),
                                             static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));

    // Z
    for (int i = 0; i < size; i++) {
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D(static_cast<float>(i),
                                            qSin(static_cast<float>(i)),
                                            (i == missingRow) ? std::numeric_limits<float>::quiet_NaN()
                                                              : static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));

    delete graph;

}

void tst_proxy::dataContainingNaNLastRow()
{
    if (!CpptestUtil::isOpenGLSupported())
        QSKIP("OpenGL not supported on this platform");

    const int size = 10;
    const int missingRow = size -  1;
    QItemModelSurfaceDataProxy *proxy  = new QItemModelSurfaceDataProxy();
    QSurface3DSeries *series = new QSurface3DSeries(proxy);
    Q3DSurface *graph = new Q3DSurface();
    graph->addSeries(series);

    // X
    QSurfaceDataArray *array = new QSurfaceDataArray();
    array->reserve(size);
    for (int i = 0; i < size; i++) {
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D((i == missingRow) ? std::numeric_limits<float>::quiet_NaN()
                                                              : static_cast<float>(i),
                                            qSin(static_cast<float>(i)), static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));

    // Y
    for (int i = 0; i < size; i++) {
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D(static_cast<float>(i),
                                            (i == missingRow) ? std::numeric_limits<float>::quiet_NaN()
                                                              : qSin(static_cast<float>(i)),
                                             static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));

    // Z
    for (int i = 0; i < size; i++) {
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D(static_cast<float>(i),
                                            qSin(static_cast<float>(i)),
                                            (i == missingRow) ? std::numeric_limits<float>::quiet_NaN()
                                                              : static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));
    delete graph;
}

void tst_proxy::dataContainingNaNFirstLastRow()
{
    if (!CpptestUtil::isOpenGLSupported())
        QSKIP("OpenGL not supported on this platform");

    const int size = 10;
    const int rowFirst = 0;
    const int rowLast = size -  1;
    QItemModelSurfaceDataProxy *proxy = new QItemModelSurfaceDataProxy();
    QSurface3DSeries *series = new QSurface3DSeries(proxy);
    Q3DSurface *graph = new Q3DSurface();
    graph->addSeries(series);

    // X
    QSurfaceDataArray *array = new QSurfaceDataArray();
    array->reserve(size);
    for (int i = 0; i < size; i++) {
        bool missingRow = (i == rowFirst || i == rowLast);
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D(missingRow ? std::numeric_limits<float>::quiet_NaN()
                                                       : static_cast<float>(i),
                                            qSin(static_cast<float>(i)), static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));

    // Y
    for (int i = 0; i < size; i++) {
        bool missingRow = (i == rowFirst || i == rowLast);
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D(static_cast<float>(i),
                                            missingRow ? std::numeric_limits<float>::quiet_NaN()
                                                       : qSin(static_cast<float>(i)),
                                             static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));

    // Z
    for (int i = 0; i < size; i++) {
        bool missingRow = (i == rowFirst || i == rowLast);
        QSurfaceDataRow *row = new QSurfaceDataRow(size);
        for (int j = 0; j < size; j++) {
            (*row)[j].setPosition(QVector3D(static_cast<float>(i),
                                            qSin(static_cast<float>(i)),
                                            missingRow ? std::numeric_limits<float>::quiet_NaN()
                                                       : static_cast<float>(j)));
        }
        *array << row;
    }
    proxy->resetArray(array);
    QVERIFY(!qIsNaN(graph->axisX()->min()));
    QVERIFY(!qIsNaN(graph->axisX()->max()));
    QVERIFY(!qIsNaN(graph->axisY()->min()));
    QVERIFY(!qIsNaN(graph->axisY()->max()));
    QVERIFY(!qIsNaN(graph->axisZ()->min()));
    QVERIFY(!qIsNaN(graph->axisZ()->max()));
    delete graph;
}

QTEST_MAIN(tst_proxy)
#include "tst_proxy.moc"

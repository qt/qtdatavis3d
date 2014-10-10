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

#include <QtDataVisualization/Q3DBars>

using namespace QtDataVisualization;

class tst_bars: public QObject
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
    void updateProperties();

private:
    Q3DBars *m_graph;
};

void tst_bars::initTestCase()
{
}

void tst_bars::cleanupTestCase()
{
}

void tst_bars::init()
{
    m_graph = new Q3DBars();
}

void tst_bars::cleanup()
{
    delete m_graph;
}

void tst_bars::construct()
{
    Q3DBars *graph = new Q3DBars();
    QVERIFY(graph);
    delete graph;
}

void tst_bars::initialProperties()
{
    QVERIFY(m_graph);
    QCOMPARE(m_graph->isMultiSeriesUniform(), false);
    QCOMPARE(m_graph->barThickness(), 1.0);
    QCOMPARE(m_graph->barSpacing(), QSizeF(1.0f, 1.0f));
    QCOMPARE(m_graph->isBarSpacingRelative(), true);
    QCOMPARE(m_graph->seriesList().length(), 0);
    QVERIFY(!m_graph->selectedSeries());
    QVERIFY(!m_graph->primarySeries());
    QCOMPARE(m_graph->floorLevel(), 0.0);

    // Common properties
    QCOMPARE(m_graph->activeTheme()->type(), Q3DTheme::ThemeQt);
    QCOMPARE(m_graph->selectionMode(), QAbstract3DGraph::SelectionItem);
    QCOMPARE(m_graph->shadowQuality(), QAbstract3DGraph::ShadowQualityMedium);
    QCOMPARE(m_graph->measureFps(), false);
    QCOMPARE(m_graph->isOrthoProjection(), false);
    QCOMPARE(m_graph->selectedElement(), QAbstract3DGraph::ElementNone);
    QCOMPARE(m_graph->aspectRatio(), 2.0);
    QCOMPARE(m_graph->optimizationHints(), QAbstract3DGraph::OptimizationDefault);
    QCOMPARE(m_graph->isPolar(), false);
    QCOMPARE(m_graph->radialLabelOffset(), 1.0);
    QCOMPARE(m_graph->horizontalAspectRatio(), 0.0);
    QCOMPARE(m_graph->isReflection(), false);
    QCOMPARE(m_graph->reflectivity(), 0.5);
    QCOMPARE(m_graph->locale(), QLocale("C"));
    QCOMPARE(m_graph->queriedGraphPosition(), QVector3D(0, 0, 0));
    QCOMPARE(m_graph->margin(), -1.0);
}

void tst_bars::initializeProperties()
{
}

void tst_bars::updateProperties()
{
}

QTEST_MAIN(tst_bars)
#include "tst_bars.moc"

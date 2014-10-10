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

#include <QtDataVisualization/Q3DSurface>

using namespace QtDataVisualization;

class tst_surface: public QObject
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
    Q3DSurface *m_graph;
};

void tst_surface::initTestCase()
{
}

void tst_surface::cleanupTestCase()
{
}

void tst_surface::init()
{
    m_graph = new Q3DSurface();
}

void tst_surface::cleanup()
{
    delete m_graph;
}

void tst_surface::construct()
{
    Q3DSurface *graph = new Q3DSurface();
    QVERIFY(graph);
    delete graph;
}

void tst_surface::initialProperties()
{
    QVERIFY(m_graph);
    QCOMPARE(m_graph->seriesList().length(), 0);
    QVERIFY(!m_graph->selectedSeries());
    QCOMPARE(m_graph->flipHorizontalGrid(), false);

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

void tst_surface::initializeProperties()
{
}

void tst_surface::updateProperties()
{
}

QTEST_MAIN(tst_surface)
#include "tst_surface.moc"

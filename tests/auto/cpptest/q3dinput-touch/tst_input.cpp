/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include <QtTest/QtTest>

#include <QtDataVisualization/QTouch3DInputHandler>

using namespace QtDataVisualization;

class tst_input: public QObject
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
    QTouch3DInputHandler *m_input;
};

void tst_input::initTestCase()
{
}

void tst_input::cleanupTestCase()
{
}

void tst_input::init()
{
    m_input = new QTouch3DInputHandler();
}

void tst_input::cleanup()
{
    delete m_input;
}

void tst_input::construct()
{
    QTouch3DInputHandler *input = new QTouch3DInputHandler();
    QVERIFY(input);
    delete input;
}

void tst_input::initialProperties()
{
    QVERIFY(m_input);

    // Common (from Q3DInputHandler and QAbstract3DInputHandler)
    QCOMPARE(m_input->isRotationEnabled(), true);
    QCOMPARE(m_input->isSelectionEnabled(), true);
    QCOMPARE(m_input->isZoomAtTargetEnabled(), true);
    QCOMPARE(m_input->isZoomEnabled(), true);
    QCOMPARE(m_input->inputPosition(), QPoint(0, 0));
    QCOMPARE(m_input->inputView(), QAbstract3DInputHandler::InputViewNone);
    QVERIFY(!m_input->scene());
}

void tst_input::initializeProperties()
{
    QVERIFY(m_input);

    // Common (from Q3DInputHandler and QAbstract3DInputHandler)
    m_input->setRotationEnabled(false);
    m_input->setSelectionEnabled(false);
    m_input->setZoomAtTargetEnabled(false);
    m_input->setZoomEnabled(false);
    m_input->setInputPosition(QPoint(100, 100));
    m_input->setInputView(QAbstract3DInputHandler::InputViewOnPrimary);

    QCOMPARE(m_input->isRotationEnabled(), false);
    QCOMPARE(m_input->isSelectionEnabled(), false);
    QCOMPARE(m_input->isZoomAtTargetEnabled(), false);
    QCOMPARE(m_input->isZoomEnabled(), false);
    QCOMPARE(m_input->inputPosition(), QPoint(100, 100));
    QCOMPARE(m_input->inputView(), QAbstract3DInputHandler::InputViewOnPrimary);
}

// TODO: QTRD-3380 (mouse/touch events)

QTEST_MAIN(tst_input)
#include "tst_input.moc"

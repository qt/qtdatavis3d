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

#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/q3dcategoryaxis.h>
#include <QtDataVisualization/qitemmodelbardataproxy.h>
#include <QtDataVisualization/q3dvalueaxis.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QScreen>
#include <QTimer>
#include <QFont>
#include <QDebug>
#include <QHeaderView>

#define USE_STATIC_DATA

using namespace QtDataVisualization;

class GraphDataGenerator : public QObject
{
public:
    explicit GraphDataGenerator(Q3DBars *bargraph, QTableWidget *tableWidget);
    ~GraphDataGenerator();

    void setupModel();
    void addRow();
    void changeStyle();
    void changePresetCamera();
    void changeTheme();
    void start();
    void selectFromTable(const QPoint &selection);
    void selectedFromTable(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void fixTableSize();

private:
    Q3DBars *m_graph;
    QTimer *m_dataTimer;
    QTimer *m_styleTimer;
    QTimer *m_presetTimer;
    QTimer *m_themeTimer;
    int m_columnCount;
    int m_rowCount;
    QTableWidget *m_tableWidget; // not owned
};

GraphDataGenerator::GraphDataGenerator(Q3DBars *bargraph, QTableWidget *tableWidget)
    : m_graph(bargraph),
      m_dataTimer(0),
      m_styleTimer(0),
      m_presetTimer(0),
      m_themeTimer(0),
      m_columnCount(100),
      m_rowCount(50),
      m_tableWidget(tableWidget)
{
    // Set up bar specifications; make the bars as wide as they are deep,
    // and add a small space between the bars
    m_graph->setBarThickness(1.0);
    m_graph->setBarSpacing(QSizeF(0.2, 0.2));

    // Set bar type to flat pyramids
    m_graph->setBarType(QDataVis::MeshStylePyramids, false);

#ifndef USE_STATIC_DATA
    // Set up sample space; make it as deep as it's wide
    m_graph->setDataWindow(m_rowCount, m_columnCount);
    m_tableWidget->setColumnCount(m_columnCount);

    // Set selection mode to full
    m_graph->setSelectionMode(QDataVis::SelectionModeItemRowAndColumn);

    // Hide axis labels by explicitly setting one empty string as label list
    m_graph->rowAxis()->setCategoryLabels(QStringList(QString()));
    m_graph->columnAxis()->setCategoryLabels(QStringList(QString()));

    m_graph->activeDataProxy()->setItemLabelFormat(QStringLiteral("@valueLabel"));
#else
    // Set selection mode to zoom row
    m_graph->setSelectionMode(QDataVis::SelectionModeSliceRow);
    m_graph->setFont(QFont("Impact", 20));
#endif

    // Set theme
    m_graph->setTheme(QDataVis::ThemeDigia);

    // Set preset camera position
    m_graph->setCameraPreset(QDataVis::CameraPresetFront);
}

GraphDataGenerator::~GraphDataGenerator()
{
    if (m_dataTimer) {
        m_dataTimer->stop();
        delete m_dataTimer;
    }
    delete m_graph;
}

void GraphDataGenerator::start()
{
#ifndef USE_STATIC_DATA
    m_dataTimer = new QTimer();
    m_dataTimer->setTimerType(Qt::CoarseTimer);
    QObject::connect(m_dataTimer, &QTimer::timeout, this, &GraphDataGenerator::addRow);
    m_dataTimer->start(0);
    m_tableWidget->setFixedWidth(m_graph->width());
#else
    setupModel();
    // Table needs to be shown before the size of its headers can be accurately obtained,
    // so we postpone it a bit
    m_dataTimer = new QTimer();
    m_dataTimer->setSingleShot(true);
    QObject::connect(m_dataTimer, &QTimer::timeout, this, &GraphDataGenerator::fixTableSize);
    m_dataTimer->start(0);
#endif
}

void GraphDataGenerator::setupModel()
{
    // Set up row and column names
    QStringList days;
    days << "Monday" << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday" << "Sunday";
    QStringList weeks;
    weeks << "week 1" << "week 2" << "week 3" << "week 4" << "week 5";

    // Set up data         Mon  Tue  Wed  Thu  Fri  Sat  Sun
    float hours[5][7] = {{2.0f, 1.0f, 3.0f, 0.2f, 1.0f, 5.0f, 10.0f},     // week 1
                         {0.5f, 1.0f, 3.0f, 1.0f, 2.0f, 2.0f, 3.0f},      // week 2
                         {1.0f, 1.0f, 2.0f, 1.0f, 4.0f, 4.0f, 4.0f},      // week 3
                         {0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f, 0.3f},      // week 4
                         {3.0f, 3.0f, 6.0f, 2.0f, 2.0f, 1.0f, 1.0f}};     // week 5

    // Add labels
    m_graph->rowAxis()->setTitle("Week of year");
    m_graph->columnAxis()->setTitle("Day of week");
    m_graph->valueAxis()->setTitle("Hours playing banjo");
    m_graph->valueAxis()->setSegmentCount(5);
    m_graph->valueAxis()->setLabelFormat("%.1f h");

    m_tableWidget->setRowCount(5);
    m_tableWidget->setColumnCount(7);
    m_tableWidget->setHorizontalHeaderLabels(days);
    m_tableWidget->setVerticalHeaderLabels(weeks);
    m_tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tableWidget->setCurrentCell(-1, -1);

    for (int week = 0; week < weeks.size(); week++) {
        for (int day = 0; day < days.size(); day++) {
            QModelIndex index = m_tableWidget->model()->index(week, day);
            m_tableWidget->model()->setData(index, hours[week][day]);
        }
    }
}

void GraphDataGenerator::addRow()
{
    m_tableWidget->model()->insertRow(0);
    if (m_tableWidget->model()->rowCount() > m_rowCount)
        m_tableWidget->model()->removeRow(m_rowCount);
    for (int i = 0; i < m_columnCount; i++) {
        QModelIndex index = m_tableWidget->model()->index(0, i);
        m_tableWidget->model()->setData(index,
            ((qreal)i / (qreal)m_columnCount) / 2.0 + (qreal)(rand() % 30) / 100.0);
    }
    m_tableWidget->resizeColumnsToContents();
}

void GraphDataGenerator::selectFromTable(const QPoint &selection)
{
    m_tableWidget->setFocus();
    m_tableWidget->setCurrentCell(selection.x(), selection.y());
}

void GraphDataGenerator::selectedFromTable(int currentRow, int currentColumn,
                                           int previousRow, int previousColumn)
{
    Q_UNUSED(previousRow)
    Q_UNUSED(previousColumn)
    m_graph->setSelectedBarPos(QPoint(currentRow, currentColumn));
}

void GraphDataGenerator::fixTableSize()
{
    int width = m_tableWidget->horizontalHeader()->length();
    width += m_tableWidget->verticalHeader()->width();
    m_tableWidget->setFixedWidth(width + 2);
    int height = m_tableWidget->verticalHeader()->length();
    height += m_tableWidget->horizontalHeader()->height();
    m_tableWidget->setFixedHeight(height + 2);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget widget;
    QVBoxLayout *layout = new QVBoxLayout(&widget);

    Q3DBars *graph = new Q3DBars();
    QSize screenSize = graph->screen()->size();

    QWidget *container = QWidget::createWindowContainer(graph);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget.setWindowTitle(QStringLiteral("Hours playing banjo"));

    QTableWidget *tableWidget = new QTableWidget(&widget);
    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tableWidget->setAlternatingRowColors(true);

    layout->addWidget(container, 1);
    layout->addWidget(tableWidget, 1, Qt::AlignHCenter);

    // Since we are dealing with QTableWidget, the model will already have data sorted properly
    // in rows and columns, so create a custom mapping to utilize this.
    QItemModelBarDataMapping *mapping = new QItemModelBarDataMapping;
    mapping->setUseModelCategories(true);
    QItemModelBarDataProxy *proxy = new QItemModelBarDataProxy(tableWidget->model(), mapping);
    graph->setActiveDataProxy(proxy);

    GraphDataGenerator generator(graph, tableWidget);

    QObject::connect(graph, &Q3DBars::selectedBarPosChanged, &generator,
                     &GraphDataGenerator::selectFromTable);
    QObject::connect(tableWidget, &QTableWidget::currentCellChanged, &generator,
                     &GraphDataGenerator::selectedFromTable);

    widget.show();
    generator.start();

    return app.exec();
}

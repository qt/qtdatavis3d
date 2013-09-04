/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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

#include <QtDataVis3D/q3dbars.h>
#include <QtDataVis3D/q3dcategoryaxis.h>
#include <QtDataVis3D/qitemmodelbardataproxy.h>
#include <QtDataVis3D/q3dvalueaxis.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QScreen>
#include <QTimer>
#include <QFont>
#include <QDebug>

#define USE_STATIC_DATA

using namespace QtDataVis3D;

class ChartDataGenerator : public QObject
{
public:
    explicit ChartDataGenerator(Q3DBars *barchart, QTableWidget *tableWidget);
    ~ChartDataGenerator();

    void setupModel();
    void addRow();
    void changeStyle();
    void changePresetCamera();
    void changeTheme();
    void start();
    void selectFromTable(const QPoint &selection);
    void selectedFromTable(int row, int column);

private:
    Q3DBars *m_chart;
    QTimer *m_dataTimer;
    QTimer *m_styleTimer;
    QTimer *m_presetTimer;
    QTimer *m_themeTimer;
    int m_columnCount;
    int m_rowCount;
    QTableWidget *m_tableWidget; // not owned
};

ChartDataGenerator::ChartDataGenerator(Q3DBars *barchart, QTableWidget *tableWidget)
    : m_chart(barchart),
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
    m_chart->setBarSpecs(1.0, QSizeF(0.2, 0.2));

#ifndef USE_STATIC_DATA
    // Set up sample space; make it as deep as it's wide
    m_chart->setDataWindow(m_rowCount, m_columnCount);
    m_tableWidget->setColumnCount(m_columnCount);
#endif

    // Set bar type to flat pyramids
    m_chart->setBarType(QDataVis::Pyramids, false);

#ifndef USE_STATIC_DATA
    // Set selection mode to full
    m_chart->setSelectionMode(QDataVis::ModeItemRowAndColumn);
#else
    // Set selection mode to zoom row
    m_chart->setSelectionMode(QDataVis::ModeSliceRow);
    m_chart->setFont(QFont("Impact", 25));
#endif

    // Set theme
    m_chart->setTheme(QDataVis::ThemeLight);

    // Set preset camera position
    m_chart->setCameraPreset(QDataVis::PresetFront);
}

ChartDataGenerator::~ChartDataGenerator()
{
    if (m_dataTimer) {
        m_dataTimer->stop();
        delete m_dataTimer;
    }
    delete m_chart;
}

void ChartDataGenerator::start()
{
#ifndef USE_STATIC_DATA
    m_dataTimer = new QTimer();
    m_dataTimer->setTimerType(Qt::CoarseTimer);
    m_dataTimer->setInterval(500);
    QObject::connect(m_dataTimer, &QTimer::timeout, this, &ChartDataGenerator::addRow);
    m_dataTimer->start(500);
    m_tableWidget->setFixedWidth(m_chart->width());
#else
    setupModel();
#endif
}

void ChartDataGenerator::setupModel()
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
    m_chart->rowAxis()->setTitle("Week of year");
    m_chart->columnAxis()->setTitle("Day of week");
    m_chart->valueAxis()->setTitle("Hours playing banjo");
    m_chart->valueAxis()->setSegmentCount(5);
    m_chart->valueAxis()->setLabelFormat("%.1f h");
    m_chart->rowAxis()->setCategoryLabels(weeks);
    m_chart->columnAxis()->setCategoryLabels(days);

    m_tableWidget->setRowCount(5);
    m_tableWidget->setColumnCount(7);
    m_tableWidget->setHorizontalHeaderLabels(days);
    m_tableWidget->setVerticalHeaderLabels(weeks);

    for (int week = 0; week < weeks.size(); week++) {
        for (int day = 0; day < days.size(); day++) {
            QModelIndex index = m_tableWidget->model()->index(week, day);
            m_tableWidget->model()->setData(index, hours[week][day]);
        }
    }
    m_tableWidget->setFixedWidth(600);

    // Set up sample space based on prepared data
    m_chart->setDataWindow(weeks.size(), days.size());
}

void ChartDataGenerator::addRow()
{
    m_tableWidget->model()->insertRow(0);
    for (int i = 0; i < m_columnCount; i++) {
        QModelIndex index = m_tableWidget->model()->index(0, i);
        m_tableWidget->model()->setData(index,
            ((qreal)i / (qreal)m_columnCount) / 2.0 + (qreal)(rand() % 30) / 100.0);
    }
    m_tableWidget->resizeColumnsToContents();
}

void ChartDataGenerator::selectFromTable(const QPoint &selection)
{
    m_tableWidget->setFocus();
    m_tableWidget->setCurrentCell(selection.x(), selection.y());
}

void ChartDataGenerator::selectedFromTable(int row, int column)
{
    m_chart->setSelectedBarPos(QPoint(row, column));
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(widget);

    Q3DBars *chart = new Q3DBars();
    QSize screenSize = chart->screen()->size();

    QWidget *container = QWidget::createWindowContainer(chart);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("Hours playing banjo"));

    QTableWidget *tableWidget = new QTableWidget(widget);
    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tableWidget->setAlternatingRowColors(true);

    layout->addWidget(container, 1);
    layout->addWidget(tableWidget, 1, Qt::AlignHCenter);

    // We don't need to initialize the mapping object in any way, as it defaults
    // to row/column support and uses the Qt::DisplayRole role for value role by default.
    QItemModelBarDataProxy *proxy = new QItemModelBarDataProxy(tableWidget->model(),
                                                               new QItemModelBarDataMapping);
    chart->setActiveDataProxy(proxy);

    ChartDataGenerator *generator = new ChartDataGenerator(chart, tableWidget);

    QObject::connect(chart, &Q3DBars::selectedBarPosChanged, generator,
                     &ChartDataGenerator::selectFromTable);
    QObject::connect(tableWidget, &QTableWidget::cellClicked, generator,
                     &ChartDataGenerator::selectedFromTable);

    widget->show();
    generator->start();

    return app.exec();
}

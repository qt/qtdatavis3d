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

#include "data.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QCommandLinkButton>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Q3DScatter *graph = new Q3DScatter();
    QWidget *container = QWidget::createWindowContainer(graph);

    container->setMinimumSize(800, 600);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    QCommandLinkButton *rangeButton = new QCommandLinkButton(widget);
    rangeButton->setText(QStringLiteral("Toggle axis ranges"));
    rangeButton->setDescription(QStringLiteral("Switch between automatic axis ranges and preset ranges"));
    rangeButton->setIconSize(QSize(0, 0));

    vLayout->addWidget(rangeButton, 1, Qt::AlignTop);

    widget->setWindowTitle(QStringLiteral("Input Handling for Axes"));

    Data *graphData = new Data(graph);

    QObject::connect(rangeButton, &QCommandLinkButton::clicked, graphData, &Data::toggleRanges);

    widget->show();
    return app.exec();
}

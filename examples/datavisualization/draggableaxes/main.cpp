// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "data.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QMessageBox>

int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", "opengl");
    QApplication app(argc, argv);
    Q3DScatter *graph = new Q3DScatter();
    QWidget *container = QWidget::createWindowContainer(graph);

    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return -1;
    }

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

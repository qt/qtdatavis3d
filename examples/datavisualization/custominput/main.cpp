// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "scatterdatamodifier.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QScreen>
#include <QtGui/QFontDatabase>

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

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    widget->setWindowTitle(QStringLiteral("Custom Input Handling"));

    QPushButton *cameraButton = new QPushButton(widget);
    cameraButton->setText(QStringLiteral("Toggle Camera Animation"));

    QComboBox *shadowQuality = new QComboBox(widget);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(2);

    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")), 0, Qt::AlignTop);
    vLayout->addWidget(shadowQuality, 1, Qt::AlignTop);

    ScatterDataModifier *modifier = new ScatterDataModifier(graph);

    QObject::connect(cameraButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::toggleCameraAnimation);

    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeShadowQuality(int)));

    QObject::connect(modifier, &ScatterDataModifier::shadowQualityChanged, shadowQuality,
                     &QComboBox::setCurrentIndex);

    widget->show();
    modifier->start();
    return app.exec();
}

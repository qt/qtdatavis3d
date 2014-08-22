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

#include "volumetrictest.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtGui/QScreen>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Q3DScatter *graph = new Q3DScatter();
    QWidget *container = QWidget::createWindowContainer(graph);

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 4, screenSize.height() / 4));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    widget->setWindowTitle(QStringLiteral("Volumetric TEST"));
    widget->resize(QSize(screenSize.width() / 1.5, screenSize.height() / 1.5));

    QCheckBox *sliceXCheckBox = new QCheckBox(widget);
    sliceXCheckBox->setText(QStringLiteral("Slice volume on X axis"));
    sliceXCheckBox->setChecked(false);
    QCheckBox *sliceYCheckBox = new QCheckBox(widget);
    sliceYCheckBox->setText(QStringLiteral("Slice volume on Y axis"));
    sliceYCheckBox->setChecked(false);
    QCheckBox *sliceZCheckBox = new QCheckBox(widget);
    sliceZCheckBox->setText(QStringLiteral("Slice volume on Z axis"));
    sliceZCheckBox->setChecked(false);

    QSlider *sliceXSlider = new QSlider(Qt::Horizontal, widget);
    sliceXSlider->setMinimum(0);
    sliceXSlider->setMaximum(1024);
    sliceXSlider->setValue(512);
    sliceXSlider->setEnabled(true);
    QSlider *sliceYSlider = new QSlider(Qt::Horizontal, widget);
    sliceYSlider->setMinimum(0);
    sliceYSlider->setMaximum(1024);
    sliceYSlider->setValue(512);
    sliceYSlider->setEnabled(true);
    QSlider *sliceZSlider = new QSlider(Qt::Horizontal, widget);
    sliceZSlider->setMinimum(0);
    sliceZSlider->setMaximum(1024);
    sliceZSlider->setValue(512);
    sliceZSlider->setEnabled(true);

    QLabel *fpsLabel = new QLabel(QStringLiteral("Fps: "), widget);

    vLayout->addWidget(fpsLabel);
    vLayout->addWidget(sliceXCheckBox);
    vLayout->addWidget(sliceXSlider);
    vLayout->addWidget(sliceYCheckBox);
    vLayout->addWidget(sliceYSlider);
    vLayout->addWidget(sliceZCheckBox);
    vLayout->addWidget(sliceZSlider, 1, Qt::AlignTop);

    VolumetricModifier *modifier = new VolumetricModifier(graph);
    modifier->setFpsLabel(fpsLabel);

    QObject::connect(sliceXCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::sliceX);
    QObject::connect(sliceYCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::sliceY);
    QObject::connect(sliceZCheckBox, &QCheckBox::stateChanged, modifier,
                     &VolumetricModifier::sliceZ);
    QObject::connect(sliceXSlider, &QSlider::valueChanged, modifier,
                     &VolumetricModifier::adjustSliceX);
    QObject::connect(sliceYSlider, &QSlider::valueChanged, modifier,
                     &VolumetricModifier::adjustSliceY);
    QObject::connect(sliceZSlider, &QSlider::valueChanged, modifier,
                     &VolumetricModifier::adjustSliceZ);

    widget->show();
    return app.exec();
}

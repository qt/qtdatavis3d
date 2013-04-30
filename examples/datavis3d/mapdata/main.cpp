/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mapdata.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QFontComboBox>
#include <QLabel>
#include <QScreen>
#include <QFontDatabase>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout(widget);

    Q3DMaps *chart = new Q3DMaps();
    QSize screenSize = chart->screen()->size();

    QWidget *container = QWidget::createWindowContainer(chart);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("Blablaa"));

    hLayout->addWidget(container);
    hLayout->addLayout(vLayout);

//    QPushButton *dataButton = new QPushButton(widget);
//    dataButton->setText(QStringLiteral("Add a row of random data"));
//    dataButton->setEnabled(false);

//    QPushButton *themeButton = new QPushButton(widget);
//    themeButton->setText(QStringLiteral("Change theme"));

//    QPushButton *labelButton = new QPushButton(widget);
//    labelButton->setText(QStringLiteral("Change label style"));

//    QPushButton *styleButton = new QPushButton(widget);
//    styleButton->setText(QStringLiteral("Change bar style"));

//    QPushButton *cameraButton = new QPushButton(widget);
//    cameraButton->setText(QStringLiteral("Change camera preset"));

//    QPushButton *selectionButton = new QPushButton(widget);
//    selectionButton->setText(QStringLiteral("Change selection mode"));

//    QCheckBox *gridCheckBox = new QCheckBox(widget);
//    gridCheckBox->setText(QStringLiteral("Show grid"));
//    gridCheckBox->setChecked(true);

//    QCheckBox *rotationCheckBox = new QCheckBox(widget);
//    rotationCheckBox->setText("Rotate with slider");

//    QCheckBox *staticCheckBox = new QCheckBox(widget);
//    staticCheckBox->setText("Use dynamic data");
//    staticCheckBox->setChecked(false);

//    QSlider *rotationSliderX = new QSlider(Qt::Horizontal, widget);
//    rotationSliderX->setTickInterval(1);
//    rotationSliderX->setMinimum(-180);
//    rotationSliderX->setValue(0);
//    rotationSliderX->setMaximum(180);
//    rotationSliderX->setEnabled(false);
//    QSlider *rotationSliderY = new QSlider(Qt::Horizontal, widget);
//    rotationSliderY->setTickInterval(1);
//    rotationSliderY->setMinimum(0);
//    rotationSliderY->setValue(0);
//    rotationSliderY->setMaximum(90);
//    rotationSliderY->setEnabled(false);

//    QSlider *sizeSliderX = new QSlider(Qt::Horizontal, widget);
//    sizeSliderX->setTickInterval(1);
//    sizeSliderX->setMinimum(1);
//    sizeSliderX->setValue(100);
//    sizeSliderX->setMaximum(100);
//    QSlider *sizeSliderZ = new QSlider(Qt::Horizontal, widget);
//    sizeSliderZ->setTickInterval(1);
//    sizeSliderZ->setMinimum(1);
//    sizeSliderZ->setValue(100);
//    sizeSliderZ->setMaximum(100);

//    QSlider *spacingSliderX = new QSlider(Qt::Horizontal, widget);
//    spacingSliderX->setTickInterval(1);
//    spacingSliderX->setMinimum(0);
//    spacingSliderX->setValue(10);
//    spacingSliderX->setMaximum(200);
//    QSlider *spacingSliderZ = new QSlider(Qt::Horizontal, widget);
//    spacingSliderZ->setTickInterval(1);
//    spacingSliderZ->setMinimum(0);
//    spacingSliderZ->setValue(10);
//    spacingSliderZ->setMaximum(200);

//    QSlider *sampleSliderX = new QSlider(Qt::Horizontal, widget);
//    sampleSliderX->setTickInterval(1);
//    sampleSliderX->setMinimum(2);
//    sampleSliderX->setValue(10);
//    sampleSliderX->setMaximum(100);
//    sampleSliderX->setEnabled(false);
//    QSlider *sampleSliderZ = new QSlider(Qt::Horizontal, widget);
//    sampleSliderZ->setTickInterval(1);
//    sampleSliderZ->setMinimum(2);
//    sampleSliderZ->setValue(10);
//    sampleSliderZ->setMaximum(100);
//    sampleSliderZ->setEnabled(false);

//    QSlider *fontSizeSlider = new QSlider(Qt::Horizontal, widget);
//    fontSizeSlider->setTickInterval(1);
//    fontSizeSlider->setMinimum(1);
//    fontSizeSlider->setValue(20);
//    fontSizeSlider->setMaximum(100);

//    QFontComboBox *fontList = new QFontComboBox(widget);

//    vLayout->addWidget(staticCheckBox, 0, Qt::AlignTop);
//    vLayout->addWidget(rotationCheckBox, 0, Qt::AlignTop);
//    vLayout->addWidget(rotationSliderX, 0, Qt::AlignTop);
//    vLayout->addWidget(rotationSliderY, 0, Qt::AlignTop);
//    vLayout->addWidget(new QLabel(QStringLiteral("Adjust relative bar size")));
//    vLayout->addWidget(sizeSliderX, 0, Qt::AlignTop);
//    vLayout->addWidget(sizeSliderZ, 0, Qt::AlignTop);
//    vLayout->addWidget(new QLabel(QStringLiteral("Adjust relative bar spacing")));
//    vLayout->addWidget(spacingSliderX, 0, Qt::AlignTop);
//    vLayout->addWidget(spacingSliderZ, 0, Qt::AlignTop);
//    vLayout->addWidget(new QLabel(QStringLiteral("Adjust sample count")));
//    vLayout->addWidget(sampleSliderX, 0, Qt::AlignTop);
//    vLayout->addWidget(sampleSliderZ, 1, Qt::AlignTop);
//    vLayout->addWidget(dataButton, 0, Qt::AlignTop);
//    vLayout->addWidget(themeButton, 0, Qt::AlignTop);
//    vLayout->addWidget(labelButton, 0, Qt::AlignTop);
//    vLayout->addWidget(styleButton, 0, Qt::AlignTop);
//    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
//    vLayout->addWidget(selectionButton, 0, Qt::AlignTop);
//    vLayout->addWidget(gridCheckBox);
//    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
//    vLayout->addWidget(fontList);
//    vLayout->addWidget(new QLabel(QStringLiteral("Adjust font size")));
//    vLayout->addWidget(fontSizeSlider, 0, Qt::AlignTop);
//    // TODO: Add example for setMeshFileName

    widget->show();

    MapsModifier *modifier = new MapsModifier(chart);

//    QObject::connect(rotationSliderX, &QSlider::valueChanged, modifier, &MapsModifier::rotateX);
//    QObject::connect(rotationSliderY, &QSlider::valueChanged, modifier, &MapsModifier::rotateY);

//    QObject::connect(sizeSliderX, &QSlider::valueChanged, modifier, &MapsModifier::setSpecsX);
//    QObject::connect(sizeSliderZ, &QSlider::valueChanged, modifier, &MapsModifier::setSpecsZ);

//    QObject::connect(spacingSliderX, &QSlider::valueChanged, modifier,
//                     &MapsModifier::setSpacingSpecsX);
//    QObject::connect(spacingSliderZ, &QSlider::valueChanged, modifier,
//                     &MapsModifier::setSpacingSpecsZ);

//    QObject::connect(sampleSliderX, &QSlider::valueChanged, modifier,
//                     &MapsModifier::setSampleCountX);
//    QObject::connect(sampleSliderZ, &QSlider::valueChanged, modifier,
//                     &MapsModifier::setSampleCountZ);

//    QObject::connect(fontSizeSlider, &QSlider::valueChanged, modifier,
//                     &MapsModifier::changeFontSize);

//    QObject::connect(styleButton, &QPushButton::clicked, modifier, &MapsModifier::changeStyle);
//    QObject::connect(cameraButton, &QPushButton::clicked, modifier,
//                     &MapsModifier::changePresetCamera);
//    QObject::connect(themeButton, &QPushButton::clicked, modifier, &MapsModifier::changeTheme);
//    QObject::connect(labelButton, &QPushButton::clicked, modifier,
//                     &MapsModifier::changeTransparency);
//    QObject::connect(dataButton, &QPushButton::clicked, modifier, &MapsModifier::addBars);
//    QObject::connect(selectionButton, &QPushButton::clicked, modifier,
//                     &MapsModifier::changeSelectionMode);

//    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,
//                     &MapsModifier::changeFont);

//    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,
//                     &MapsModifier::setGridEnabled);

//    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSliderX,
//                     &QSlider::setEnabled);
//    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSliderX,
//                     &QSlider::setValue);
//    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSliderY,
//                     &QSlider::setEnabled);
//    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSliderY,
//                     &QSlider::setValue);

//    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, dataButton,
//                     &QPushButton::setEnabled);
//    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, sampleSliderX,
//                     &QSlider::setEnabled);
//    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, sampleSliderZ,
//                     &QSlider::setEnabled);
//    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, modifier, &MapsModifier::restart);

    modifier->start();

    return app.exec();
}

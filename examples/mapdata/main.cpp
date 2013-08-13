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

#include "mapdata.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QFontComboBox>
#include <QLabel>
#include <QScreen>
#include <QFontDatabase>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();

    Q3DMaps *chart = new Q3DMaps();
    QSize screenSize = chart->screen()->size();

    QWidget *container = QWidget::createWindowContainer(chart);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("Population of a few cities in Finland"));

    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

//    QPushButton *dataButton = new QPushButton(widget);
//    dataButton->setText(QStringLiteral("Add a row of random data"));
//    dataButton->setEnabled(false);

    QPushButton *themeButton = new QPushButton(widget);
    themeButton->setText(QStringLiteral("Change theme"));

    QPushButton *labelButton = new QPushButton(widget);
    labelButton->setText(QStringLiteral("Change label style"));

    QPushButton *styleButton = new QPushButton(widget);
    styleButton->setText(QStringLiteral("Change bar style"));

    QPushButton *cameraButton = new QPushButton(widget);
    cameraButton->setText(QStringLiteral("Change camera preset"));

//    QPushButton *selectionButton = new QPushButton(widget);
//    selectionButton->setText(QStringLiteral("Change selection mode"));

//    QCheckBox *gridCheckBox = new QCheckBox(widget);
//    gridCheckBox->setText(QStringLiteral("Show grid"));
//    gridCheckBox->setChecked(true);

//    QCheckBox *rotationCheckBox = new QCheckBox(widget);
//    rotationCheckBox->setText("Rotate with slider");

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

    QComboBox *valueDimension = new QComboBox(widget);
    valueDimension->addItem(QStringLiteral("Height"));
    valueDimension->addItem(QStringLiteral("Width"));
    valueDimension->addItem(QStringLiteral("Depth"));
    valueDimension->addItem(QStringLiteral("Radius"));
    valueDimension->addItem(QStringLiteral("All"));
    valueDimension->setCurrentIndex(4);

    QComboBox *shadowQuality = new QComboBox(widget);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->setCurrentIndex(1);

    QFontComboBox *fontList = new QFontComboBox(widget);

    QSlider *fontSizeSlider = new QSlider(Qt::Horizontal, widget);
    fontSizeSlider->setTickInterval(1);
    fontSizeSlider->setMinimum(1);
    fontSizeSlider->setValue(80);
    fontSizeSlider->setMaximum(200);

//    vLayout->addWidget(rotationCheckBox, 0, Qt::AlignTop);
//    vLayout->addWidget(rotationSliderX, 0, Qt::AlignTop);
//    vLayout->addWidget(rotationSliderY, 0, Qt::AlignTop);
//    vLayout->addWidget(new QLabel(QStringLiteral("Adjust relative bar size")));
//    vLayout->addWidget(sizeSliderX, 0, Qt::AlignTop);
//    vLayout->addWidget(sizeSliderZ, 0, Qt::AlignTop);
//    vLayout->addWidget(dataButton, 0, Qt::AlignTop);
    vLayout->addWidget(themeButton, 0, Qt::AlignTop);
    vLayout->addWidget(labelButton, 0, Qt::AlignTop);
    vLayout->addWidget(styleButton, 0, Qt::AlignTop);
    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Apply value to:")));
    vLayout->addWidget(valueDimension, 0, Qt::AlignTop);
//    vLayout->addWidget(selectionButton, 0, Qt::AlignTop);
//    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust font size")));
    vLayout->addWidget(fontSizeSlider, 1, Qt::AlignTop);
//    // TODO: Add example for setMeshFileName

    widget->show();

    MapsModifier *modifier = new MapsModifier(chart);

//    QObject::connect(rotationSliderX, &QSlider::valueChanged, modifier, &MapsModifier::rotateX);
//    QObject::connect(rotationSliderY, &QSlider::valueChanged, modifier, &MapsModifier::rotateY);

//    QObject::connect(sizeSliderX, &QSlider::valueChanged, modifier, &MapsModifier::setSpecsX);
//    QObject::connect(sizeSliderZ, &QSlider::valueChanged, modifier, &MapsModifier::setSpecsZ);

    QObject::connect(fontSizeSlider, &QSlider::valueChanged, modifier,
                     &MapsModifier::changeFontSize);

    QObject::connect(styleButton, &QPushButton::clicked, modifier, &MapsModifier::changeStyle);
    QObject::connect(cameraButton, &QPushButton::clicked, modifier,
                     &MapsModifier::changePresetCamera);
    QObject::connect(themeButton, &QPushButton::clicked, modifier, &MapsModifier::changeTheme);
    QObject::connect(labelButton, &QPushButton::clicked, modifier,
                     &MapsModifier::changeTransparency);
//    QObject::connect(dataButton, &QPushButton::clicked, modifier, &MapsModifier::addBars);
//    QObject::connect(selectionButton, &QPushButton::clicked, modifier,
//                     &MapsModifier::changeSelectionMode);

    QObject::connect(valueDimension, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeValueDimension(int)));

    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeShadowQuality(int)));
    QObject::connect(modifier, &MapsModifier::shadowQuality, shadowQuality,
                     &QComboBox::setCurrentIndex);

    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,
                     &MapsModifier::changeFont);

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

    modifier->start();

    return app.exec();
}

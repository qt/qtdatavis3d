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

#include "chart.h"

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
    QVBoxLayout *vLayout = new QVBoxLayout();

    Q3DBars *widgetchart = new Q3DBars();
    QSize screenSize = widgetchart->screen()->size();

    QWidget *container = QWidget::createWindowContainer(widgetchart);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("Average temperatures in Oulu, Finland (2006-2012)"));

    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    QPushButton *dataButton = new QPushButton(widget);
    dataButton->setText(QStringLiteral("Insert a row of data"));
    dataButton->setEnabled(false);

    QPushButton *multiDataButton = new QPushButton(widget);
    multiDataButton->setText(QStringLiteral("Insert many rows of data"));
    multiDataButton->setEnabled(false);

    QPushButton *changeSingleDataButton = new QPushButton(widget);
    changeSingleDataButton->setText(QStringLiteral("Change selected bar value"));
    changeSingleDataButton->setEnabled(false);

    QPushButton *changeRowButton = new QPushButton(widget);
    changeRowButton->setText(QStringLiteral("Change selected row values"));
    changeRowButton->setEnabled(false);

    QPushButton *changeRowsButton = new QPushButton(widget);
    changeRowsButton->setText(QStringLiteral("Change three rows from selected"));
    changeRowsButton->setEnabled(false);

    QPushButton *removeRowButton = new QPushButton(widget);
    removeRowButton->setText(QStringLiteral("Remove selected row"));
    removeRowButton->setEnabled(false);

    QPushButton *removeRowsButton = new QPushButton(widget);
    removeRowsButton->setText(QStringLiteral("remove three rows from selected"));
    removeRowsButton->setEnabled(false);

    QPushButton *themeButton = new QPushButton(widget);
    themeButton->setText(QStringLiteral("Change theme"));

    QPushButton *labelButton = new QPushButton(widget);
    labelButton->setText(QStringLiteral("Change label style"));

    QPushButton *styleButton = new QPushButton(widget);
    styleButton->setText(QStringLiteral("Change bar style"));

    QPushButton *cameraButton = new QPushButton(widget);
    cameraButton->setText(QStringLiteral("Change camera preset"));

    QPushButton *selectionButton = new QPushButton(widget);
    selectionButton->setText(QStringLiteral("Change selection mode"));

    QPushButton *setSelectedBarButton = new QPushButton(widget);
    setSelectedBarButton->setText(QStringLiteral("Select/deselect bar at (5,5)"));

    QPushButton *swapAxisButton = new QPushButton(widget);
    swapAxisButton->setText(QStringLiteral("Swap value axis"));
    swapAxisButton->setEnabled(false);

    QPushButton *releaseAxesButton = new QPushButton(widget);
    releaseAxesButton->setText(QStringLiteral("Release all axes"));
    releaseAxesButton->setEnabled(true);

    QPushButton *releaseProxiesButton = new QPushButton(widget);
    releaseProxiesButton->setText(QStringLiteral("Release all proxies"));
    releaseProxiesButton->setEnabled(true);

    QCheckBox *backgroundCheckBox = new QCheckBox(widget);
    backgroundCheckBox->setText(QStringLiteral("Show background"));
    backgroundCheckBox->setChecked(true);

    QCheckBox *gridCheckBox = new QCheckBox(widget);
    gridCheckBox->setText(QStringLiteral("Show grid"));
    gridCheckBox->setChecked(true);

    QCheckBox *rotationCheckBox = new QCheckBox(widget);
    rotationCheckBox->setText("Rotate with slider");

    QCheckBox *staticCheckBox = new QCheckBox(widget);
    staticCheckBox->setText("Use dynamic data");
    staticCheckBox->setChecked(false);

    QSlider *rotationSliderX = new QSlider(Qt::Horizontal, widget);
    rotationSliderX->setTickInterval(1);
    rotationSliderX->setMinimum(-180);
    rotationSliderX->setValue(0);
    rotationSliderX->setMaximum(180);
    rotationSliderX->setEnabled(false);
    QSlider *rotationSliderY = new QSlider(Qt::Horizontal, widget);
    rotationSliderY->setTickInterval(1);
    rotationSliderY->setMinimum(0);
    rotationSliderY->setValue(0);
    rotationSliderY->setMaximum(90);
    rotationSliderY->setEnabled(false);

    QSlider *ratioSlider = new QSlider(Qt::Horizontal, widget);
    ratioSlider->setTickInterval(1);
    ratioSlider->setMinimum(10);
    ratioSlider->setValue(30);
    ratioSlider->setMaximum(100);

    QSlider *spacingSliderX = new QSlider(Qt::Horizontal, widget);
    spacingSliderX->setTickInterval(1);
    spacingSliderX->setMinimum(0);
    spacingSliderX->setValue(10);
    spacingSliderX->setMaximum(200);
    QSlider *spacingSliderZ = new QSlider(Qt::Horizontal, widget);
    spacingSliderZ->setTickInterval(1);
    spacingSliderZ->setMinimum(0);
    spacingSliderZ->setValue(10);
    spacingSliderZ->setMaximum(200);

    QSlider *sampleSliderX = new QSlider(Qt::Horizontal, widget);
    sampleSliderX->setTickInterval(1);
    sampleSliderX->setMinimum(2);
    sampleSliderX->setValue(10);
    sampleSliderX->setMaximum(200);
    sampleSliderX->setEnabled(false);
    QSlider *sampleSliderZ = new QSlider(Qt::Horizontal, widget);
    sampleSliderZ->setTickInterval(1);
    sampleSliderZ->setMinimum(2);
    sampleSliderZ->setValue(10);
    sampleSliderZ->setMaximum(200);
    sampleSliderZ->setEnabled(false);

    QSlider *fontSizeSlider = new QSlider(Qt::Horizontal, widget);
    fontSizeSlider->setTickInterval(1);
    fontSizeSlider->setMinimum(1);
    fontSizeSlider->setValue(20);
    fontSizeSlider->setMaximum(100);

    QFontComboBox *fontList = new QFontComboBox(widget);

    QComboBox *shadowQuality = new QComboBox(widget);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(5);

    vLayout->addWidget(staticCheckBox, 0, Qt::AlignTop);
    vLayout->addWidget(rotationCheckBox, 0, Qt::AlignTop);
    vLayout->addWidget(rotationSliderX, 0, Qt::AlignTop);
    vLayout->addWidget(rotationSliderY, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust relative bar size")));
    vLayout->addWidget(ratioSlider, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust relative bar spacing")));
    vLayout->addWidget(spacingSliderX, 0, Qt::AlignTop);
    vLayout->addWidget(spacingSliderZ, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust sample count")));
    vLayout->addWidget(sampleSliderX, 0, Qt::AlignTop);
    vLayout->addWidget(sampleSliderZ, 1, Qt::AlignTop);
    vLayout->addWidget(dataButton, 0, Qt::AlignTop);
    vLayout->addWidget(multiDataButton, 0, Qt::AlignTop);
    vLayout->addWidget(changeSingleDataButton, 0, Qt::AlignTop);
    vLayout->addWidget(changeRowButton, 0, Qt::AlignTop);
    vLayout->addWidget(changeRowsButton, 0, Qt::AlignTop);
    vLayout->addWidget(removeRowButton, 0, Qt::AlignTop);
    vLayout->addWidget(removeRowsButton, 0, Qt::AlignTop);
    vLayout->addWidget(themeButton, 0, Qt::AlignTop);
    vLayout->addWidget(labelButton, 0, Qt::AlignTop);
    vLayout->addWidget(styleButton, 0, Qt::AlignTop);
    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
    vLayout->addWidget(selectionButton, 0, Qt::AlignTop);
    vLayout->addWidget(setSelectedBarButton, 0, Qt::AlignTop);
    vLayout->addWidget(swapAxisButton, 0, Qt::AlignTop);
    vLayout->addWidget(releaseAxesButton, 0, Qt::AlignTop);
    vLayout->addWidget(releaseProxiesButton, 0, Qt::AlignTop);
    vLayout->addWidget(backgroundCheckBox);
    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust font size")));
    vLayout->addWidget(fontSizeSlider, 0, Qt::AlignTop);
    // TODO: Add example for setMeshFileName

    widget->show();

    ChartModifier *modifier = new ChartModifier(widgetchart);

    QObject::connect(rotationSliderX, &QSlider::valueChanged, modifier, &ChartModifier::rotateX);
    QObject::connect(rotationSliderY, &QSlider::valueChanged, modifier, &ChartModifier::rotateY);

    QObject::connect(ratioSlider, &QSlider::valueChanged, modifier, &ChartModifier::setSpecsRatio);

    QObject::connect(spacingSliderX, &QSlider::valueChanged, modifier,
                     &ChartModifier::setSpacingSpecsX);
    QObject::connect(spacingSliderZ, &QSlider::valueChanged, modifier,
                     &ChartModifier::setSpacingSpecsZ);

    QObject::connect(sampleSliderX, &QSlider::valueChanged, modifier,
                     &ChartModifier::setSampleCountX);
    QObject::connect(sampleSliderZ, &QSlider::valueChanged, modifier,
                     &ChartModifier::setSampleCountZ);

    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeShadowQuality(int)));
    QObject::connect(modifier, &ChartModifier::shadowQualityChanged, shadowQuality,
                     &QComboBox::setCurrentIndex);
    QObject::connect(widgetchart, &Q3DBars::shadowQualityChanged, modifier,
                     &ChartModifier::shadowQualityUpdatedByVisual);
    QObject::connect(widgetchart, &Q3DBars::selectedBarPosChanged, modifier,
                     &ChartModifier::handleSelectionChange);

    QObject::connect(fontSizeSlider, &QSlider::valueChanged, modifier,
                     &ChartModifier::changeFontSize);

    QObject::connect(styleButton, &QPushButton::clicked, modifier, &ChartModifier::changeStyle);
    QObject::connect(cameraButton, &QPushButton::clicked, modifier,
                     &ChartModifier::changePresetCamera);
    QObject::connect(themeButton, &QPushButton::clicked, modifier, &ChartModifier::changeTheme);
    QObject::connect(labelButton, &QPushButton::clicked, modifier,
                     &ChartModifier::changeTransparency);
    QObject::connect(dataButton, &QPushButton::clicked, modifier, &ChartModifier::addRow);
    QObject::connect(multiDataButton, &QPushButton::clicked, modifier, &ChartModifier::addRows);
    QObject::connect(changeSingleDataButton, &QPushButton::clicked, modifier, &ChartModifier::changeItem);
    QObject::connect(changeRowButton, &QPushButton::clicked, modifier, &ChartModifier::changeRow);
    QObject::connect(changeRowsButton, &QPushButton::clicked, modifier, &ChartModifier::changeRows);
    QObject::connect(removeRowButton, &QPushButton::clicked, modifier, &ChartModifier::removeRow);
    QObject::connect(removeRowsButton, &QPushButton::clicked, modifier, &ChartModifier::removeRows);
    QObject::connect(selectionButton, &QPushButton::clicked, modifier,
                     &ChartModifier::changeSelectionMode);
    QObject::connect(setSelectedBarButton, &QPushButton::clicked, modifier,
                     &ChartModifier::selectBar);
    QObject::connect(swapAxisButton, &QPushButton::clicked, modifier,
                     &ChartModifier::swapAxis);
    QObject::connect(releaseAxesButton, &QPushButton::clicked, modifier,
                     &ChartModifier::releaseAxes);
    QObject::connect(releaseProxiesButton, &QPushButton::clicked, modifier,
                     &ChartModifier::releaseProxies);

    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,
                     &ChartModifier::changeFont);

    QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,
                     &ChartModifier::setBackgroundEnabled);
    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,
                     &ChartModifier::setGridEnabled);

    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSliderX,
                     &QSlider::setEnabled);
    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSliderX,
                     &QSlider::setValue);
    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSliderY,
                     &QSlider::setEnabled);
    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSliderY,
                     &QSlider::setValue);

    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, dataButton,
                     &QPushButton::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, multiDataButton,
                     &QPushButton::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, changeSingleDataButton,
                     &QPushButton::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, changeRowButton,
                     &QPushButton::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, changeRowsButton,
                     &QPushButton::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, removeRowButton,
                     &QPushButton::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, removeRowsButton,
                     &QPushButton::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, sampleSliderX,
                     &QSlider::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, sampleSliderZ,
                     &QSlider::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, swapAxisButton,
                     &QSlider::setEnabled);
    QObject::connect(staticCheckBox, &QCheckBox::stateChanged, modifier, &ChartModifier::restart);

    modifier->start();

    return app.exec();
}

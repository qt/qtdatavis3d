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
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("Average temperatures in Oulu, Finland (2006-2012)"));

    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

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

    QCheckBox *backgroundCheckBox = new QCheckBox(widget);
    backgroundCheckBox->setText(QStringLiteral("Show background"));
    backgroundCheckBox->setChecked(false);

    QCheckBox *gridCheckBox = new QCheckBox(widget);
    gridCheckBox->setText(QStringLiteral("Show grid"));
    gridCheckBox->setChecked(true);

    QSlider *rotationSliderX = new QSlider(Qt::Horizontal, widget);
    rotationSliderX->setTickInterval(1);
    rotationSliderX->setMinimum(-180);
    rotationSliderX->setValue(0);
    rotationSliderX->setMaximum(180);
    QSlider *rotationSliderY = new QSlider(Qt::Horizontal, widget);
    rotationSliderY->setTickInterval(1);
    rotationSliderY->setMinimum(-90);
    rotationSliderY->setValue(0);
    rotationSliderY->setMaximum(90);

    QSlider *fontSizeSlider = new QSlider(Qt::Horizontal, widget);
    fontSizeSlider->setTickInterval(1);
    fontSizeSlider->setMinimum(1);
    fontSizeSlider->setValue(20);
    fontSizeSlider->setMaximum(100);

    QFontComboBox *fontList = new QFontComboBox(widget);
    fontList->setCurrentFont(QFont("Arial"));

    QComboBox *shadowQuality = new QComboBox(widget);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(5);

    vLayout->addWidget(new QLabel(QStringLiteral("Rotate horizontally")));
    vLayout->addWidget(rotationSliderX, 0, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Rotate vertically")));
    vLayout->addWidget(rotationSliderY, 0, Qt::AlignTop);
    vLayout->addWidget(themeButton, 0, Qt::AlignTop);
    vLayout->addWidget(labelButton, 0, Qt::AlignTop);
    vLayout->addWidget(styleButton, 0, Qt::AlignTop);
    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
    vLayout->addWidget(selectionButton, 0, Qt::AlignTop);
    vLayout->addWidget(backgroundCheckBox);
    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust font size")));
    vLayout->addWidget(fontSizeSlider, 1, Qt::AlignTop);

    widget->show();

    ChartModifier *modifier = new ChartModifier(widgetchart);

    QObject::connect(rotationSliderX, &QSlider::valueChanged, modifier, &ChartModifier::rotateX);
    QObject::connect(rotationSliderY, &QSlider::valueChanged, modifier, &ChartModifier::rotateY);

    QObject::connect(themeButton, &QPushButton::clicked, modifier, &ChartModifier::changeTheme);
    QObject::connect(labelButton, &QPushButton::clicked, modifier,
                     &ChartModifier::changeTransparency);
    QObject::connect(styleButton, &QPushButton::clicked, modifier, &ChartModifier::changeStyle);
    QObject::connect(cameraButton, &QPushButton::clicked, modifier,
                     &ChartModifier::changePresetCamera);
    QObject::connect(selectionButton, &QPushButton::clicked, modifier,
                     &ChartModifier::changeSelectionMode);

    QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,
                     &ChartModifier::setBackgroundEnabled);
    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,
                     &ChartModifier::setGridEnabled);

    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeShadowQuality(int)));
    QObject::connect(modifier, &ChartModifier::shadowQualityChanged, shadowQuality,
                     &QComboBox::setCurrentIndex);
    QObject::connect(widgetchart, &Q3DBars::shadowQualityChanged, modifier,
                     &ChartModifier::shadowQualityUpdatedByVisual);

    QObject::connect(fontSizeSlider, &QSlider::valueChanged, modifier,
                     &ChartModifier::changeFontSize);
    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,
                     &ChartModifier::changeFont);

    modifier->start();

    return app.exec();
}

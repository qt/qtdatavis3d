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

#include "scatterchart.h"

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

    Q3DScatter *chart = new Q3DScatter();
    QSize screenSize = chart->screen()->size();

    QWidget *container = QWidget::createWindowContainer(chart);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("values of some things in something"));

    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    QPushButton *themeButton = new QPushButton(widget);
    themeButton->setText(QStringLiteral("Change theme"));

    QPushButton *labelButton = new QPushButton(widget);
    labelButton->setText(QStringLiteral("Change label style"));

    QPushButton *styleButton = new QPushButton(widget);
    styleButton->setText(QStringLiteral("Change item style"));

    QPushButton *cameraButton = new QPushButton(widget);
    cameraButton->setText(QStringLiteral("Change camera preset"));

    QPushButton *clearButton = new QPushButton(widget);
    clearButton->setText(QStringLiteral("Clear chart"));

    QPushButton *addOneButton = new QPushButton(widget);
    addOneButton->setText(QStringLiteral("Add item"));

    QPushButton *addBunchButton = new QPushButton(widget);
    addBunchButton->setText(QStringLiteral("Add bunch of items"));

    QPushButton *insertOneButton = new QPushButton(widget);
    insertOneButton->setText(QStringLiteral("Insert item"));

    QPushButton *insertBunchButton = new QPushButton(widget);
    insertBunchButton->setText(QStringLiteral("Insert bunch of items"));

    QPushButton *changeOneButton = new QPushButton(widget);
    changeOneButton->setText(QStringLiteral("Change selected item"));

    QPushButton *changeBunchButton = new QPushButton(widget);
    changeBunchButton->setText(QStringLiteral("Change bunch of items"));

    QPushButton *removeOneButton = new QPushButton(widget);
    removeOneButton->setText(QStringLiteral("Remove selected item"));

    QPushButton *removeBunchButton = new QPushButton(widget);
    removeBunchButton->setText(QStringLiteral("Remove bunch of items"));

    QPushButton *setSelectedItemButton = new QPushButton(widget);
    setSelectedItemButton->setText(QStringLiteral("Select/deselect item 3"));

    QPushButton *startTimerButton = new QPushButton(widget);
    startTimerButton->setText(QStringLiteral("Start/stop timer"));

    QCheckBox *backgroundCheckBox = new QCheckBox(widget);
    backgroundCheckBox->setText(QStringLiteral("Show background"));
    backgroundCheckBox->setChecked(true);

    QCheckBox *gridCheckBox = new QCheckBox(widget);
    gridCheckBox->setText(QStringLiteral("Show grid"));
    gridCheckBox->setChecked(true);

    QComboBox *shadowQuality = new QComboBox(widget);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(3);

    QFontComboBox *fontList = new QFontComboBox(widget);

    QSlider *fontSizeSlider = new QSlider(Qt::Horizontal, widget);
    fontSizeSlider->setTickInterval(1);
    fontSizeSlider->setMinimum(1);
    fontSizeSlider->setValue(30);
    fontSizeSlider->setMaximum(200);

    vLayout->addWidget(themeButton, 0, Qt::AlignTop);
    vLayout->addWidget(labelButton, 0, Qt::AlignTop);
    vLayout->addWidget(styleButton, 0, Qt::AlignTop);
    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
    vLayout->addWidget(clearButton, 0, Qt::AlignTop);
    vLayout->addWidget(addOneButton, 0, Qt::AlignTop);
    vLayout->addWidget(addBunchButton, 0, Qt::AlignTop);
    vLayout->addWidget(insertOneButton, 0, Qt::AlignTop);
    vLayout->addWidget(insertBunchButton, 0, Qt::AlignTop);
    vLayout->addWidget(changeOneButton, 0, Qt::AlignTop);
    vLayout->addWidget(changeBunchButton, 0, Qt::AlignTop);
    vLayout->addWidget(removeOneButton, 0, Qt::AlignTop);
    vLayout->addWidget(removeBunchButton, 0, Qt::AlignTop);
    vLayout->addWidget(setSelectedItemButton, 0, Qt::AlignTop);
    vLayout->addWidget(startTimerButton, 0, Qt::AlignTop);
    vLayout->addWidget(backgroundCheckBox);
    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust font size")));
    vLayout->addWidget(fontSizeSlider, 1, Qt::AlignTop);

    widget->show();

    ScatterDataModifier *modifier = new ScatterDataModifier(chart);

    QObject::connect(fontSizeSlider, &QSlider::valueChanged, modifier,
                     &ScatterDataModifier::changeFontSize);

    QObject::connect(styleButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::changeStyle);
    QObject::connect(cameraButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::changePresetCamera);
    QObject::connect(clearButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::clear);
    QObject::connect(addOneButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::addOne);
    QObject::connect(addBunchButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::addBunch);
    QObject::connect(insertOneButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::insertOne);
    QObject::connect(insertBunchButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::insertBunch);
    QObject::connect(changeOneButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::changeOne);
    QObject::connect(changeBunchButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::changeBunch);
    QObject::connect(removeOneButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::removeOne);
    QObject::connect(removeBunchButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::removeBunch);
    QObject::connect(setSelectedItemButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::selectItem);
    QObject::connect(startTimerButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::startStopTimer);
    QObject::connect(themeButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::changeTheme);
    QObject::connect(labelButton, &QPushButton::clicked, modifier,
                     &ScatterDataModifier::changeStyle);

    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeShadowQuality(int)));
    QObject::connect(modifier, &ScatterDataModifier::shadowQualityChanged, shadowQuality,
                     &QComboBox::setCurrentIndex);
    QObject::connect(chart, &Q3DScatter::shadowQualityChanged, modifier,
                     &ScatterDataModifier::shadowQualityUpdatedByVisual);
    QObject::connect(chart, &Q3DScatter::selectedItemIndexChanged, modifier,
                     &ScatterDataModifier::handleSelectionChange);

    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,
                     &ScatterDataModifier::changeFont);

    QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,
                     &ScatterDataModifier::setBackgroundEnabled);
    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,
                     &ScatterDataModifier::setGridEnabled);

    modifier->start();

    return app.exec();
}

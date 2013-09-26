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

#include "chartmodifier.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QSlider>
#include <QLabel>
#include <QScreen>
#include <QPainter>
#include <QFontComboBox>
#include <QDebug>

using namespace QtDataVisualization;

const int initialTheme = 4;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);

    Q3DSurface *surfaceChart = new Q3DSurface();
    QSize screenSize = surfaceChart->screen()->size();

    // Set to default, should be same as the initial on themeList
    surfaceChart->setTheme(QDataVis::ColorTheme(initialTheme));

    QWidget *container = QWidget::createWindowContainer(surfaceChart);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("Surface tester"));

    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    QCheckBox *smoothCB = new QCheckBox(widget);
    smoothCB->setText(QStringLiteral("Smooth "));
    smoothCB->setChecked(surfaceChart->isSmoothSurfaceEnabled());

    QCheckBox *surfaceGridCB = new QCheckBox(widget);
    surfaceGridCB->setText(QStringLiteral("Surface Grid"));
    surfaceGridCB->setChecked(true);

    //QCheckBox *sqrtSinCB = new QCheckBox(widget);
    QRadioButton *sqrtSinCB = new QRadioButton(widget);
    sqrtSinCB->setText(QStringLiteral("Sqrt & Sin"));
    sqrtSinCB->setChecked(false);

    QRadioButton *planeCB = new QRadioButton(widget);
    planeCB->setText(QStringLiteral("Plane"));
    planeCB->setChecked(false);

    QRadioButton *heightMapCB = new QRadioButton(widget);
    heightMapCB->setText(QStringLiteral("Height map"));
    heightMapCB->setChecked(false);

    QCheckBox *gridSlidersLockCB = new QCheckBox(widget);
    gridSlidersLockCB->setText(QStringLiteral("Lock"));
    gridSlidersLockCB->setChecked(false);

    QSlider *gridSliderX = new QSlider(Qt::Horizontal, widget);
    gridSliderX->setTickInterval(1);
    gridSliderX->setMinimum(2);
    gridSliderX->setValue(30);
    gridSliderX->setMaximum(200);
    gridSliderX->setEnabled(true);
    QSlider *gridSliderZ = new QSlider(Qt::Horizontal, widget);
    gridSliderZ->setTickInterval(1);
    gridSliderZ->setMinimum(2);
    gridSliderZ->setValue(30);
    gridSliderZ->setMaximum(200);
    gridSliderZ->setEnabled(true);

    QSlider *axisRangeSliderX = new QSlider(Qt::Horizontal, widget);
    axisRangeSliderX->setTickInterval(1);
    axisRangeSliderX->setMinimum(2);
    axisRangeSliderX->setValue(16);
    axisRangeSliderX->setMaximum(100);
    axisRangeSliderX->setEnabled(true);
    QSlider *axisRangeSliderZ = new QSlider(Qt::Horizontal, widget);
    axisRangeSliderZ->setTickInterval(1);
    axisRangeSliderZ->setMinimum(2);
    axisRangeSliderZ->setValue(16);
    axisRangeSliderZ->setMaximum(100);
    axisRangeSliderZ->setEnabled(true);

    QSlider *axisMinSliderX = new QSlider(Qt::Horizontal, widget);
    axisMinSliderX->setTickInterval(1);
    axisMinSliderX->setMinimum(-50);
    axisMinSliderX->setValue(-8);
    axisMinSliderX->setMaximum(50);
    axisMinSliderX->setEnabled(true);
    QSlider *axisMinSliderZ = new QSlider(Qt::Horizontal, widget);
    axisMinSliderZ->setTickInterval(1);
    axisMinSliderZ->setMinimum(-50);
    axisMinSliderZ->setValue(-8);
    axisMinSliderZ->setMaximum(50);
    axisMinSliderZ->setEnabled(true);

    QLinearGradient gr(0, 0, 100, 1);
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.33, Qt::blue);
    gr.setColorAt(0.67, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);
    QPixmap pm(100, 24);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(gr));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 100, 24);
    QPushButton *colorPB = new QPushButton();
    colorPB->setIcon(QIcon(pm));
    colorPB->setIconSize(QSize(100, 24));

    QFontComboBox *fontList = new QFontComboBox(widget);
    fontList->setCurrentFont(QFont("Arial"));

    QPushButton *labelButton = new QPushButton(widget);
    labelButton->setText(QStringLiteral("Change label style"));

    QComboBox *themeList = new QComboBox(widget);
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));
    themeList->setCurrentIndex(initialTheme);

//    QComboBox *shadowQuality = new QComboBox(widget);
//    shadowQuality->addItem(QStringLiteral("None"));
//    shadowQuality->addItem(QStringLiteral("Low"));
//    shadowQuality->addItem(QStringLiteral("Medium"));
//    shadowQuality->addItem(QStringLiteral("High"));
//    shadowQuality->addItem(QStringLiteral("Low Soft"));
//    shadowQuality->addItem(QStringLiteral("Medium Soft"));
//    shadowQuality->addItem(QStringLiteral("High Soft"));
//    shadowQuality->setCurrentIndex(3);
    QComboBox *selectionMode = new QComboBox(widget);
    selectionMode->addItem(QStringLiteral("ModeNone"));
    selectionMode->addItem(QStringLiteral("ModeItem"));
    selectionMode->addItem(QStringLiteral("ModeSliceRow"));
    selectionMode->addItem(QStringLiteral("ModeSliceColumn"));
    selectionMode->setCurrentIndex(1);

    // Add controls to the layout
    vLayout->addWidget(smoothCB);
    vLayout->addWidget(surfaceGridCB);
    vLayout->addWidget(new QLabel(QStringLiteral("Select surface sample")));
    vLayout->addWidget(sqrtSinCB);
    vLayout->addWidget(planeCB);
    vLayout->addWidget(heightMapCB);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust sample count")));
    vLayout->addWidget(gridSlidersLockCB);
    vLayout->addWidget(gridSliderX);
    vLayout->addWidget(gridSliderZ);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust axis range")));
    vLayout->addWidget(axisRangeSliderX);
    vLayout->addWidget(axisRangeSliderZ);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust axis minimum")));
    vLayout->addWidget(axisMinSliderX);
    vLayout->addWidget(axisMinSliderZ);
    vLayout->addWidget(colorPB);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList);
    vLayout->addWidget(labelButton);
    vLayout->addWidget(new QLabel(QStringLiteral("Change theme")));
    vLayout->addWidget(themeList);
//    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
//    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Selection Mode")));
    vLayout->addWidget(selectionMode);

    widget->show();

    ChartModifier *modifier = new ChartModifier(surfaceChart);

    // Connect controls to slots on modifier
    QObject::connect(smoothCB, &QCheckBox::stateChanged,
                     modifier, &ChartModifier::toggleSmooth);
    QObject::connect(surfaceGridCB, &QCheckBox::stateChanged,
                     modifier, &ChartModifier::toggleSurfaceGrid);
    QObject::connect(sqrtSinCB, &QRadioButton::toggled,
                     modifier, &ChartModifier::toggleSqrtSin);
    QObject::connect(planeCB, &QCheckBox::toggled,
                     modifier, &ChartModifier::togglePlane);
    QObject::connect(heightMapCB, &QCheckBox::toggled,
                     modifier, &ChartModifier::setHeightMapData);
    QObject::connect(gridSlidersLockCB, &QCheckBox::stateChanged,
                     modifier, &ChartModifier::toggleGridSliderLock);
    QObject::connect(gridSliderX, &QSlider::valueChanged,
                     modifier, &ChartModifier::adjustXCount);
    QObject::connect(gridSliderZ, &QSlider::valueChanged,
                     modifier, &ChartModifier::adjustZCount);
    QObject::connect(axisRangeSliderX, &QSlider::valueChanged,
                     modifier, &ChartModifier::adjustXRange);
    QObject::connect(axisRangeSliderZ, &QSlider::valueChanged,
                     modifier, &ChartModifier::adjustZRange);
    QObject::connect(axisMinSliderX, &QSlider::valueChanged,
                     modifier, &ChartModifier::adjustXMin);
    QObject::connect(axisMinSliderZ, &QSlider::valueChanged,
                     modifier, &ChartModifier::adjustZMin);
    QObject::connect(colorPB, &QPushButton::pressed,
                     modifier, &ChartModifier::gradientPressed);
    QObject::connect(fontList, &QFontComboBox::currentFontChanged,
                     modifier, &ChartModifier::changeFont);
    QObject::connect(labelButton, &QPushButton::clicked,
                     modifier, &ChartModifier::changeTransparency);
    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)),
                     modifier, SLOT(changeTheme(int)));
//    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)),
//                     modifier, SLOT(changeShadowQuality(int)));
    QObject::connect(selectionMode, SIGNAL(currentIndexChanged(int)),
                     modifier, SLOT(changeSelectionMode(int)));

    modifier->setGridSliderZ(gridSliderZ);
    modifier->setGridSliderX(gridSliderX);
    modifier->setAxisRangeSliderX(axisRangeSliderX);
    modifier->setAxisRangeSliderZ(axisRangeSliderZ);
    modifier->setAxisMinSliderX(axisMinSliderX);
    modifier->setAxisMinSliderZ(axisMinSliderZ);
    modifier->toggleGridSliderLock(gridSlidersLockCB->checkState());
    sqrtSinCB->setChecked(true);

    return app.exec();
}

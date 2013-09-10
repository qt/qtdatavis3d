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
#include <QDebug>

using namespace QtDataVisualization;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);

    Q3DSurface *surfaceChart = new Q3DSurface();
    QSize screenSize = surfaceChart->screen()->size();

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
    smoothCB->setChecked(surfaceChart->smoothSurface());

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
    gridSliderX->setValue(10);
    gridSliderX->setMaximum(200);
    gridSliderX->setEnabled(true);
    QSlider *gridSliderZ = new QSlider(Qt::Horizontal, widget);
    gridSliderZ->setTickInterval(1);
    gridSliderZ->setMinimum(2);
    gridSliderZ->setValue(10);
    gridSliderZ->setMaximum(200);
    gridSliderZ->setEnabled(true);

    QLinearGradient gr(0, 0, 100, 1);
    gr.setColorAt(0.0, Qt::green);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(1.0, Qt::red);
    QPixmap pm(100, 24);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(gr));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 100, 24);
    //pm.save("C:\\Users\\misalmel\\Work\\test.png", "png");
    QPushButton *color = new QPushButton();
    color->setIcon(QIcon(pm));
    color->setIconSize(QSize(100, 24));
    color->setFlat(true);

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
    vLayout->addWidget(color);

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

    modifier->setGridSliderZ(gridSliderZ);
    modifier->setGridSliderX(gridSliderX);
    modifier->toggleGridSliderLock(gridSlidersLockCB->checkState());

//    QList<qreal> lowList;
//    lowList << 15.0  << 35.0  << 55.0  << 75.0  << 80.0  << 75.0  << 55.0  << 35.0  << 15.0;
//    lowList << 65.0  << 105.0 << 135.0 << 155.0 << 190.0 << 155.0 << 135.0 << 105.0 << 65.0;
//    lowList << 105.0 << 170.0 << 215.0 << 240.0 << 245.0 << 240.0 << 215.0 << 170.0 << 105.0;
//    lowList << 65.0  << 105.0 << 135.0 << 155.0 << 190.0 << 155.0 << 135.0 << 105.0 << 65.0;
//    lowList << 15.0  << 35.0  << 55.0  << 75.0  << 80.0  << 75.0  << 55.0  << 35.0  << 16.1;

//    lowList << 15.0 << 65.0  << 105.0 << 65.0  << 15.0;
//    lowList << 35.0 << 105.0 << 170.0 << 105.0 << 35;
//    lowList << 55.0 << 135.0 << 215.0 << 135.0 << 55;
//    lowList << 75.0 << 155.0 << 240.0 << 155.0 << 75;
//    lowList << 80.0 << 190.0 << 245.0 << 190.0 << 80;
//    lowList << 75.0 << 155.0 << 240.0 << 155.0 << 75.0;
//    lowList << 55.0 << 135.0 << 215.0 << 135.0 << 55;
//    lowList << 35.0 << 105.0 << 170.0 << 105.0 << 35.0;
//    lowList << 15.0 << 65.0  << 105.0 << 65.0  << 16.1;

//   surfaceChart->appendSeries(lowList, 9, 5);

//    QList<qreal> topList;
//    topList << 2.1 << 2.2;
//    surfaceChart.appendSeries(topList);

//    surfaceChart.resize(screenSize.width() / 1.5, screenSize.height() / 1.5);
//    surfaceChart.setPosition(screenSize.width() / 6, screenSize.height() / 6);
//    surfaceChart.show();

    return app.exec();
}

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

#include "graphmodifier.h"
#include <QtDataVisualization/q3dtheme.h>

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
#include <QFrame>
#include <QDebug>

using namespace QtDataVisualization;

const int initialTheme = 4;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    QVBoxLayout *vLayout2 = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);
    vLayout2->setAlignment(Qt::AlignTop);

    Q3DSurface *surfaceGraph = new Q3DSurface();
    QSize screenSize = surfaceGraph->screen()->size();

    // Set to default, should be same as the initial on themeList
    surfaceGraph->activeTheme()->setType(Q3DTheme::Theme(initialTheme));

    QWidget *container = QWidget::createWindowContainer(surfaceGraph);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("Surface tester"));

    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);
    hLayout->addLayout(vLayout2);

    QCheckBox *smoothCB = new QCheckBox(widget);
    smoothCB->setText(QStringLiteral("Flat Surface"));
    smoothCB->setChecked(true);

    QCheckBox *surfaceGridCB = new QCheckBox(widget);
    surfaceGridCB->setText(QStringLiteral("Surface Grid"));
    surfaceGridCB->setChecked(true);

    QCheckBox *surfaceCB = new QCheckBox(widget);
    surfaceCB->setText(QStringLiteral("Surface Visible"));
    surfaceCB->setChecked(true);

    QCheckBox *seriesVisibleCB = new QCheckBox(widget);
    seriesVisibleCB->setText(QStringLiteral("Series Visible"));
    seriesVisibleCB->setChecked(true);

#ifdef MULTI_SERIES
    smoothCB->setText(QStringLiteral("S1 Flat Surface"));
    surfaceGridCB->setText(QStringLiteral("S1 Surface Grid"));
    surfaceCB->setText(QStringLiteral("S1 Surface Visible"));
    seriesVisibleCB->setText(QStringLiteral("Series 1 Visible"));

    QCheckBox *smoothS2CB = new QCheckBox(widget);
    smoothS2CB->setText(QStringLiteral("S2 Flat Surface"));
    smoothS2CB->setChecked(true);

    QCheckBox *surfaceGridS2CB = new QCheckBox(widget);
    surfaceGridS2CB->setText(QStringLiteral("S2 Surface Grid"));
    surfaceGridS2CB->setChecked(true);

    QCheckBox *surfaceS2CB = new QCheckBox(widget);
    surfaceS2CB->setText(QStringLiteral("S2 Surface Visible"));
    surfaceS2CB->setChecked(true);

    QCheckBox *series2VisibleCB = new QCheckBox(widget);
    series2VisibleCB->setText(QStringLiteral("Series 2 Visible"));
    series2VisibleCB->setChecked(true);

    QCheckBox *smoothS3CB = new QCheckBox(widget);
    smoothS3CB->setText(QStringLiteral("S3 Flat Surface"));
    smoothS3CB->setChecked(true);

    QCheckBox *surfaceGridS3CB = new QCheckBox(widget);
    surfaceGridS3CB->setText(QStringLiteral("S3 Surface Grid"));
    surfaceGridS3CB->setChecked(true);

    QCheckBox *surfaceS3CB = new QCheckBox(widget);
    surfaceS3CB->setText(QStringLiteral("S3 Surface Visible"));
    surfaceS3CB->setChecked(true);

    QCheckBox *series3VisibleCB = new QCheckBox(widget);
    series3VisibleCB->setText(QStringLiteral("Series 3 Visible"));
    series3VisibleCB->setChecked(true);

    QCheckBox *smoothS4CB = new QCheckBox(widget);
    smoothS4CB->setText(QStringLiteral("S4 Flat Surface"));
    smoothS4CB->setChecked(true);

    QCheckBox *surfaceGridS4CB = new QCheckBox(widget);
    surfaceGridS4CB->setText(QStringLiteral("S4 Surface Grid"));
    surfaceGridS4CB->setChecked(true);

    QCheckBox *surfaceS4CB = new QCheckBox(widget);
    surfaceS4CB->setText(QStringLiteral("S4 Surface Visible"));
    surfaceS4CB->setChecked(true);

    QCheckBox *series4VisibleCB = new QCheckBox(widget);
    series4VisibleCB->setText(QStringLiteral("Series 4 Visible"));
    series4VisibleCB->setChecked(true);

    QCheckBox *series1CB = new QCheckBox(widget);
    series1CB->setText(QStringLiteral("Series 1"));

    QCheckBox *series2CB = new QCheckBox(widget);
    series2CB->setText(QStringLiteral("Series 2"));

    QCheckBox *series3CB = new QCheckBox(widget);
    series3CB->setText(QStringLiteral("Series 3"));

    QCheckBox *series4CB = new QCheckBox(widget);
    series4CB->setText(QStringLiteral("Series 4"));
#else
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
#endif

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

    QPushButton *meshButton = new QPushButton(widget);
    meshButton->setText(QStringLiteral("Change pointer mesh"));

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

    QComboBox *shadowQuality = new QComboBox(widget);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(3);

    QComboBox *selectionMode = new QComboBox(widget);
    selectionMode->addItem(QStringLiteral("None"),
                               int(QAbstract3DGraph::SelectionNone));
    selectionMode->addItem(QStringLiteral("Item"),
                               int(QAbstract3DGraph::SelectionItem));
    selectionMode->addItem(QStringLiteral("Multi: Item"),
                               int(QAbstract3DGraph::SelectionItem | QAbstract3DGraph::SelectionMultiSeries));
    selectionMode->addItem(QStringLiteral("Row"),
                               int(QAbstract3DGraph::SelectionRow));
    selectionMode->addItem(QStringLiteral("Item and Row"),
                               int(QAbstract3DGraph::SelectionItemAndRow));
    selectionMode->addItem(QStringLiteral("Column"),
                               int(QAbstract3DGraph::SelectionColumn));
    selectionMode->addItem(QStringLiteral("Item and Column"),
                               int(QAbstract3DGraph::SelectionItemAndColumn));
    selectionMode->addItem(QStringLiteral("Row and Column"),
                               int(QAbstract3DGraph::SelectionRowAndColumn));
    selectionMode->addItem(QStringLiteral("Item, Row and Column"),
                               int(QAbstract3DGraph::SelectionItemRowAndColumn));
    selectionMode->addItem(QStringLiteral("Multi: Item, Row and Column"),
                               int(QAbstract3DGraph::SelectionItemRowAndColumn | QAbstract3DGraph::SelectionMultiSeries));
    selectionMode->addItem(QStringLiteral("Slice into Row"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionRow));
    selectionMode->addItem(QStringLiteral("Slice into Row and Item"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndRow));
    selectionMode->addItem(QStringLiteral("Multi: Slice, Row & Item"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndRow
                                   | QAbstract3DGraph::SelectionMultiSeries));
    selectionMode->addItem(QStringLiteral("Slice into Column"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionColumn));
    selectionMode->addItem(QStringLiteral("Slice into Column and Item"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndColumn));
    selectionMode->addItem(QStringLiteral("Multi: Slice, Column & Item"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndColumn
                                   | QAbstract3DGraph::SelectionMultiSeries));

#ifndef MULTI_SERIES
    QPushButton *selectButton = new QPushButton(widget);
    selectButton->setText(QStringLiteral("Select random point"));

    QPushButton *flipViewsButton = new QPushButton(widget);
    flipViewsButton->setText(QStringLiteral("Flip Views"));

    QLabel *selectionInfoLabel = new QLabel(widget);
#endif

    QPushButton *changeRowButton = new QPushButton(widget);
    changeRowButton->setText(QStringLiteral("Change a row"));

    QPushButton *changeRowsButton = new QPushButton(widget);
    changeRowsButton->setText(QStringLiteral("Change 3 rows"));

    QPushButton *changeItemButton = new QPushButton(widget);
    changeItemButton->setText(QStringLiteral("Change item"));

    QPushButton *changeMultipleItemButton = new QPushButton(widget);
    changeMultipleItemButton->setText(QStringLiteral("Change many items"));

    QPushButton *changeMultipleRowsButton = new QPushButton(widget);
    changeMultipleRowsButton->setText(QStringLiteral("Change many rows"));

    QPushButton *addRowButton = new QPushButton(widget);
    addRowButton->setText(QStringLiteral("Add a row"));

    QPushButton *addRowsButton = new QPushButton(widget);
    addRowsButton->setText(QStringLiteral("Add 3 rows"));

    QPushButton *insertRowButton = new QPushButton(widget);
    insertRowButton->setText(QStringLiteral("Insert a row"));

    QPushButton *insertRowsButton = new QPushButton(widget);
    insertRowsButton->setText(QStringLiteral("Insert 3 rows"));

    QPushButton *removeRowButton = new QPushButton(widget);
    removeRowButton->setText(QStringLiteral("Remove a row"));

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);

    QFrame* line3 = new QFrame();
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);

    // Add controls to the layout
#ifdef MULTI_SERIES
    vLayout->addWidget(series1CB);
#endif
    vLayout->addWidget(smoothCB);
    vLayout->addWidget(surfaceGridCB);
    vLayout->addWidget(surfaceCB);
    vLayout->addWidget(seriesVisibleCB);
#ifdef MULTI_SERIES
    vLayout->addWidget(line);
    vLayout->addWidget(series2CB);
    vLayout->addWidget(smoothS2CB);
    vLayout->addWidget(surfaceGridS2CB);
    vLayout->addWidget(surfaceS2CB);
    vLayout->addWidget(series2VisibleCB);
    vLayout->addWidget(line2);
    vLayout->addWidget(series3CB);
    vLayout->addWidget(smoothS3CB);
    vLayout->addWidget(surfaceGridS3CB);
    vLayout->addWidget(surfaceS3CB);
    vLayout->addWidget(series3VisibleCB);
    vLayout->addWidget(line3);
    vLayout->addWidget(series4CB);
    vLayout->addWidget(smoothS4CB);
    vLayout->addWidget(surfaceGridS4CB);
    vLayout->addWidget(surfaceS4CB);
    vLayout->addWidget(series4VisibleCB);
#endif
#ifndef MULTI_SERIES
    vLayout->addWidget(new QLabel(QStringLiteral("Select surface sample")));
    vLayout->addWidget(sqrtSinCB);
    vLayout->addWidget(planeCB);
    vLayout->addWidget(heightMapCB);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust sample count")));
    vLayout->addWidget(gridSlidersLockCB);
    vLayout->addWidget(gridSliderX);
    vLayout->addWidget(gridSliderZ);
#endif
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust axis range")));
    vLayout->addWidget(axisRangeSliderX);
    vLayout->addWidget(axisRangeSliderZ);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust axis minimum")));
    vLayout->addWidget(axisMinSliderX);
    vLayout->addWidget(axisMinSliderZ);
    vLayout2->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout2->addWidget(fontList);
    vLayout2->addWidget(labelButton);
    vLayout2->addWidget(meshButton);
    vLayout2->addWidget(new QLabel(QStringLiteral("Change theme")));
    vLayout2->addWidget(themeList);
    vLayout2->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout2->addWidget(shadowQuality);
    vLayout2->addWidget(new QLabel(QStringLiteral("Selection Mode")));
    vLayout2->addWidget(selectionMode);
#ifndef MULTI_SERIES
    vLayout2->addWidget(selectButton);
    vLayout2->addWidget(selectionInfoLabel);
    vLayout2->addWidget(flipViewsButton);
#endif

    vLayout2->addWidget(colorPB);
    vLayout2->addWidget(changeRowButton);
    vLayout2->addWidget(changeRowsButton);
    vLayout2->addWidget(changeMultipleRowsButton);
    vLayout2->addWidget(changeItemButton);
    vLayout2->addWidget(changeMultipleItemButton);
    vLayout2->addWidget(addRowButton);
    vLayout2->addWidget(addRowsButton);
    vLayout2->addWidget(insertRowButton);
    vLayout2->addWidget(insertRowsButton);
    vLayout2->addWidget(removeRowButton);

    widget->show();

    GraphModifier *modifier = new GraphModifier(surfaceGraph);

    // Connect controls to slots on modifier
    QObject::connect(smoothCB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSmooth);
    QObject::connect(surfaceGridCB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSurfaceGrid);
    QObject::connect(surfaceCB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSurface);
    QObject::connect(seriesVisibleCB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSeriesVisible);
#ifdef MULTI_SERIES
    QObject::connect(smoothS2CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSmoothS2);
    QObject::connect(surfaceGridS2CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSurfaceGridS2);
    QObject::connect(surfaceS2CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSurfaceS2);
    QObject::connect(series2VisibleCB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSeries2Visible);

    QObject::connect(smoothS3CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSmoothS3);
    QObject::connect(surfaceGridS3CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSurfaceGridS3);
    QObject::connect(surfaceS3CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSurfaceS3);
    QObject::connect(series3VisibleCB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSeries3Visible);

    QObject::connect(smoothS4CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSmoothS4);
    QObject::connect(surfaceGridS4CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSurfaceGridS4);
    QObject::connect(surfaceS4CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSurfaceS4);
    QObject::connect(series4VisibleCB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSeries4Visible);

    QObject::connect(series1CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSeries1);
    QObject::connect(series1CB, &QCheckBox::stateChanged,
                     smoothCB, &QPushButton::setEnabled);
    QObject::connect(series1CB, &QCheckBox::stateChanged,
                     surfaceGridCB, &QPushButton::setEnabled);
    QObject::connect(series1CB, &QCheckBox::stateChanged,
                     surfaceCB, &QPushButton::setEnabled);
    QObject::connect(series1CB, &QCheckBox::stateChanged,
                     seriesVisibleCB, &QPushButton::setEnabled);

    QObject::connect(series2CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSeries2);
    QObject::connect(series2CB, &QCheckBox::stateChanged,
                     smoothS2CB, &QPushButton::setEnabled);
    QObject::connect(series2CB, &QCheckBox::stateChanged,
                     surfaceGridS2CB, &QPushButton::setEnabled);
    QObject::connect(series2CB, &QCheckBox::stateChanged,
                     surfaceS2CB, &QPushButton::setEnabled);
    QObject::connect(series2CB, &QCheckBox::stateChanged,
                     series2VisibleCB, &QPushButton::setEnabled);

    QObject::connect(series3CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSeries3);
    QObject::connect(series3CB, &QCheckBox::stateChanged,
                     smoothS3CB, &QPushButton::setEnabled);
    QObject::connect(series3CB, &QCheckBox::stateChanged,
                     surfaceGridS3CB, &QPushButton::setEnabled);
    QObject::connect(series3CB, &QCheckBox::stateChanged,
                     surfaceS3CB, &QPushButton::setEnabled);
    QObject::connect(series3CB, &QCheckBox::stateChanged,
                     series3VisibleCB, &QPushButton::setEnabled);

    QObject::connect(series4CB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleSeries4);
    QObject::connect(series4CB, &QCheckBox::stateChanged,
                     smoothS4CB, &QPushButton::setEnabled);
    QObject::connect(series4CB, &QCheckBox::stateChanged,
                     surfaceGridS4CB, &QPushButton::setEnabled);
    QObject::connect(series4CB, &QCheckBox::stateChanged,
                     surfaceS4CB, &QPushButton::setEnabled);
    QObject::connect(series4CB, &QCheckBox::stateChanged,
                     series4VisibleCB, &QPushButton::setEnabled);
#else
    QObject::connect(sqrtSinCB, &QRadioButton::toggled,
                     modifier, &GraphModifier::toggleSqrtSin);
    QObject::connect(planeCB, &QCheckBox::toggled,
                     modifier, &GraphModifier::togglePlane);
    QObject::connect(heightMapCB, &QCheckBox::toggled,
                     modifier, &GraphModifier::setHeightMapData);
    QObject::connect(gridSlidersLockCB, &QCheckBox::stateChanged,
                     modifier, &GraphModifier::toggleGridSliderLock);
    QObject::connect(gridSliderX, &QSlider::valueChanged,
                     modifier, &GraphModifier::adjustXCount);
    QObject::connect(gridSliderZ, &QSlider::valueChanged,
                     modifier, &GraphModifier::adjustZCount);
#endif
    QObject::connect(axisRangeSliderX, &QSlider::valueChanged,
                     modifier, &GraphModifier::adjustXRange);
    QObject::connect(axisRangeSliderZ, &QSlider::valueChanged,
                     modifier, &GraphModifier::adjustZRange);
    QObject::connect(axisMinSliderX, &QSlider::valueChanged,
                     modifier, &GraphModifier::adjustXMin);
    QObject::connect(axisMinSliderZ, &QSlider::valueChanged,
                     modifier, &GraphModifier::adjustZMin);
    QObject::connect(colorPB, &QPushButton::pressed,
                     modifier, &GraphModifier::gradientPressed);
    QObject::connect(fontList, &QFontComboBox::currentFontChanged,
                     modifier, &GraphModifier::changeFont);
    QObject::connect(labelButton, &QPushButton::clicked,
                     modifier, &GraphModifier::changeStyle);
    QObject::connect(meshButton, &QPushButton::clicked,
                     modifier, &GraphModifier::changeMesh);
    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)),
                     modifier, SLOT(changeTheme(int)));
    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)),
                     modifier, SLOT(changeShadowQuality(int)));
    QObject::connect(selectionMode, SIGNAL(currentIndexChanged(int)),
                     modifier, SLOT(changeSelectionMode(int)));
#ifndef MULTI_SERIES
    QObject::connect(selectButton, &QPushButton::clicked,
                     modifier, &GraphModifier::selectButtonClicked);
    QObject::connect(flipViewsButton, &QPushButton::clicked,
                     modifier, &GraphModifier::flipViews);
#endif
    QObject::connect(changeRowButton,&QPushButton::clicked,
                     modifier, &GraphModifier::changeRow);
    QObject::connect(changeRowsButton,&QPushButton::clicked,
                     modifier, &GraphModifier::changeRows);
    QObject::connect(changeItemButton,&QPushButton::clicked,
                     modifier, &GraphModifier::changeItem);
    QObject::connect(changeMultipleItemButton,&QPushButton::clicked,
                     modifier, &GraphModifier::changeMultipleItem);
    QObject::connect(changeMultipleRowsButton,&QPushButton::clicked,
                     modifier, &GraphModifier::changeMultipleRows);
    QObject::connect(addRowButton,&QPushButton::clicked,
                     modifier, &GraphModifier::addRow);
    QObject::connect(addRowsButton,&QPushButton::clicked,
                     modifier, &GraphModifier::addRows);
    QObject::connect(insertRowButton,&QPushButton::clicked,
                     modifier, &GraphModifier::insertRow);
    QObject::connect(insertRowsButton,&QPushButton::clicked,
                     modifier, &GraphModifier::insertRows);
    QObject::connect(removeRowButton,&QPushButton::clicked,
                     modifier, &GraphModifier::removeRow);

#ifdef MULTI_SERIES
    modifier->setSeries1CB(series1CB);
    modifier->setSeries2CB(series2CB);
    modifier->setSeries3CB(series3CB);
    modifier->setSeries4CB(series4CB);
    series1CB->setChecked(true);
    series2CB->setChecked(true);
    series3CB->setChecked(true);
    series4CB->setChecked(true);
#endif
    modifier->setAxisRangeSliderX(axisRangeSliderX);
    modifier->setAxisRangeSliderZ(axisRangeSliderZ);
    modifier->setAxisMinSliderX(axisMinSliderX);
    modifier->setAxisMinSliderZ(axisMinSliderZ);
    selectionMode->setCurrentIndex(1);
#ifndef MULTI_SERIES
    modifier->setGridSliderZ(gridSliderZ);
    modifier->setGridSliderX(gridSliderX);
    modifier->toggleGridSliderLock(gridSlidersLockCB->checkState());
    modifier->setSelectionInfoLabel(selectionInfoLabel);
    sqrtSinCB->setChecked(true);
#endif
    shadowQuality->setCurrentIndex(3);

    return app.exec();
}

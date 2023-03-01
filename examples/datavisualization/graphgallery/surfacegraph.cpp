// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "surfacegraph.h"
#include "surfacegraphmodifier.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qradiobutton.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qcommandlinkbutton.h>
#include <QtGui/qpainter.h>

using namespace Qt::StringLiterals;

SurfaceGraph::SurfaceGraph()
{
    m_surfaceGraph = new Q3DSurface();
}

SurfaceGraph::~SurfaceGraph() = default;

bool SurfaceGraph::initialize()
{
    if (!m_surfaceGraph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return -1;
    }

    m_surfaceWidget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(m_surfaceWidget);
    m_container = QWidget::createWindowContainer(m_surfaceGraph, m_surfaceWidget);
    QSize screenSize = m_surfaceGraph->screen()->size();
    m_container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.75));
    m_container->setMaximumSize(screenSize);
    m_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_container->setFocusPolicy(Qt::StrongFocus);
    hLayout->addWidget(m_container, 1);

    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);

    // Create control widgets
    QGroupBox *modelGroupBox = new QGroupBox(u"Model"_s);

    QRadioButton *sqrtSinModelRB = new QRadioButton(m_surfaceWidget);
    sqrtSinModelRB->setText(u"Sqrt && Sin"_s);
    sqrtSinModelRB->setChecked(false);

    QRadioButton *heightMapModelRB = new QRadioButton(m_surfaceWidget);
    heightMapModelRB->setText(u"Multiseries\nHeight Map"_s);
    heightMapModelRB->setChecked(false);

    QRadioButton *texturedModelRB = new QRadioButton(m_surfaceWidget);
    texturedModelRB->setText(u"Textured\nTopography"_s);
    texturedModelRB->setChecked(false);

    QVBoxLayout *modelVBox = new QVBoxLayout;
    modelVBox->addWidget(sqrtSinModelRB);
    modelVBox->addWidget(heightMapModelRB);
    modelVBox->addWidget(texturedModelRB);
    modelGroupBox->setLayout(modelVBox);

    QGroupBox *selectionGroupBox = new QGroupBox(u"Graph Selection Mode"_s);

    QRadioButton *modeNoneRB = new QRadioButton(m_surfaceWidget);
    modeNoneRB->setText(u"No selection"_s);
    modeNoneRB->setChecked(false);

    QRadioButton *modeItemRB = new QRadioButton(m_surfaceWidget);
    modeItemRB->setText(u"Item"_s);
    modeItemRB->setChecked(false);

    QRadioButton *modeSliceRowRB = new QRadioButton(m_surfaceWidget);
    modeSliceRowRB->setText(u"Row Slice"_s);
    modeSliceRowRB->setChecked(false);

    QRadioButton *modeSliceColumnRB = new QRadioButton(m_surfaceWidget);
    modeSliceColumnRB->setText(u"Column Slice"_s);
    modeSliceColumnRB->setChecked(false);

    QVBoxLayout *selectionVBox = new QVBoxLayout;
    selectionVBox->addWidget(modeNoneRB);
    selectionVBox->addWidget(modeItemRB);
    selectionVBox->addWidget(modeSliceRowRB);
    selectionVBox->addWidget(modeSliceColumnRB);
    selectionGroupBox->setLayout(selectionVBox);

    QGroupBox *axisGroupBox = new QGroupBox(u"Axis ranges"_s);

    QSlider *axisMinSliderX = new QSlider(Qt::Horizontal);
    axisMinSliderX->setMinimum(0);
    axisMinSliderX->setTickInterval(1);
    axisMinSliderX->setEnabled(true);
    QSlider *axisMaxSliderX = new QSlider(Qt::Horizontal);
    axisMaxSliderX->setMinimum(1);
    axisMaxSliderX->setTickInterval(1);
    axisMaxSliderX->setEnabled(true);
    QSlider *axisMinSliderZ = new QSlider(Qt::Horizontal);
    axisMinSliderZ->setMinimum(0);
    axisMinSliderZ->setTickInterval(1);
    axisMinSliderZ->setEnabled(true);
    QSlider *axisMaxSliderZ = new QSlider(Qt::Horizontal);
    axisMaxSliderZ->setMinimum(1);
    axisMaxSliderZ->setTickInterval(1);
    axisMaxSliderZ->setEnabled(true);

    QVBoxLayout *axisVBox = new QVBoxLayout;
    axisVBox->addWidget(new QLabel(u"Column range"_s));
    axisVBox->addWidget(axisMinSliderX);
    axisVBox->addWidget(axisMaxSliderX);
    axisVBox->addWidget(new QLabel(u"Row range"_s));
    axisVBox->addWidget(axisMinSliderZ);
    axisVBox->addWidget(axisMaxSliderZ);
    axisGroupBox->setLayout(axisVBox);

    // Mode-dependent controls
    // sqrt-sin
    QGroupBox *colorGroupBox = new QGroupBox(u"Custom gradient"_s);

    QLinearGradient grBtoY(0, 0, 1, 100);
    grBtoY.setColorAt(1.f, Qt::black);
    grBtoY.setColorAt(0.67f, Qt::blue);
    grBtoY.setColorAt(0.33f, Qt::red);
    grBtoY.setColorAt(0.f, Qt::yellow);
    QPixmap pm(24, 100);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grBtoY));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientBtoYPB = new QPushButton(m_surfaceWidget);
    gradientBtoYPB->setIcon(QIcon(pm));
    gradientBtoYPB->setIconSize(QSize(24, 100));

    QLinearGradient grGtoR(0, 0, 1, 100);
    grGtoR.setColorAt(1.f, Qt::darkGreen);
    grGtoR.setColorAt(0.5f, Qt::yellow);
    grGtoR.setColorAt(0.2f, Qt::red);
    grGtoR.setColorAt(0.f, Qt::darkRed);
    pmp.setBrush(QBrush(grGtoR));
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientGtoRPB = new QPushButton(m_surfaceWidget);
    gradientGtoRPB->setIcon(QIcon(pm));
    gradientGtoRPB->setIconSize(QSize(24, 100));

    QHBoxLayout *colorHBox = new QHBoxLayout;
    colorHBox->addWidget(gradientBtoYPB);
    colorHBox->addWidget(gradientGtoRPB);
    colorGroupBox->setLayout(colorHBox);

    // Multiseries heightmap
    QGroupBox *showGroupBox = new QGroupBox(u"_show Object"_s);
    showGroupBox->setVisible(false);

    QCheckBox *checkboxShowOilRigOne = new QCheckBox(u"Oil Rig 1"_s);
    checkboxShowOilRigOne->setChecked(true);

    QCheckBox *checkboxShowOilRigTwo = new QCheckBox(u"Oil Rig 2"_s);
    checkboxShowOilRigTwo->setChecked(true);

    QCheckBox *checkboxShowRefinery = new QCheckBox(u"Refinery"_s);

    QVBoxLayout *showVBox = new QVBoxLayout;
    showVBox->addWidget(checkboxShowOilRigOne);
    showVBox->addWidget(checkboxShowOilRigTwo);
    showVBox->addWidget(checkboxShowRefinery);
    showGroupBox->setLayout(showVBox);

    QGroupBox *visualsGroupBox = new QGroupBox(u"Visuals"_s);
    visualsGroupBox->setVisible(false);

    QCheckBox *checkboxVisualsSeeThrough = new QCheckBox(u"See-Through"_s);

    QCheckBox *checkboxHighlightOil = new QCheckBox(u"Highlight Oil"_s);

    QCheckBox *checkboxShowShadows = new QCheckBox(u"Shadows"_s);
    checkboxShowShadows->setChecked(true);

    QVBoxLayout *visualVBox = new QVBoxLayout;
    visualVBox->addWidget(checkboxVisualsSeeThrough);
    visualVBox->addWidget(checkboxHighlightOil);
    visualVBox->addWidget(checkboxShowShadows);
    visualsGroupBox->setLayout(visualVBox);

    QLabel *labelSelection = new QLabel(u"Selection:"_s);
    labelSelection->setVisible(false);

    QLabel *labelSelectedItem = new QLabel(u"Nothing"_s);
    labelSelectedItem->setVisible(false);

    // Textured topography heightmap
    QCheckBox *enableTexture = new QCheckBox(u"Surface texture"_s);
    enableTexture->setVisible(false);

    int height = 400;
    int width = 110;
    int border = 10;
    QLinearGradient gr(0, 0, 1, height - 2 * border);
    gr.setColorAt(1.f, Qt::black);
    gr.setColorAt(0.8f, Qt::darkGreen);
    gr.setColorAt(0.6f, Qt::green);
    gr.setColorAt(0.4f, Qt::yellow);
    gr.setColorAt(0.2f, Qt::red);
    gr.setColorAt(0.f, Qt::darkRed);

    QPixmap pmHighlight(width, height);
    pmHighlight.fill(Qt::transparent);
    QPainter pmpHighlight(&pmHighlight);
    pmpHighlight.setBrush(QBrush(gr));
    pmpHighlight.setPen(Qt::NoPen);
    pmpHighlight.drawRect(border, border, 35, height - 2 * border);
    pmpHighlight.setPen(Qt::black);
    int step = (height - 2 * border) / 5;
    for (int i = 0; i < 6; i++) {
        int yPos = i * step + border;
        pmpHighlight.drawLine(border, yPos, 55, yPos);
        const int height = 550 - (i * 110);
        pmpHighlight.drawText(60, yPos + 2, QString::number(height) + u" m"_s);
    }

    QLabel *label = new QLabel(m_surfaceWidget);
    label->setPixmap(pmHighlight);

    QGroupBox *heightMapGroupBox = new QGroupBox(u"Highlight color map"_s);
    QVBoxLayout *colorMapVBox = new QVBoxLayout;
    colorMapVBox->addWidget(label);
    heightMapGroupBox->setLayout(colorMapVBox);
    heightMapGroupBox->setVisible(false);

    // Populate vertical layout
    // Common
    vLayout->addWidget(modelGroupBox);
    vLayout->addWidget(selectionGroupBox);
    vLayout->addWidget(axisGroupBox);

    // Sqrt Sin
    vLayout->addWidget(colorGroupBox);

    // Multiseries heightmap
    vLayout->addWidget(showGroupBox);
    vLayout->addWidget(visualsGroupBox);
    vLayout->addWidget(labelSelection);
    vLayout->addWidget(labelSelectedItem);

    // Textured topography
    vLayout->addWidget(heightMapGroupBox);
    vLayout->addWidget(enableTexture);

    // Create the controller
    SurfaceGraphModifier *modifier = new SurfaceGraphModifier(m_surfaceGraph, labelSelectedItem);

    // Connect widget controls to controller
    QObject::connect(heightMapModelRB, &QRadioButton::toggled,
                     modifier, &SurfaceGraphModifier::enableHeightMapModel);
    QObject::connect(sqrtSinModelRB, &QRadioButton::toggled,
                     modifier, &SurfaceGraphModifier::enableSqrtSinModel);
    QObject::connect(texturedModelRB, &QRadioButton::toggled,
                     modifier, &SurfaceGraphModifier::enableTopographyModel);

    QObject::connect(modeNoneRB, &QRadioButton::toggled,
                     modifier, &SurfaceGraphModifier::toggleModeNone);
    QObject::connect(modeItemRB,  &QRadioButton::toggled,
                     modifier, &SurfaceGraphModifier::toggleModeItem);
    QObject::connect(modeSliceRowRB,  &QRadioButton::toggled,
                     modifier, &SurfaceGraphModifier::toggleModeSliceRow);
    QObject::connect(modeSliceColumnRB,  &QRadioButton::toggled,
                     modifier, &SurfaceGraphModifier::toggleModeSliceColumn);

    QObject::connect(axisMinSliderX, &QSlider::valueChanged,
                     modifier, &SurfaceGraphModifier::adjustXMin);
    QObject::connect(axisMaxSliderX, &QSlider::valueChanged,
                     modifier, &SurfaceGraphModifier::adjustXMax);
    QObject::connect(axisMinSliderZ, &QSlider::valueChanged,
                     modifier, &SurfaceGraphModifier::adjustZMin);
    QObject::connect(axisMaxSliderZ, &QSlider::valueChanged,
                     modifier, &SurfaceGraphModifier::adjustZMax);

    // Mode dependent connections
    QObject::connect(gradientBtoYPB, &QPushButton::pressed,
                     modifier, &SurfaceGraphModifier::setBlackToYellowGradient);
    QObject::connect(gradientGtoRPB, &QPushButton::pressed,
                     modifier, &SurfaceGraphModifier::setGreenToRedGradient);

    QObject::connect(checkboxShowOilRigOne, &QCheckBox::stateChanged,
                     modifier, &SurfaceGraphModifier::toggleItemOne);
    QObject::connect(checkboxShowOilRigTwo, &QCheckBox::stateChanged,
                     modifier, &SurfaceGraphModifier::toggleItemTwo);
    QObject::connect(checkboxShowRefinery, &QCheckBox::stateChanged,
                     modifier, &SurfaceGraphModifier::toggleItemThree);

    QObject::connect(checkboxVisualsSeeThrough, &QCheckBox::stateChanged,
                     modifier, &SurfaceGraphModifier::toggleSeeThrough);
    QObject::connect(checkboxHighlightOil, &QCheckBox::stateChanged,
                     modifier, &SurfaceGraphModifier::toggleOilHighlight);
    QObject::connect(checkboxShowShadows, &QCheckBox::stateChanged,
                     modifier, &SurfaceGraphModifier::toggleShadows);

    QObject::connect(enableTexture, &QCheckBox::stateChanged,
                     modifier, &SurfaceGraphModifier::toggleSurfaceTexture);

    // Connections to disable features depending on mode
    QObject::connect(sqrtSinModelRB, &QRadioButton::toggled,
                     colorGroupBox, &QGroupBox::setVisible);

    QObject::connect(heightMapModelRB, &QRadioButton::toggled,
                     showGroupBox, &QGroupBox::setVisible);
    QObject::connect(heightMapModelRB, &QRadioButton::toggled,
                     visualsGroupBox, &QGroupBox::setVisible);
    QObject::connect(heightMapModelRB, &QRadioButton::toggled,
                     labelSelection, &QLabel::setVisible);
    QObject::connect(heightMapModelRB, &QRadioButton::toggled,
                     labelSelectedItem, &QLabel::setVisible);

    QObject::connect(texturedModelRB, &QRadioButton::toggled,
                     enableTexture, &QLabel::setVisible);
    QObject::connect(texturedModelRB, &QRadioButton::toggled,
                     heightMapGroupBox, &QGroupBox::setVisible);

    modifier->setAxisMinSliderX(axisMinSliderX);
    modifier->setAxisMaxSliderX(axisMaxSliderX);
    modifier->setAxisMinSliderZ(axisMinSliderZ);
    modifier->setAxisMaxSliderZ(axisMaxSliderZ);

    sqrtSinModelRB->setChecked(true);
    modeItemRB->setChecked(true);
    enableTexture->setChecked(true);

    return true;
}

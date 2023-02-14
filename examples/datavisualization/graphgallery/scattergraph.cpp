// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "scattergraph.h"
#include "scatterdatamodifier.h"

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qcommandlinkbutton.h>

ScatterGraph::ScatterGraph()
{
    m_scatterGraph = new Q3DScatter();
    m_container = QWidget::createWindowContainer(m_scatterGraph);
}

ScatterGraph::~ScatterGraph()
{
    delete m_container;
}

bool ScatterGraph::initialize()
{
    if (!m_scatterGraph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return -1;
    }

    QSize screenSize = m_scatterGraph->screen()->size();
    m_container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.75));
    m_container->setMaximumSize(screenSize);
    m_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_container->setFocusPolicy(Qt::StrongFocus);

    m_scatterWidget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(m_scatterWidget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(m_container, 1);
    hLayout->addLayout(vLayout);

    QCommandLinkButton *cameraButton = new QCommandLinkButton(m_scatterWidget);
    cameraButton->setText(QStringLiteral("Change camera preset"));
    cameraButton->setDescription(QStringLiteral("Switch between a number of preset camera positions"));
    cameraButton->setIconSize(QSize(0, 0));

    QCommandLinkButton *itemCountButton = new QCommandLinkButton(m_scatterWidget);
    itemCountButton->setText(QStringLiteral("Toggle item count"));
    itemCountButton->setDescription(QStringLiteral("Switch between 900 and 10000 data points"));
    itemCountButton->setIconSize(QSize(0, 0));

    QCommandLinkButton *rangeButton = new QCommandLinkButton(m_scatterWidget);
    rangeButton->setText(QStringLiteral("Toggle axis ranges"));
    rangeButton->setDescription(QStringLiteral("Switch between automatic axis ranges and preset ranges"));
    rangeButton->setIconSize(QSize(0, 0));

    QCheckBox *backgroundCheckBox = new QCheckBox(m_scatterWidget);
    backgroundCheckBox->setText(QStringLiteral("Show background"));
    backgroundCheckBox->setChecked(true);

    QCheckBox *gridCheckBox = new QCheckBox(m_scatterWidget);
    gridCheckBox->setText(QStringLiteral("Show grid"));
    gridCheckBox->setChecked(true);

    QCheckBox *smoothCheckBox = new QCheckBox(m_scatterWidget);
    smoothCheckBox->setText(QStringLiteral("Smooth dots"));
    smoothCheckBox->setChecked(true);

    QComboBox *itemStyleList = new QComboBox(m_scatterWidget);
    itemStyleList->addItem(QStringLiteral("Sphere"), int(QAbstract3DSeries::MeshSphere));
    itemStyleList->addItem(QStringLiteral("Cube"), int(QAbstract3DSeries::MeshCube));
    itemStyleList->addItem(QStringLiteral("Minimal"), int(QAbstract3DSeries::MeshMinimal));
    itemStyleList->addItem(QStringLiteral("Point"), int(QAbstract3DSeries::MeshPoint));
    itemStyleList->setCurrentIndex(0);

    QComboBox *themeList = new QComboBox(m_scatterWidget);
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));
    themeList->setCurrentIndex(3);

    QComboBox *shadowQuality = new QComboBox(m_scatterWidget);
    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(6);

    vLayout->addWidget(cameraButton);
    vLayout->addWidget(itemCountButton);
    vLayout->addWidget(rangeButton);
    vLayout->addWidget(backgroundCheckBox);
    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(smoothCheckBox);
    vLayout->addWidget(new QLabel(QStringLiteral("Change dot style")));
    vLayout->addWidget(itemStyleList);
    vLayout->addWidget(new QLabel(QStringLiteral("Change theme")));
    vLayout->addWidget(themeList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality, 1, Qt::AlignTop);

    ScatterDataModifier *modifier = new ScatterDataModifier(m_scatterGraph);

    QObject::connect(cameraButton, &QCommandLinkButton::clicked, modifier,
                     &ScatterDataModifier::changePresetCamera);
    QObject::connect(itemCountButton, &QCommandLinkButton::clicked, modifier,
                     &ScatterDataModifier::toggleItemCount);
    QObject::connect(rangeButton, &QCommandLinkButton::clicked, modifier,
                     &ScatterDataModifier::toggleRanges);

    QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,
                     &ScatterDataModifier::setBackgroundEnabled);
    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,
                     &ScatterDataModifier::setGridEnabled);
    QObject::connect(smoothCheckBox, &QCheckBox::stateChanged, modifier,
                     &ScatterDataModifier::setSmoothDots);

    QObject::connect(modifier, &ScatterDataModifier::backgroundEnabledChanged,
                     backgroundCheckBox, &QCheckBox::setChecked);
    QObject::connect(modifier, &ScatterDataModifier::gridEnabledChanged,
                     gridCheckBox, &QCheckBox::setChecked);
    QObject::connect(itemStyleList, &QComboBox::currentIndexChanged, modifier,
                     qOverload<int>(&ScatterDataModifier::changeStyle));

    QObject::connect(themeList, &QComboBox::currentIndexChanged, modifier,
                     qOverload<int>(&ScatterDataModifier::changeTheme));

    QObject::connect(shadowQuality, &QComboBox::currentIndexChanged, modifier,
                     qOverload<int>(&ScatterDataModifier::changeShadowQuality));

    QObject::connect(modifier, &ScatterDataModifier::shadowQualityChanged, shadowQuality,
                     &QComboBox::setCurrentIndex);
    QObject::connect(m_scatterGraph, &Q3DScatter::shadowQualityChanged, modifier,
                     &ScatterDataModifier::shadowQualityUpdatedByVisual);

    return true;
}

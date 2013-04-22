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

#include "chart.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QScreen>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout(widget);

    Q3DBars *widgetchart = new Q3DBars();
    QSize screenSize = widgetchart->screen()->size();

    QWidget *container = QWidget::createWindowContainer(widgetchart);
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    widget->setWindowTitle(QStringLiteral("Average temperatures in Oulu, Finland (2008-2012)"));

    hLayout->addWidget(container);
    hLayout->addLayout(vLayout);

    QPushButton *themeButton = new QPushButton(widget);
    themeButton->setText(QStringLiteral("Change theme"));
    QPushButton *styleButton = new QPushButton(widget);
    styleButton->setText(QStringLiteral("Change bar style"));
    QPushButton *cameraButton = new QPushButton(widget);
    cameraButton->setText(QStringLiteral("Change camera preset"));
    QCheckBox *rotationCheckBox = new QCheckBox(widget);
    rotationCheckBox->setText("Rotate with slider");
    QSlider *rotationSlider = new QSlider(Qt::Horizontal);
    rotationSlider->setTickInterval(1);
    rotationSlider->setMinimum(-180);
    rotationSlider->setValue(0);
    rotationSlider->setMaximum(180);
    rotationSlider->setEnabled(false);

    vLayout->addWidget(rotationCheckBox, 0, Qt::AlignTop);
    vLayout->addWidget(rotationSlider, 1, Qt::AlignTop);
    vLayout->addWidget(themeButton, 0, Qt::AlignTop);
    vLayout->addWidget(styleButton, 0, Qt::AlignTop);
    vLayout->addWidget(cameraButton, 0, Qt::AlignTop);

    widget->show();

    ChartModifier *modifier = new ChartModifier(widgetchart);

    QObject::connect(rotationSlider, &QSlider::valueChanged, modifier, &ChartModifier::rotate);
    QObject::connect(styleButton, &QPushButton::clicked, modifier, &ChartModifier::changeStyle);
    QObject::connect(cameraButton, &QPushButton::clicked, modifier,
                     &ChartModifier::changePresetCamera);
    QObject::connect(themeButton, &QPushButton::clicked, modifier, &ChartModifier::changeTheme);
    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSlider,
                     &QSlider::setEnabled);
    QObject::connect(rotationCheckBox, &QCheckBox::stateChanged, rotationSlider,
                     &QSlider::setValue);

    modifier->start();

    return app.exec();
}

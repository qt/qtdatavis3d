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

using namespace QtDataVis3D;

const QString celsiusString = QString(QChar(0xB0)) + "C";

ChartModifier::ChartModifier(Q3DBars *barchart)
    : m_chart(barchart),
      m_columnCount(21),
      m_rowCount(21),
      m_xRotation(0.0f),
      m_yRotation(0.0f),
      m_static(true),
      m_barWidth(1.0f),
      m_barDepth(1.0f),
      m_barSpacingX(0.1f),
      m_barSpacingZ(0.1f),
      m_fontSize(20)
{
    // Don't set any styles or specifications, start from defaults
}

ChartModifier::~ChartModifier()
{
    delete m_chart;
}

void ChartModifier::start()
{
    if (m_static)
        addDataSet();
}

void ChartModifier::restart(bool dynamicData)
{
    m_static = !dynamicData;

    if (m_static) {
        start();
        // Set selection mode to zoom row
        m_chart->setSelectionMode(Q3DBars::ZoomRow);
        m_chart->setFont(QFont("Times Roman", 20));
    } else {
        // Set up sample space; make it as deep as it's wide
        m_chart->setupSampleSpace(m_columnCount, m_rowCount);
        // Set selection mode to full
        m_chart->setSelectionMode(Q3DBars::BarRowAndColumn);
    }
}

void ChartModifier::addDataSet()
{
    // Prepare data to be visualized
    // Use QDataSet adder

    // Set window title
    m_chart->setWindowTitle(QStringLiteral("Average temperatures in Oulu, Finland (2006-2012)"));

    // Set up row and column names
    QVector<QString> months;
    months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    QVector<QString> years;
    years << "2006" << "2007" << "2008" << "2009" << "2010" << "2011" << "2012";

    // Set up data
    float temp[7][12] = {{-6.7f, -11.7f, -9.7f, 3.3f, 9.2f, 14.0f, 16.3f, 17.8f, 10.2f, 2.1f, -2.6f, -0.3f},    // 2006
                         {-6.8f, -13.3f, 0.2f, 1.5f, 7.9f, 13.4f, 16.1f, 15.5f, 8.2f, 5.4f, -2.6f, -0.8f},      // 2007
                         {-4.2f, -4.0f, -4.6f, 1.9f, 7.3f, 12.5f, 15.0f, 12.8f, 7.6f, 5.1f, -0.9f, -1.3f},      // 2008
                         {-7.8f, -8.8f, -4.2f, 0.7f, 9.3f, 13.2f, 15.8f, 15.5f, 11.2f, 0.6f, 0.7f, -8.4f},      // 2009
                         {-14.4f, -12.1f, -7.0f, 2.3f, 11.0f, 12.6f, 18.8f, 13.8f, 9.4f, 3.9f, -5.6f, -13.0f},  // 2010
                         {-9.0f, -15.2f, -3.8f, 2.6f, 8.3f, 15.9f, 18.6f, 14.9f, 11.1f, 5.3f, 1.8f, -0.2f},     // 2011
                         {-8.7f, -11.3f, -2.3f, 0.4f, 7.5f, 12.2f, 16.4f, 14.1f, 9.2f, 3.1f, 0.3f, -12.1f}};    // 2012

    // Create data set
    QDataSet *dataSet = new QDataSet();

    // Add labels
    dataSet->setLabels("Year", "Month", "Average temperature (" + celsiusString + ")",
                       years, months);

    // Create data rows
    QDataRow *dataRow;
    for (int year = 0; year < years.size(); year++) {
        dataRow = new QDataRow(years.at(year));
        // Create data items
        for (int month = 0; month < months.size(); month++) {
            // Add data to rows
            dataRow->addItem(new QDataItem(temp[year][month], celsiusString));
        }
        // Add row to set
        dataSet->addRow(dataRow);
        // Get next pointer
        dataRow++;
    }

    // Set up sample space based on prepared data
    m_chart->setupSampleSpace(months.size(), years.size());

    // Add data to chart
    m_chart->addDataSet(dataSet);
}

void ChartModifier::addBars()
{
    QVector<float> data;
    for (float i = 0; i < m_columnCount; i++)
        data.append(((i + 1) / (float)m_columnCount) * (float)(rand() % 100));
    m_chart->addDataRow(data);
}

void ChartModifier::changeStyle()
{
    static int model = 0;
    switch (model) {
    case 0:
        m_chart->setBarType(Q3DBars::Cylinders, false);
        break;
    case 1:
        m_chart->setBarType(Q3DBars::Cylinders, true);
        break;
    case 2:
        m_chart->setBarType(Q3DBars::Cones, false);
        break;
    case 3:
        m_chart->setBarType(Q3DBars::Cones, true);
        break;
    case 4:
        m_chart->setBarType(Q3DBars::Bars, false);
        break;
    case 5:
        m_chart->setBarType(Q3DBars::Bars, true);
        break;
    case 6:
        m_chart->setBarType(Q3DBars::Pyramids, false);
        break;
    case 7:
        m_chart->setBarType(Q3DBars::Pyramids, true);
        break;
    }
    model++;
    if (model > 7)
        model = 0;
}

void ChartModifier::changePresetCamera()
{
    static int preset = 0;

    m_chart->setCameraPreset((Q3DBars::CameraPreset)preset);

    if (++preset > (int)Q3DBars::PresetDirectlyAboveCCW45)
        preset = 0;
}

void ChartModifier::changeTheme()
{
    static int theme = 0;

    m_chart->setTheme((Q3DBars::ColorTheme)theme);

    if (++theme > (int)Q3DBars::ThemeLight)
        theme = 0;
}
void ChartModifier::changeTransparency()
{
    static int transparency = 0;

    m_chart->setLabelTransparency((Q3DBars::LabelTransparency)transparency);

    if (++transparency > (int)Q3DBars::TransparencyNoBackground)
        transparency = 0;
}

void ChartModifier::changeSelectionMode()
{
    static int selectionMode = 0;

    m_chart->setSelectionMode((Q3DBars::SelectionMode)selectionMode);

    if (++selectionMode > (int)Q3DBars::ZoomColumn)
        selectionMode = 0;
}

void ChartModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    newFont.setPixelSize(m_fontSize);
    m_chart->setFont(newFont);
}

void ChartModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    m_chart->setFontSize((GLfloat)m_fontSize);
}

void ChartModifier::setGridEnabled(int enabled)
{
    m_chart->setGridEnabled((bool)enabled);
}

void ChartModifier::rotateX(int rotation)
{
    m_xRotation = rotation;
    m_chart->setCameraPosition(m_xRotation, m_yRotation);
}

void ChartModifier::rotateY(int rotation)
{
    m_yRotation = rotation;
    m_chart->setCameraPosition(m_xRotation, m_yRotation);
}

void ChartModifier::setSpecsX(int barwidth)
{
    m_barWidth = (float)barwidth / 100.0f;
    m_chart->setBarSpecs(QSizeF(m_barWidth, m_barDepth), QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSpecsZ(int bardepth)
{
    m_barDepth = (float)bardepth / 100.0f;
    m_chart->setBarSpecs(QSizeF(m_barWidth, m_barDepth), QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSpacingSpecsX(int spacing)
{
    m_barSpacingX = (float)spacing / 100.0f;
    m_chart->setBarSpecs(QSizeF(m_barWidth, m_barDepth), QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSpacingSpecsZ(int spacing)
{
    m_barSpacingZ = (float)spacing / 100.0f;
    m_chart->setBarSpecs(QSizeF(m_barWidth, m_barDepth), QSizeF(m_barSpacingX, m_barSpacingZ));
}

void ChartModifier::setSampleCountX(int samples)
{
    m_columnCount = samples;
    m_chart->setupSampleSpace(m_columnCount, m_rowCount);
}

void ChartModifier::setSampleCountZ(int samples)
{
    m_rowCount = samples;
    m_chart->setupSampleSpace(m_columnCount, m_rowCount);
}

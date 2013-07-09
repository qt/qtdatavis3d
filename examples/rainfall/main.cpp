/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
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

#include "q3dbars.h"
#include "qdataitem.h"
#include "qcategoryaxis.h"
#include "qolddataproxy.h"

#include <QGuiApplication>
#include <QFont>
#include <QDebug>

using namespace QtDataVis3D;

class RainfallChart : public QObject
{
public:
    explicit RainfallChart(Q3DBars *rainfall);
    ~RainfallChart();

    void addDataSet();
    void start();

private:
    Q3DBars *m_chart;
    int m_columnCount;
    int m_rowCount;
};

RainfallChart::RainfallChart(Q3DBars *rainfall)
    : m_chart(rainfall),
      m_columnCount(12),
      m_rowCount(13)
{
    // Set up bar specifications; make the bars as wide as they are deep,
    // and add a small space between the bars
    m_chart->setBarSpecs(QSizeF(1.0f, 1.0f), QSizeF(0.2f, 0.2f), true);

    // Set up sample space; make it match actual data size
    m_chart->setupSampleSpace(m_rowCount, m_columnCount);

    // Set axis labels and titles
    QStringList months;
    months << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    QStringList years;
    years << "2000" << "2001" << "2002" << "2003" << "2004" << "2005" << "2006" << "2007" << "2008" << "2009" << "2010" << "2011" << "2012";

    m_chart->rowAxis()->setTitle("Year");
    m_chart->columnAxis()->setTitle("Month");
    m_chart->valueAxis()->setTitle("rainfall (in mm)");
    m_chart->rowAxis()->setLabels(years);
    m_chart->columnAxis()->setLabels(months);

    // Set bar type to cylinder
    m_chart->setBarType(Cylinders, false);

    // Set shadows to medium
    m_chart->setShadowQuality(ShadowMedium);

    // Set font
    m_chart->setFont(QFont("Century Gothic", 40));

    // Set selection mode to bar and column
    //m_chart->setSelectionMode(ModeBarAndColumn);
    m_chart->setSelectionMode(ModeZoomColumn);

    // Set theme
    m_chart->setTheme(ThemeBlueNcs);

    // Set preset camera position
    m_chart->setCameraPreset(PresetIsometricRightHigh);

    // Disable grid
    m_chart->setGridVisible(false);

    // Set window title
    m_chart->setWindowTitle(QStringLiteral("Monthly rainfall in Northern Finland (2000-2012)"));
}

RainfallChart::~RainfallChart()
{
    delete m_chart;
}

void RainfallChart::start()
{
    addDataSet();
}

void RainfallChart::addDataSet()
{
    // Fill in rainfall per month from 2000 to 2012 in Northern Finland (Sodankylä, Utsjoki, Kuusamo)
    QVector< QVector<QDataItem*> > data;
    QVector<QDataItem*> row;
    // TODO: Change this example to load data from file
    // 2000
    row.append(new QDataItem(72, "mm")); //January 2000
    row.append(new QDataItem(47, "mm")); //February 2000
    row.append(new QDataItem(37, "mm")); //March 2000
    row.append(new QDataItem(79, "mm")); //April 2000
    row.append(new QDataItem(42, "mm")); //May 2000
    row.append(new QDataItem(73, "mm")); //June 2000
    row.append(new QDataItem(94, "mm")); //July 2000
    row.append(new QDataItem(37, "mm")); //August 2000
    row.append(new QDataItem(17, "mm")); //September 2000
    row.append(new QDataItem(69, "mm")); //October 2000
    row.append(new QDataItem(42, "mm")); //November 2000
    row.append(new QDataItem(42, "mm")); //December 2000
    data.append(row);
    row.clear();
    // 2001
    row.append(new QDataItem(25, "mm")); //January 2001
    row.append(new QDataItem(47, "mm")); //February 2001
    row.append(new QDataItem(20, "mm")); //March 2001
    row.append(new QDataItem(70, "mm")); //April 2001
    row.append(new QDataItem(27, "mm")); //May 2001
    row.append(new QDataItem(40, "mm")); //June 2001
    row.append(new QDataItem(123, "mm")); //July 2001
    row.append(new QDataItem(39, "mm")); //August 2001
    row.append(new QDataItem(66, "mm")); //September 2001
    row.append(new QDataItem(55, "mm")); //October 2001
    row.append(new QDataItem(29, "mm")); //November 2001
    row.append(new QDataItem(12, "mm")); //December 2001
    data.append(row);
    row.clear();
    // 2002
    row.append(new QDataItem(24, "mm")); //January 2002
    row.append(new QDataItem(45, "mm")); //February 2002
    row.append(new QDataItem(27, "mm")); //March 2002
    row.append(new QDataItem(30, "mm")); //April 2002
    row.append(new QDataItem(16, "mm")); //May 2002
    row.append(new QDataItem(98, "mm")); //June 2002
    row.append(new QDataItem(122, "mm")); //July 2002
    row.append(new QDataItem(20, "mm")); //August 2002
    row.append(new QDataItem(50, "mm")); //September 2002
    row.append(new QDataItem(24, "mm")); //October 2002
    row.append(new QDataItem(22, "mm")); //November 2002
    row.append(new QDataItem(12, "mm")); //December 2002
    data.append(row);
    row.clear();
    // 2003
    row.append(new QDataItem(43, "mm")); //January 2003
    row.append(new QDataItem(17, "mm")); //February 2003
    row.append(new QDataItem(26, "mm")); //March 2003
    row.append(new QDataItem(22, "mm")); //April 2003
    row.append(new QDataItem(60, "mm")); //May 2003
    row.append(new QDataItem(14, "mm")); //June 2003
    row.append(new QDataItem(86, "mm")); //July 2003
    row.append(new QDataItem(77, "mm")); //August 2003
    row.append(new QDataItem(69, "mm")); //September 2003
    row.append(new QDataItem(49, "mm")); //October 2003
    row.append(new QDataItem(23, "mm")); //November 2003
    row.append(new QDataItem(44, "mm")); //December 2003
    data.append(row);
    row.clear();
    // 2004
    row.append(new QDataItem(15, "mm")); //January 2004
    row.append(new QDataItem(19, "mm")); //February 2004
    row.append(new QDataItem(10, "mm")); //March 2004
    row.append(new QDataItem(11, "mm")); //April 2004
    row.append(new QDataItem(41, "mm")); //May 2004
    row.append(new QDataItem(29, "mm")); //June 2004
    row.append(new QDataItem(49, "mm")); //July 2004
    row.append(new QDataItem(72, "mm")); //August 2004
    row.append(new QDataItem(50, "mm")); //September 2004
    row.append(new QDataItem(18, "mm")); //October 2004
    row.append(new QDataItem(19, "mm")); //November 2004
    row.append(new QDataItem(40, "mm")); //December 2004
    data.append(row);
    row.clear();
    // 2005
    row.append(new QDataItem(60, "mm")); //January 2005
    row.append(new QDataItem(24, "mm")); //February 2005
    row.append(new QDataItem(12, "mm")); //March 2005
    row.append(new QDataItem(50, "mm")); //April 2005
    row.append(new QDataItem(88, "mm")); //May 2005
    row.append(new QDataItem(32, "mm")); //June 2005
    row.append(new QDataItem(76, "mm")); //July 2005
    row.append(new QDataItem(55, "mm")); //August 2005
    row.append(new QDataItem(92, "mm")); //September 2005
    row.append(new QDataItem(35, "mm")); //October 2005
    row.append(new QDataItem(105, "mm")); //November 2005
    row.append(new QDataItem(59, "mm")); //December 2005
    data.append(row);
    row.clear();
    // 2006
    row.append(new QDataItem(27, "mm")); //January 2006
    row.append(new QDataItem(18, "mm")); //February 2006
    row.append(new QDataItem(17, "mm")); //March 2006
    row.append(new QDataItem(26, "mm")); //April 2006
    row.append(new QDataItem(24, "mm")); //May 2006
    row.append(new QDataItem(18, "mm")); //June 2006
    row.append(new QDataItem(35, "mm")); //July 2006
    row.append(new QDataItem(28, "mm")); //August 2006
    row.append(new QDataItem(80, "mm")); //September 2006
    row.append(new QDataItem(52, "mm")); //October 2006
    row.append(new QDataItem(43, "mm")); //November 2006
    row.append(new QDataItem(44, "mm")); //December 2006
    data.append(row);
    row.clear();
    // 2007
    row.append(new QDataItem(41, "mm")); //January 2007
    row.append(new QDataItem(21, "mm")); //February 2007
    row.append(new QDataItem(30, "mm")); //March 2007
    row.append(new QDataItem(20, "mm")); //April 2007
    row.append(new QDataItem(53, "mm")); //May 2007
    row.append(new QDataItem(29, "mm")); //June 2007
    row.append(new QDataItem(139, "mm")); //July 2007
    row.append(new QDataItem(52, "mm")); //August 2007
    row.append(new QDataItem(51, "mm")); //September 2007
    row.append(new QDataItem(24, "mm")); //October 2007
    row.append(new QDataItem(47, "mm")); //November 2007
    row.append(new QDataItem(33, "mm")); //December 2007
    data.append(row);
    row.clear();
    // 2008
    row.append(new QDataItem(67, "mm")); //January 2008
    row.append(new QDataItem(19, "mm")); //February 2008
    row.append(new QDataItem(30, "mm")); //March 2008
    row.append(new QDataItem(31, "mm")); //April 2008
    row.append(new QDataItem(29, "mm")); //May 2008
    row.append(new QDataItem(79, "mm")); //June 2008
    row.append(new QDataItem(75, "mm")); //July 2008
    row.append(new QDataItem(99, "mm")); //August 2008
    row.append(new QDataItem(34, "mm")); //September 2008
    row.append(new QDataItem(52, "mm")); //October 2008
    row.append(new QDataItem(60, "mm")); //November 2008
    row.append(new QDataItem(20, "mm")); //December 2008
    data.append(row);
    row.clear();
    // 2009
    row.append(new QDataItem(9, "mm")); //January 2009
    row.append(new QDataItem(22, "mm")); //February 2009
    row.append(new QDataItem(11, "mm")); //March 2009
    row.append(new QDataItem(10, "mm")); //April 2009
    row.append(new QDataItem(69, "mm")); //May 2009
    row.append(new QDataItem(30, "mm")); //June 2009
    row.append(new QDataItem(78, "mm")); //July 2009
    row.append(new QDataItem(93, "mm")); //August 2009
    row.append(new QDataItem(70, "mm")); //September 2009
    row.append(new QDataItem(32, "mm")); //October 2009
    row.append(new QDataItem(56, "mm")); //November 2009
    row.append(new QDataItem(23, "mm")); //December 2009
    data.append(row);
    row.clear();
    // 2010
    row.append(new QDataItem(12, "mm")); //January 2010
    row.append(new QDataItem(28, "mm")); //February 2010
    row.append(new QDataItem(55, "mm")); //March 2010
    row.append(new QDataItem(20, "mm")); //April 2010
    row.append(new QDataItem(65, "mm")); //May 2010
    row.append(new QDataItem(26, "mm")); //June 2010
    row.append(new QDataItem(134, "mm")); //July 2010
    row.append(new QDataItem(57, "mm")); //August 2010
    row.append(new QDataItem(51, "mm")); //September 2010
    row.append(new QDataItem(53, "mm")); //October 2010
    row.append(new QDataItem(8, "mm")); //November 2010
    row.append(new QDataItem(9, "mm")); //December 2010
    data.append(row);
    row.clear();
    // 2011
    row.append(new QDataItem(34, "mm")); //January 2011
    row.append(new QDataItem(20, "mm")); //February 2011
    row.append(new QDataItem(30, "mm")); //March 2011
    row.append(new QDataItem(31, "mm")); //April 2011
    row.append(new QDataItem(42, "mm")); //May 2011
    row.append(new QDataItem(78, "mm")); //June 2011
    row.append(new QDataItem(85, "mm")); //July 2011
    row.append(new QDataItem(33, "mm")); //August 2011
    row.append(new QDataItem(42, "mm")); //September 2011
    row.append(new QDataItem(87, "mm")); //October 2011
    row.append(new QDataItem(41, "mm")); //November 2011
    row.append(new QDataItem(72, "mm")); //December 2011
    data.append(row);
    row.clear();
    // 2012
    row.append(new QDataItem(32, "mm")); //January 2012
    row.append(new QDataItem(42, "mm")); //February 2012
    row.append(new QDataItem(30, "mm")); //March 2012
    row.append(new QDataItem(50, "mm")); //April 2012
    row.append(new QDataItem(30, "mm")); //May 2012
    row.append(new QDataItem(70, "mm")); //June 2012
    row.append(new QDataItem(52, "mm")); //July 2012
    row.append(new QDataItem(20, "mm")); //August 2012
    row.append(new QDataItem(99, "mm")); //September 2012
    row.append(new QDataItem(70, "mm")); //October 2012
    row.append(new QDataItem(69, "mm")); //November 2012
    row.append(new QDataItem(49, "mm")); //December 2012
    data.append(row);
    row.clear();

    // TODO QDataSet reverses the data in rows and columns when you add it, so results are now mirrored.
    qWarning() << "Example broken! QDataSet mirrors data, rows and columns will not be correct!";
    QOldDataProxy *proxy = new QOldDataProxy;
    m_chart->setDataProxy(proxy);
    static_cast<QOldDataProxy *>(m_chart->dataProxy())->addDataSet(data);
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    Q3DBars rainfall;
    rainfall.resize(1280, 800);
    rainfall.setPosition(QPoint(10, 30));
    rainfall.show();

    RainfallChart *rainfallchart = new RainfallChart(&rainfall);
    rainfallchart->start();

    return app.exec();
}

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

#ifndef CUSTOMITEMGRAPH_H
#define CUSTOMITEMGRAPH_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>

using namespace QtDataVisualization;

class CustomItemGraph : public QObject
{
    Q_OBJECT
public:
    explicit CustomItemGraph(Q3DSurface *surface, QLabel *label);
    ~CustomItemGraph();

    void toggleItemOne(bool show);
    void toggleItemTwo(bool show);
    void toggleItemThree(bool show);
    void toggleSeeThrough(bool seethrough);
    void toggleOilHighlight(bool highlight);
    void toggleShadows(bool shadows);

private:
    void handleElementSelected(QAbstract3DGraph::ElementType type);

private:
    Q3DSurface *m_graph;
    QLabel *m_textField;
};

#endif

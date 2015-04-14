/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
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
#include <QtCore/QPropertyAnimation>

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
    void resetSelection();

private:
    Q3DSurface *m_graph;
    QLabel *m_textField;
    QPropertyAnimation *m_selectionAnimation;
    QCustom3DItem *m_previouslyAnimatedItem;
    QVector3D m_previousScaling;
};

#endif

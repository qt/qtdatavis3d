// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef CUSTOMITEMGRAPH_H
#define CUSTOMITEMGRAPH_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QtCore/QPropertyAnimation>

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

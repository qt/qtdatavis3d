// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QABSTRACT3DGRAPH_H
#define QABSTRACT3DGRAPH_H

#include <QtGraphs/qgraphsglobal.h>
#include <QtGraphs/q3dtheme.h>
#include <QtGraphs/q3dscene.h>
#include <QtGraphs/qabstract3dinputhandler.h>
#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtCore/QLocale>
#include <QtQuick/QQuickWindow>
#include <QtQuickWidgets/QQuickWidget>

QT_BEGIN_NAMESPACE

class QCustom3DItem;
class QAbstract3DAxis;
class QAbstract3DSeries;
class QQuickDataVisItem;

class Q_GRAPHS_EXPORT QAbstract3DGraph : public QQuickWidget
{
    Q_OBJECT
    Q_FLAGS(SelectionFlag SelectionFlags)
    Q_FLAGS(OptimizationHint OptimizationHints)
    Q_PROPERTY(Q3DTheme* activeTheme READ activeTheme WRITE setActiveTheme NOTIFY activeThemeChanged)
    Q_PROPERTY(QAbstract3DGraph::SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(QAbstract3DGraph::ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
    Q_PROPERTY(Q3DScene* scene READ scene CONSTANT)

public:
     enum SelectionFlag {
        SelectionNone              = 0,
        SelectionItem              = 1,
        SelectionRow               = 2,
        SelectionItemAndRow        = SelectionItem | SelectionRow,
        SelectionColumn            = 4,
        SelectionItemAndColumn     = SelectionItem | SelectionColumn,
        SelectionRowAndColumn      = SelectionRow | SelectionColumn,
        SelectionItemRowAndColumn  = SelectionItem | SelectionRow | SelectionColumn,
        SelectionSlice             = 8,
        SelectionMultiSeries       = 16
    };
    Q_DECLARE_FLAGS(SelectionFlags, SelectionFlag)

    enum ShadowQuality {
        ShadowQualityNone = 0,
        ShadowQualityLow,
        ShadowQualityMedium,
        ShadowQualityHigh,
        ShadowQualitySoftLow,
        ShadowQualitySoftMedium,
        ShadowQualitySoftHigh
    };
    Q_ENUM(ShadowQuality)

    enum ElementType {
        ElementNone = 0,
        ElementSeries,
        ElementAxisXLabel,
        ElementAxisYLabel,
        ElementAxisZLabel,
        ElementCustomItem
    };
    Q_ENUM(ElementType)

    enum OptimizationHint {
        OptimizationDefault = 0,
        OptimizationStatic  = 1
    };
    Q_DECLARE_FLAGS(OptimizationHints, OptimizationHint)

    Q3DScene *scene() const;
    QAbstract3DGraph::ShadowQuality shadowQuality() const;
    void setShadowQuality(const QAbstract3DGraph::ShadowQuality &shadowQuality);
    Q3DTheme *activeTheme() const;
    void setActiveTheme(Q3DTheme *activeTheme);
    QAbstract3DGraph::SelectionFlags selectionMode() const;
    void setSelectionMode(const QAbstract3DGraph::SelectionFlags &selectionMode);
    virtual ~QAbstract3DGraph();

protected:
    QAbstract3DGraph();

Q_SIGNALS:
    void shadowQualityChanged(QAbstract3DGraph::ShadowQuality quality);
    void activeThemeChanged(Q3DTheme *activeTheme);
    void selectionModeChanged(const QAbstract3DGraph::SelectionFlags selectionMode);

private:
    Q_DISABLE_COPY(QAbstract3DGraph)
    QScopedPointer<QQuickDataVisItem> d_ptr;

    friend class Q3DBars;
    friend class Q3DScatter;
    friend class Q3DSurface;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstract3DGraph::SelectionFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstract3DGraph::OptimizationHints)

QT_END_NAMESPACE

#endif

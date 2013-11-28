/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
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

#ifndef Q3DTHEME_H
#define Q3DTHEME_H

#include <QtDataVisualization/qdatavisualizationenums.h>
#include <QLinearGradient>
#include <QFont>
#include <QColor>

QT_DATAVISUALIZATION_BEGIN_NAMESPACE

class Q3DThemePrivate;

class QT_DATAVISUALIZATION_EXPORT Q3DTheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QtDataVisualization::QDataVis::Theme type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor NOTIFY baseColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor windowColor READ windowColor WRITE setWindowColor NOTIFY windowColorChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    Q_PROPERTY(QColor textBackgroundColor READ textBackgroundColor WRITE setTextBackgroundColor NOTIFY textBackgroundColorChanged)
    Q_PROPERTY(QColor gridLineColor READ gridLineColor WRITE setGridLineColor NOTIFY gridLineColorChanged)
    Q_PROPERTY(QColor singleHighlightColor READ singleHighlightColor WRITE setSingleHighlightColor NOTIFY singleHighlightColorChanged)
    Q_PROPERTY(QColor multiHighlightColor READ multiHighlightColor WRITE setMultiHighlightColor NOTIFY multiHighlightColorChanged)
    Q_PROPERTY(QColor lightColor READ lightColor WRITE setLightColor NOTIFY lightColorChanged) // TODO: Not used yet
    Q_PROPERTY(QLinearGradient baseGradient READ baseGradient WRITE setBaseGradient NOTIFY baseGradientChanged)
    Q_PROPERTY(QLinearGradient singleHighlightGradient READ singleHighlightGradient WRITE setSingleHighlightGradient NOTIFY singleHighlightGradientChanged)
    Q_PROPERTY(QLinearGradient multiHighlightGradient READ multiHighlightGradient WRITE setMultiHighlightGradient NOTIFY multiHighlightGradientChanged)
    Q_PROPERTY(float lightStrength READ lightStrength WRITE setLightStrength NOTIFY lightStrengthChanged)
    Q_PROPERTY(float ambientLightStrength READ ambientLightStrength WRITE setAmbientLightStrength NOTIFY ambientLightStrengthChanged)
    Q_PROPERTY(float highlightLightStrength READ highlightLightStrength WRITE setHighlightLightStrength NOTIFY highlightLightStrengthChanged)
    Q_PROPERTY(bool labelBorderEnabled READ isLabelBorderEnabled WRITE setLabelBorderEnabled NOTIFY labelBorderEnabledChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(bool backgroundEnabled READ isBackgroundEnabled WRITE setBackgroundEnabled NOTIFY backgroundEnabledChanged)
    Q_PROPERTY(bool gridEnabled READ isGridEnabled WRITE setGridEnabled NOTIFY gridEnabledChanged)
    Q_PROPERTY(bool labelBackgroundEnabled READ isLabelBackgroundEnabled WRITE setLabelBackgroundEnabled NOTIFY labelBackgroundEnabledChanged)
    Q_PROPERTY(QtDataVisualization::QDataVis::ColorStyle colorStyle READ colorStyle WRITE setColorStyle NOTIFY colorStyleChanged)
    // TODO: Add everything that we need

public:
    explicit Q3DTheme(QObject *parent = 0);
    explicit Q3DTheme(QDataVis::Theme themeType, QObject *parent = 0);
    virtual ~Q3DTheme();

    void setType(QDataVis::Theme themeType);
    QDataVis::Theme type() const;

    void setBaseColor(const QColor &color);
    QColor baseColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setWindowColor(const QColor &color);
    QColor windowColor() const;

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void setTextBackgroundColor(const QColor &color);
    QColor textBackgroundColor() const;

    void setGridLineColor(const QColor &color);
    QColor gridLineColor() const;

    void setSingleHighlightColor(const QColor &color);
    QColor singleHighlightColor() const;

    void setMultiHighlightColor(const QColor &color);
    QColor multiHighlightColor() const;

    void setLightColor(const QColor &color);
    QColor lightColor() const;

    void setBaseGradient(const QLinearGradient &gradient);
    QLinearGradient baseGradient() const;

    void setSingleHighlightGradient(const QLinearGradient &gradient);
    QLinearGradient singleHighlightGradient() const;

    void setMultiHighlightGradient(const QLinearGradient &gradient);
    QLinearGradient multiHighlightGradient() const;

    void setLightStrength(float strength);
    float lightStrength() const;

    void setAmbientLightStrength(float strength);
    float ambientLightStrength() const;

    void setHighlightLightStrength(float strength);
    float highlightLightStrength() const;

    void setLabelBorderEnabled(bool enabled);
    bool isLabelBorderEnabled() const;

    void setFont(const QFont &font);
    QFont font() const;

    void setBackgroundEnabled(bool enabled);
    bool isBackgroundEnabled() const;

    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;

    void setLabelBackgroundEnabled(bool enabled);
    bool isLabelBackgroundEnabled() const;

    void setColorStyle(QDataVis::ColorStyle style);
    QDataVis::ColorStyle colorStyle() const;

signals:
    void typeChanged(QDataVis::Theme themeType);
    void baseColorChanged(QColor color);
    void backgroundColorChanged(QColor color);
    void windowColorChanged(QColor color);
    void textColorChanged(QColor color);
    void textBackgroundColorChanged(QColor color);
    void gridLineColorChanged(QColor color);
    void singleHighlightColorChanged(QColor color);
    void multiHighlightColorChanged(QColor color);
    void lightColorChanged(QColor color);
    void baseGradientChanged(QLinearGradient gradient);
    void singleHighlightGradientChanged(QLinearGradient gradient);
    void multiHighlightGradientChanged(QLinearGradient gradient);
    void lightStrengthChanged(float strength);
    void ambientLightStrengthChanged(float strength);
    void highlightLightStrengthChanged(float strength);
    void labelBorderEnabledChanged(bool enabled);
    void fontChanged(QFont font);
    void backgroundEnabledChanged(bool enabled);
    void gridEnabledChanged(bool enabled);
    void labelBackgroundEnabledChanged(bool enabled);
    void colorStyleChanged(QDataVis::ColorStyle style);

protected:
    explicit Q3DTheme(Q3DThemePrivate *d,
                      QDataVis::Theme themeType,
                      QObject *parent = 0);
    QScopedPointer<Q3DThemePrivate> d_ptr;

    friend class ThemeManager;

private:
    Q_DISABLE_COPY(Q3DTheme)
};

QT_DATAVISUALIZATION_END_NAMESPACE

#endif

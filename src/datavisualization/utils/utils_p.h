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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef UTILS_P_H
#define UTILS_P_H

#include "datavisualizationglobal_p.h"

class QVector3D;
class QColor;
class QPainter;
class QString;
class QPoint;
class QImage;
class QLinearGradient;

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

class Utils
{
public:
    enum ParamType {
        ParamTypeUnknown = 0,
        ParamTypeInt,
        ParamTypeUInt,
        ParamTypeReal
    };

    static GLuint getNearestPowerOfTwo(GLuint value, GLuint &padding);
    static QVector3D vectorFromColor(const QColor &color);
    static QColor colorFromVector(const QVector3D &colorVector);
    static void printText(QPainter *painter, const QString &text, const QSize &position,
                          bool absoluteCoords = true, float rotation = 0.0f, float scale = 1.0f);
    static QImage printTextToImage(const QFont &font,
                                   const QString &text,
                                   const QColor &bgrColor,
                                   const QColor &txtColor,
                                   bool labelBackground,
                                   bool borders = false,
                                   int maxLabelWidth = 0);
    static QVector4D getSelection(QPoint mousepos, int height);
    static QImage getGradientImage(const QLinearGradient &gradient);

    static ParamType findFormatParamType(const QString &format);
    static QString formatLabel(const QByteArray &format, ParamType paramType, qreal value);
    static QString defaultLabelFormat();

    static float wrapValue(float value, float min, float max);

private:
    static ParamType mapFormatCharToParamType(const QChar &formatChar);
};

QT_END_NAMESPACE_DATAVISUALIZATION

#endif

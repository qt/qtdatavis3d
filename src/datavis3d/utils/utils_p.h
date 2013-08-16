/****************************************************************************
**
** Copyright (C) 2013 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVis3D module.
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
// This file is not part of the QtDataVis3D API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef UTILS_P_H
#define UTILS_P_H

#include "datavis3dglobal_p.h"
#include "q3dbars.h"

class QVector3D;
class QColor;
class QPainter;
class QString;
class QPoint;
class QImage;

QT_DATAVIS3D_BEGIN_NAMESPACE

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
    static void printText(QPainter *painter, const QString &text, const QSize &position,
                          bool absoluteCoords = true, qreal rotation = 0, qreal scale = 1.0f);
    static QImage printTextToImage(const QFont &font,
                                   const QString &text,
                                   const QColor &bgrColor,
                                   const QColor &txtColor,
                                   QDataVis::LabelTransparency transparency);
    static QVector3D getSelection(QPoint mousepos, int height);

    static ParamType findFormatParamType(const QString &format);
    static QString formatLabel(const QByteArray &format, ParamType paramType, qreal value);

private:
    static ParamType mapFormatCharToParamType(const QChar &formatChar);
};

QT_DATAVIS3D_END_NAMESPACE

#endif

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

#include "utils_p.h"

#include <QtGui/QPainter>

QT_BEGIN_NAMESPACE_DATAVISUALIZATION

#define NUM_IN_POWER(y, x) for (;y<x;y<<=1)
#define MIN_POWER 2

GLuint Utils::getNearestPowerOfTwo(GLuint value, GLuint &padding)
{
    GLuint powOfTwoValue = MIN_POWER;
    NUM_IN_POWER(powOfTwoValue, value);
    padding = powOfTwoValue - value;
    return powOfTwoValue;
}

QVector4D Utils::vectorFromColor(const QColor &color)
{
    return QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

QColor Utils::colorFromVector(const QVector3D &colorVector)
{
    return QColor(colorVector.x() * 255.0f, colorVector.y() * 255.0f,
                  colorVector.z() * 255.0f, 255.0f);
}

QColor Utils::colorFromVector(const QVector4D &colorVector)
{
    return QColor(colorVector.x() * 255.0f, colorVector.y() * 255.0f,
                  colorVector.z() * 255.0f, colorVector.w() * 255.0f);
}

QImage Utils::printTextToImage(const QFont &font, const QString &text, const QColor &bgrColor,
                               const QColor &txtColor, bool labelBackground,
                               bool borders, int maxLabelWidth)
{
    GLuint paddingWidth = 20;
    GLuint paddingHeight = 20;
    // Calculate text dimensions
    QFont valueFont = font;
    valueFont.setPointSize(textureFontSize);
    QFontMetrics valueFM(valueFont);
    int valueStrWidth = valueFM.width(text);
    if (maxLabelWidth && labelBackground)
        valueStrWidth = maxLabelWidth;
    int valueStrHeight = valueFM.height();
    valueStrWidth += paddingWidth / 2; // Fix clipping problem with skewed fonts (italic or italic-style)
    QSize labelSize;

#if defined(QT_OPENGL_ES_2)
    // ES2 can't handle textures with dimensions not in power of 2. Resize labels accordingly.
    // Add some padding before converting to power of two to avoid too tight fit
    GLuint prePadding = 5;
    // ES2 needs to use this always (when given) because of the power of 2 -issue.
    if (maxLabelWidth)
        valueStrWidth = maxLabelWidth + paddingWidth / 2;
    labelSize = QSize(valueStrWidth + prePadding, valueStrHeight + prePadding);
    labelSize.setWidth(getNearestPowerOfTwo(labelSize.width(), paddingWidth));
    labelSize.setHeight(getNearestPowerOfTwo(labelSize.height(), paddingHeight));
#else
    if (!labelBackground)
        labelSize = QSize(valueStrWidth, valueStrHeight);
    else
        labelSize = QSize(valueStrWidth + paddingWidth * 2, valueStrHeight + paddingHeight * 2);
#endif

    // Create image
    QImage image = QImage(labelSize, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    // Init painter
    QPainter painter(&image);
    // Paint text
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setFont(valueFont);
    if (!labelBackground) {
        painter.setPen(txtColor);
#if defined(QT_OPENGL_ES_2)
        painter.drawText((labelSize.width() - valueStrWidth) / 2.0f,
                         (labelSize.height() - valueStrHeight) / 2.0f,
                         valueStrWidth, valueStrHeight,
                         Qt::AlignCenter | Qt::AlignVCenter,
                         text);
#else
        painter.drawText(0, 0,
                         valueStrWidth, valueStrHeight,
                         Qt::AlignCenter | Qt::AlignVCenter,
                         text);
#endif
    } else {
        painter.setBrush(QBrush(bgrColor));
        if (borders) {
            painter.setPen(QPen(QBrush(txtColor), 5, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
            painter.drawRoundedRect(5, 5, labelSize.width() - 10, labelSize.height() - 10,
                                    10.0, 10.0);
        } else {
            painter.setPen(bgrColor);
            painter.drawRoundedRect(0, 0, labelSize.width(), labelSize.height(), 10.0, 10.0);
        }
        painter.setPen(txtColor);
        painter.drawText((labelSize.width() - valueStrWidth) / 2.0f,
                         (labelSize.height() - valueStrHeight) / 2.0f,
                         valueStrWidth, valueStrHeight,
                         Qt::AlignCenter | Qt::AlignVCenter,
                         text);
    }
    return image;
}

QVector4D Utils::getSelection(QPoint mousepos, int height)
{
    // This is the only one that works with OpenGL ES 2.0, so we're forced to use it
    // Item count will be limited to 256*256*256
    GLubyte pixel[4] = {255, 255, 255, 255};
    glReadPixels(mousepos.x(), height - mousepos.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                 (void *)pixel);
    QVector4D selectedColor(pixel[0], pixel[1], pixel[2], pixel[3]);
    return selectedColor;
}

QImage Utils::getGradientImage(QLinearGradient &gradient)
{
    QImage image(QSize(gradientTextureWidth, gradientTextureHeight), QImage::Format_RGB32);
    gradient.setFinalStop(qreal(gradientTextureWidth), qreal(gradientTextureHeight));
    gradient.setStart(0.0, 0.0);

    QPainter pmp(&image);
    pmp.setBrush(QBrush(gradient));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, int(gradientTextureWidth), int(gradientTextureHeight));
    return image;
}

Utils::ParamType Utils::mapFormatCharToParamType(const QChar &formatChar)
{
    ParamType retVal = ParamTypeUnknown;
    if (formatChar == QLatin1Char('d')
            || formatChar == QLatin1Char('i')
            || formatChar == QLatin1Char('c')) {
        retVal = ParamTypeInt;
    } else if (formatChar == QLatin1Char('u')
               || formatChar == QLatin1Char('o')
               || formatChar == QLatin1Char('x')
               || formatChar == QLatin1Char('X')) {
        retVal = ParamTypeUInt;
    } else if (formatChar == QLatin1Char('f')
               || formatChar == QLatin1Char('F')
               || formatChar == QLatin1Char('e')
               || formatChar == QLatin1Char('E')
               || formatChar == QLatin1Char('g')
               || formatChar == QLatin1Char('G')) {
        retVal = ParamTypeReal;
    }

    return retVal;
}

Utils::ParamType Utils::findFormatParamType(const QString &format)
{
    static QRegExp formatMatcher(QStringLiteral("%[\\-\\+#\\s\\d\\.lhjztL]*([dicuoxfegXFEG])"));

    if (formatMatcher.indexIn(format, 0) != -1) {
        QString capStr = formatMatcher.cap(1);
        if (capStr.isEmpty())
            return ParamTypeUnknown;
        else
            return mapFormatCharToParamType(capStr.at(0));
    }

    return ParamTypeUnknown;
}

QString Utils::formatLabel(const QByteArray &format, ParamType paramType, qreal value)
{
    switch (paramType) {
    case ParamTypeInt:
        return QString().sprintf(format, (qint64)value);
    case ParamTypeUInt:
        return QString().sprintf(format, (quint64)value);
    case ParamTypeReal:
        return QString().sprintf(format, value);
    default:
        return QString::fromUtf8(format); // To detect errors
    }
}

QString Utils::defaultLabelFormat()
{
    static const QString defaultFormat(QStringLiteral("%.2f"));
    return defaultFormat;
}

float Utils::wrapValue(float value, float min, float max)
{
    if (value > max) {
        value = min + (value - max);

        // In case single wrap fails, jump to opposite end.
        if (value > max)
            value = min;
    }

    if (value < min) {
        value = max + (value - min);

        // In case single wrap fails, jump to opposite end.
        if (value < min)
            value = max;
    }

    return value;
}

QQuaternion Utils::calculateRotation(const QVector3D &xyzRotations)
{
    QQuaternion rotQuatX = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, xyzRotations.x());
    QQuaternion rotQuatY = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, xyzRotations.y());
    QQuaternion rotQuatZ = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, xyzRotations.z());
    QQuaternion totalRotation = rotQuatY * rotQuatZ * rotQuatX;
    return totalRotation;
}

QT_END_NAMESPACE_DATAVISUALIZATION

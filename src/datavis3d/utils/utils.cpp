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

#include "utils_p.h"

#include <QVector3D>
#include <QColor>
#include <QPainter>
#include <QPoint>
#include <QImage>
#include <QRegExp>
#include <qmath.h>

QT_DATAVIS3D_BEGIN_NAMESPACE

#define NUM_IN_POWER(y, x) for (;y<x;y<<=1)
#define MIN_POWER 32

GLuint Utils::getNearestPowerOfTwo(GLuint value, GLuint &padding)
{
    GLuint powOfTwoValue = MIN_POWER;
    NUM_IN_POWER(powOfTwoValue, value);
    padding = powOfTwoValue - value;
    return powOfTwoValue;
}

QVector3D Utils::vectorFromColor(const QColor &color)
{
    return QVector3D(color.redF(), color.greenF(), color.blueF());
}

void Utils::printText(QPainter *painter, const QString &text, const QSize &position,
                      bool absoluteCoords, qreal rotation, qreal scale)
{
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setPen(Qt::black); // TODO: Use black, as nothing works
    QFont bgrFont = QFont(QStringLiteral("Arial"), 17);
    QFont valueFont = QFont(QStringLiteral("Arial"), 11);
    valueFont.setBold(true);
    painter->setFont(bgrFont);
    QFontMetrics valueFM(valueFont);
    QFontMetrics bgrFM(bgrFont);
    int valueStrLen = valueFM.width(text);
    int bgrStrLen = 0;
    int bgrHeight = valueFM.height() + 8;
    QString bgrStr = QString();
    do {
        bgrStr.append(QStringLiteral("I"));
        bgrStrLen = bgrFM.width(bgrStr);
    } while (bgrStrLen <= (valueStrLen + 8));
#if 0
    // Hack solution, as drawRect doesn't work
    painter->drawText(position.width() - (bgrStrLen / 2),
                      position.height() - bgrHeight,
                      bgrStrLen, bgrHeight,
                      Qt::AlignCenter | Qt::AlignVCenter,
                      bgrStr);
    //painter->setPen(d_ptr->m_textColor);
    painter->setPen(Qt::lightGray); // TODO: Use lightGray, as nothing works
    painter->setFont(valueFont);
    painter->drawText(position.width() - (valueStrLen / 2),
                      position.height() - bgrHeight,
                      valueStrLen, bgrHeight,
                      Qt::AlignCenter | Qt::AlignVCenter,
                      text);
#else
    //qDebug() << painter->window() << painter->viewport();
    painter->scale(scale, scale);
    if (absoluteCoords) {
        // This assumes absolute screen coordinates
        painter->translate(position.width() - (((float)bgrStrLen / 2.0f)
                                               * qCos(qDegreesToRadians(rotation)))
                           + (((float)bgrHeight / 2.0f) * qSin(qDegreesToRadians(rotation))),
                           position.height()
                           - ((((float)bgrHeight / 2.0f) * qCos(qDegreesToRadians(rotation)))
                              + (((float)bgrStrLen / 2.0f) * qSin(qDegreesToRadians(rotation)))));
    } else {
        // This calculates y as a distance from screen bottom
        painter->translate(position.width() - (((float)bgrStrLen / 2.0f)
                                               * qCos(qDegreesToRadians(rotation)))
                           + (((float)bgrHeight / 2.0f) * qSin(qDegreesToRadians(rotation))),
                           painter->window().height() - position.height()
                           - ((((float)bgrHeight / 2.0f) * qCos(qDegreesToRadians(rotation)))
                              + (((float)bgrStrLen / 2.0f) * qSin(qDegreesToRadians(rotation)))));
    }
    //qDebug() << painter->window().height() - position.height()
    //            - ((((float)bgrHeight / 2.0f) * qCos(qDegreesToRadians(rotation)))
    //               + (((float)bgrStrLen / 2.0f) * qSin(qDegreesToRadians(rotation))));
    painter->rotate(rotation);
    painter->drawText(0, 0,
                      bgrStrLen, bgrHeight,
                      Qt::AlignCenter | Qt::AlignVCenter,
                      bgrStr);
    painter->setPen(Qt::lightGray); // TODO: Use lightGray, as nothing works
    painter->setFont(valueFont);
    painter->drawText(6, 0,
                      valueStrLen, bgrHeight,
                      Qt::AlignCenter | Qt::AlignVCenter,
                      text);
    painter->resetTransform();
#endif
    painter->restore();
}

QImage Utils::printTextToImage(const QFont &font, const QString &text, const QColor &bgrColor,
                               const QColor &txtColor, QDataVis::LabelTransparency transparency)
{
    GLuint paddingWidth = 15;
    GLuint paddingHeight = 15;
    // Calculate text dimensions
    QFont valueFont = font;
    valueFont.setPointSize(30);
    QFontMetrics valueFM(valueFont);
    int valueStrWidth = valueFM.width(text);
    int valueStrHeight = valueFM.height();
    QSize labelSize;

#if defined(Q_OS_ANDROID)
    // Android can't handle textures with dimensions not in power of 2. Resize labels accordingly.
    // Add some padding before converting to power of two to avoid too tight fit
    GLuint prePadding = 10;
    labelSize = QSize(valueStrWidth + prePadding, valueStrHeight + prePadding);
    //qDebug() << "label size before padding" << labelSize;
    labelSize.setWidth(getNearestPowerOfTwo(labelSize.width(), paddingWidth));
    labelSize.setHeight(getNearestPowerOfTwo(labelSize.height(), paddingHeight));
    paddingWidth += prePadding;
    paddingHeight += prePadding;
    paddingWidth /= 2;
    paddingHeight /= 2;
    //qDebug() << "label size after padding" << labelSize << paddingWidth << paddingHeight;
#else
    if (QDataVis::TransparencyNoBackground == transparency)
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
    switch (transparency) {
    // TODO: Texture size padding fix for Android f**ks this up for axis labels. Fix or disable for android.
    case QDataVis::TransparencyNoBackground: {
        painter.setFont(valueFont);
        painter.setPen(txtColor);
        painter.drawText(0, 0,
                         valueStrWidth, valueStrHeight,
                         Qt::AlignCenter | Qt::AlignVCenter,
                         text);
        break;
    }
    case QDataVis::TransparencyFromTheme: {
        painter.setBrush(QBrush(bgrColor));
        painter.setPen(bgrColor);
        painter.drawRoundedRect(0, 0, labelSize.width(), labelSize.height(), 10.0, 10.0f);
        painter.setFont(valueFont);
        painter.setPen(txtColor);
        painter.drawText(paddingWidth, paddingHeight,
                         valueStrWidth, valueStrHeight,
                         Qt::AlignCenter | Qt::AlignVCenter,
                         text);
        break;
    }
    case QDataVis::TransparencyNone: {
        painter.setBrush(QBrush(bgrColor));
        painter.setPen(bgrColor);
        painter.drawRect(0, 0, labelSize.width(), labelSize.height());
        painter.setFont(valueFont);
        painter.setPen(txtColor);
        painter.drawText(paddingWidth, paddingHeight,
                         valueStrWidth, valueStrHeight,
                         Qt::AlignCenter | Qt::AlignVCenter,
                         text);
        break;
    }
    }
    return image;
}

QVector3D Utils::getSelection(QPoint mousepos, int height)
{
    QVector3D selectedColor;

    //#if defined(QT_OPENGL_ES_2)
    // This is the only one that works with ANGLE (ES 2.0)
    // Item count will be limited to 256*256*256
    GLubyte pixel[4];
    glReadPixels(mousepos.x(), height - mousepos.y(), 1, 1,
                 GL_RGBA, GL_UNSIGNED_BYTE, (void *)pixel);
    //qDebug() << "rgba" << pixel[0] << pixel[1] << pixel[2];// << pixel[3];
    //#else
    //// These work with desktop OpenGL
    //// They offer a lot higher possible object count and a possibility to use object ids
    //GLuint pixel[3];
    //glReadPixels(mousepos.x(), height - mousepos.y(), 1, 1,
    //             GL_RGB, GL_UNSIGNED_INT, (void *)pixel);
    //qDebug() << "rgba" << pixel[0] << pixel[1] << pixel[2];// << pixel[3];

    //GLfloat pixel3[3];
    //glReadPixels(mousepos.x(), height - mousepos.y(), 1, 1,
    //             GL_RGB, GL_FLOAT, (void *)pixel3);
    //qDebug() << "rgba" << pixel3[0] << pixel3[1] << pixel3[2];// << pixel[3];
    //#endif
    selectedColor = QVector3D(pixel[0], pixel[1], pixel[2]);
    //qDebug() << selectedColor;

    return selectedColor;
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
        return QString::number(value);
    }
}

QT_DATAVIS3D_END_NAMESPACE

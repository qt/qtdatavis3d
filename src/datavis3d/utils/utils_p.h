#ifndef UTILS_P_H
#define UTILS_P_H

#include "qdatavis3dglobal.h"

class QVector3D;
class QColor;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class Utils
{
public:
    static QVector3D vectorFromColor(const QColor &color);
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif

#include "utils_p.h"

#include <QVector3D>
#include <QColor>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

QVector3D Utils::vectorFromColor(const QColor &color)
{
    return QVector3D(color.redF(), color.greenF(), color.blueF());
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

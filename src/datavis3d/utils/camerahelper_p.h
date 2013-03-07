#ifndef CAMERAPOSITIONER_P_H
#define CAMERAPOSITIONER_P_H

#include "qdatavis3dglobal.h"

class QMatrix4x4;
class QVector3D;
class QPoint;
class QPointF;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class CameraHelper
{
public:
    // How fast camera rotates when mouse is dragged. Default is 100.
    static void setRotationSpeed(int speed);
    // Set camera rotation in degrees
    static void setCameraRotation(QPointF rotation);
    // Set default camera orientation. Position's x and y should be 0.
    static void setDefaultCameraOrientation(QVector3D defaultPosition
                                     , QVector3D defaultTarget
                                     , QVector3D defaultUp);
    static QMatrix4x4 calculateViewMatrix(QPoint mousePos, int zoom
                                          , int screenWidth, int screenHeight);
    static void updateMousePos(QPoint mousePos);
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif

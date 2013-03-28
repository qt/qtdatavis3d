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
    static void setCameraRotation(const QPointF &rotation);
    // Set default camera orientation. Position's x and y should be 0.
    static void setDefaultCameraOrientation(const QVector3D &defaultPosition
                                     , const QVector3D &defaultTarget
                                     , const QVector3D &defaultUp);
    // Calculate view matrix based on rotation and zoom
    static QMatrix4x4 calculateViewMatrix(const QPoint &mousePos, int zoom
                                          , int screenWidth, int screenHeight);
    // Calcluate light position based on rotation. Call after calling calculateViewMatrix to get
    // up-to-date position
    static QVector3D calculateLightPosition(const QVector3D &lightPosition);
    static void updateMousePos(const QPoint &mousePos);
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif

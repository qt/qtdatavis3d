#include "camerahelper_p.h"

#include <qmath.h>
#include <QMatrix4x4>
#include <QVector3D>

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

// Initial camera position
QVector3D m_position = QVector3D(0, 0.25, 3);
QVector3D m_target = QVector3D(0, 0, 0);
QVector3D m_up = QVector3D(0, 1, 0);

QPoint m_previousMousePos(0, 0);

float m_xRotation = 0;
float m_yRotation = 0;
float m_defaultXRotation = 0;
float m_defaultYRotation = 0;

float m_pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f;
float m_rotationSpeed = 100;

// FUNCTIONS
void CameraHelper::setRotationSpeed(int speed)
{
    // increase for faster rotation
    m_rotationSpeed = speed;
}

void CameraHelper::setCameraRotation(QPointF rotation)
{
    m_xRotation = rotation.x();
    m_defaultXRotation = m_xRotation;
    m_yRotation = rotation.y();
    m_defaultYRotation = m_yRotation;
}

void CameraHelper::setDefaultCameraOrientation(QVector3D defaultPosition
                                               , QVector3D defaultTarget
                                               , QVector3D defaultUp)
{
    m_position = defaultPosition;
    m_target = defaultTarget;
    m_up = defaultUp;
}

QMatrix4x4 CameraHelper::calculateViewMatrix(QPoint mousePos, int zoom
                                             , int screenWidth, int screenHeight)
{
    QMatrix4x4 viewMatrix;

    // Calculate mouse movement since last frame
    float mouseMoveX = float(m_previousMousePos.x() - mousePos.x())
            / (screenWidth / m_rotationSpeed);
    float mouseMoveY = float(m_previousMousePos.y() - mousePos.y())
            / (screenHeight / m_rotationSpeed);
    // Apply to rotations
    m_xRotation -= mouseMoveX;
    m_yRotation -= mouseMoveY;
    // Reset at 360 in x and limit to 0...90 in y
    if (qFabs(m_xRotation) >= 360)
        m_xRotation = 0;
    if (m_yRotation >= 90) {
        m_yRotation = 90;
    }
    else if (m_yRotation <= 0) {
        m_yRotation = 0;
    }

    // Apply to view matrix
    viewMatrix.lookAt(
                m_position                  // Camera is here
                , m_target                  // and looks here
                , m_up                      // Head is up (set to 0,-1,0 to look upside-down)
                );
    // Compensate for translation (if m_target is off origin)
    viewMatrix.translate(m_target.x(), m_target.y(), m_target.z());
    // Apply rotations
    // Handle x and z rotation when y -angle is other than 0
    viewMatrix.rotate(m_xRotation, 0, cos(m_yRotation*m_pi/180), sin(m_yRotation*m_pi/180));
    // y rotation is always "clean"
    viewMatrix.rotate(m_yRotation, 1.0f, 0, 0);
    // handle zoom by scaling
    viewMatrix.scale((float)zoom / 100.0f);
    // Compensate for translation (if m_target is off origin)
    viewMatrix.translate(-m_target.x(), -m_target.y(), -m_target.z());
    //qDebug() << m_xRotation << m_yRotation;

    //qDebug() << "sin(m_yRotation)" << sin(m_yRotation*m_pi/180);
    //qDebug() << "asin(m_yRotation)" << asin(m_yRotation*m_pi/180);
    //qDebug() << "cos(m_yRotation)" << cos(m_yRotation*m_pi/180);
    //qDebug() << "tan(m_yRotation)" << tan(m_yRotation*m_pi/180);

    m_previousMousePos = mousePos;
    return viewMatrix;
}

void CameraHelper::updateMousePos(QPoint mousePos)
{
    m_previousMousePos = mousePos;
    // if mouse position is set to (0, 0), reset rotations
    if (QPoint(0, 0) == mousePos) {
        m_xRotation = m_defaultXRotation;
        m_yRotation = m_defaultYRotation;
    }
}

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

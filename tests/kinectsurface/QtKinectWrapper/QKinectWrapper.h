/*
    QtKinectWrapper - Qt Kinect Wrapper Class
    Copyright (C) 2011-2013:
            Daniel Roggen, droggen@gmail.com

    All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
    Requirements:

        The QKinectWrapper class requires the following to be installed (versions are those tested and known to work):
            Windows:
                OpenNI-Win32-1.5.4.0-Dev.msi
                SensorKinect093-Bin-Win32-v5.1.2.1.msi (Note: this is PrimeSense Sensor KinectMod 5.1.2.1)
                NITE-Win32-1.5.2.21-Dev.msi

            Other: not checked

    Basics:
        QKinectWrapper provides Qt-friendly wrapper to get body skeleton, as well as camera image and depth map from a Kinect.
        It provides around the logic to 1) initialize the kinect, 2) continuously read the Kinect sensor, including depth map, image frame, and skeleton, 3) perform the necessary calibration, 4) shutdown.
        In principle up to 15 bodies are supported (limited by the Kinect driver)
        QKinectWrapper creates a dedicated thread to read the data of the Kinect. The application is shielded from any thread nastiness:
            - Safe functions are available to get the data from the kinect (taking care of mutexing as required). This includes functions to read image, depth map, and get the body skeleton.
            - Signals are used to indicate to the application a change of state in QKinectWrapper (e.g. initialize, running, error, stop), and the availability of new data (i.e. a new frame/depth/skeleton from the camera)

    Howto:
        1) Instanciate QKinectWrapper
        2) Connect the 'dataNotification' and 'statusNotification' signals to your application
        3) Optionally, set some visualization parameters (e.g. setPen)
        4) Call 'start'
        5) In the data notification slot, get the data from the kinect with methods getImage, getDepth, getJoints
        6) When done, call stop
        Use the statusNotification to get notified of initialization error or change of status of the kinect reader thread
*/

#ifndef __QKINECTWRAPPER_H
#define __QKINECTWRAPPER_H

#include <QWidget>
#include <QPainter>
#include <QThread>
#include <QMutex>
#include <XnOpenNI.h>

#ifndef Q_MOC_RUN       // Fixes a MOC issue with Qt5 (Qt4.x ok) that fails to properly expand some macros. We skip the include in the MOC pass.
#include <XnCppWrapper.h>
#endif // Q_MOC_RUN

namespace QKinect
{
/**
  \brief Status of the body tracking
 **/
enum KinectStatus
{
    Idle = 0,
    Initializing = 1,
    OkRun = 2,
    ErrorStop = 3
};

class QKinectWrapper : public QThread
{
    Q_OBJECT
public:
    QKinectWrapper();
    virtual ~QKinectWrapper();
    void run();

    void start();
    void stop();

    bool isRunning();
    bool isStopped();

    QImage getDepth();

    void setMaxDist(unsigned int dist);

private:
    // <-- Data to export outside of the thread
    QImage imageDepth;
    // --> End data to export outside of the thread

    unsigned int m_maxDist;
    qreal m_depthComp;
    bool t_requeststop;
    QMutex mutex;
    volatile KinectStatus status; // 0: pre-init. 1: init. 2: ok-run. 3:error-stop
    xn::Context g_Context;
    xn::DepthGenerator g_DepthGenerator;

    QImage createDepthImage();

    bool initialize();

signals:
    void dataNotification();
    void statusNotification(QKinect::KinectStatus);
};

}  // End namespace

// Register types used in signal/slot mechanism
Q_DECLARE_METATYPE(QKinect::KinectStatus);

#endif // __QKINECTWRAPPER_H

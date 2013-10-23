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

#include "QKinectWrapper.h"
#include <qmath.h>
#include <QDebug>

#if defined(_MSC_VER)
#define isnan(x) _isnan(x)
#endif

namespace QKinect
{

/******************************************************************************
*******************************************************************************
* QKinectWrapper   QKinectWrapper   QKinectWrapper   QKinectWrapper   QKinectWrapper
*******************************************************************************
******************************************************************************/


/******************************************************************************
* PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC   PUBLIC
******************************************************************************/


/**
    \brief Constructor - does nothing with the kinect driver but inits some internal vars
**/
QKinectWrapper::QKinectWrapper()
{
    // Register types used in signal/slot mechanism
    qRegisterMetaType<QKinect::KinectStatus>( "QKinect::KinectStatus" );

    status = QKinect::Idle;
}

/**
    \brief Destructor - calls stop - which terminates the kinect-dedicated reading thread - blocks until terminated.
**/
QKinectWrapper::~QKinectWrapper()
{
    stop();
}
void QKinectWrapper::start()
{
    t_requeststop = false;
    status = QKinect::Idle;
    QThread::start();
}

/**
    \brief Request the the kinect-dedicated reading thread to terminate, and thus
    release all kinect handles.
    Sets requeststop, which makes the main reading loop terminate (thread ended) at
    some later time. Blocks until terminated.
**/
void QKinectWrapper::stop()
{
    t_requeststop=true;

    wait();
}

/**
    \brief Return the latest depth image
    Call this afer a dataNotification signal
**/
QImage QKinectWrapper::getDepth()
{
    QMutexLocker locker(&mutex);
    return imageDepth;
}

/**
    \brief Returns if running
    Runing comprises initializing and reading data (not running is idle or error)
    We query the thread state to get this.
**/
bool QKinectWrapper::isRunning()
{
    if (QThread::isRunning())
        return true;
    return false;

}

/**
    \brief Returns if stopped
    Runing comprises initializing and reading data (not running is idle or error)
    We query the thread state to get this.
**/
bool QKinectWrapper::isStopped()
{
    if (QThread::isRunning())
        return false;
    return true;

}

/******************************************************************************
* PRIVATE/PROTECTED   PRIVATE/PROTECTED   PRIVATE/PROTECTED   PRIVATE/PROTECTED
******************************************************************************/


/**
    \brief Starts the kinect-dedicated reading thread.
    Does the following:
    1. Notify the start of the initialization and initialize
    2. Notify the initialization outcomes (error, or running if initialization was successful)
    3. If successful, does a continous:
    3.1. Wait/update kinect data
    3.2. Protected by a mutex, generate the images and data structures, made available to the user
    3.3. Check if a stop has been requested
    4. Stop the kinect reading and release resources, notify.

    The variable accessed outside of this thread are protected by mutexes. This includes:
    - The status
    - The QImages for depth, camera
    - The bodies
    - etc.
**/
void QKinectWrapper::run()
{
    mutex.lock();
    status = QKinect::Initializing;
    emit statusNotification(status);
    mutex.unlock();

    bool ok = initialize();

    if (!ok) {
        mutex.lock();
        status = QKinect::ErrorStop;
        emit statusNotification(status);
        mutex.unlock();
        return;
    }

    mutex.lock();
    status = QKinect::OkRun;
    emit statusNotification(status);
    mutex.unlock();

    while (!t_requeststop) {
        (void)g_Context.WaitAndUpdateAll();
        // Prepare the data to export outside of the thread
        mutex.lock();
        xn::DepthMetaData depthMD;
        g_DepthGenerator.GetMetaData(depthMD);
        imageDepth = createDepthImage();
        emit dataNotification();
        mutex.unlock();
    }
    g_Context.Release();

    mutex.lock();
    status = QKinect::Idle;
    emit statusNotification(status);
    mutex.unlock();
}

/**
    \brief Setups and initializes the kinect/openni
    Returns false if any step fails.
**/
bool QKinectWrapper::initialize()
{
    XnStatus nRetVal = XN_STATUS_OK;
    XnMapOutputMode outputMode;

    // Initialize the context
    nRetVal = g_Context.Init();
    if (nRetVal != XN_STATUS_OK) {
        mutex.lock();
        qCritical() << QString("Context creation failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }

    // Create a depth generator
    nRetVal = g_DepthGenerator.Create(g_Context);
    if (nRetVal != XN_STATUS_OK) {
        mutex.lock();
        qCritical() << QString("Depth generator creation failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }

//    XnUInt32 nCount = g_DepthGenerator.GetSupportedMapOutputModesCount();
//    qDebug() << nCount;
//    XnMapOutputMode *aModes = new XnMapOutputMode[nCount];
//    g_DepthGenerator.GetSupportedMapOutputModes(aModes, nCount);

//    for (int i = 0; i < nCount; i++)
//        qDebug() << aModes[i].nFPS << aModes[i].nXRes << aModes[i].nYRes;

    outputMode.nXRes = 640;
    outputMode.nYRes = 480;
    outputMode.nFPS = 30;
    nRetVal = g_DepthGenerator.SetMapOutputMode(outputMode);
    if (nRetVal != XN_STATUS_OK) {
        mutex.lock();
        qCritical() << QString("Depth generator SetMapOutputMode failed: %1").arg(xnGetStatusString(nRetVal));
        mutex.unlock();
        return false;
    }

    // Start producting data
    nRetVal = g_Context.StartGeneratingAll();

    return true;
}

/**
  \brief Creates a QImage comprising the depth map
**/
QImage QKinectWrapper::createDepthImage()
{
    // Here must mutex / run also access the data
    xn::SceneMetaData smd;
    xn::DepthMetaData dmd;
    g_DepthGenerator.GetMetaData(dmd);

    XnUInt16 g_nXRes = dmd.XRes();
    XnUInt16 g_nYRes = dmd.YRes();

    QImage image(g_nXRes, g_nYRes, QImage::Format_RGB32);

    const XnDepthPixel* pDepth = dmd.Data();
    const XnLabel* pLabels = smd.Data();

    for (unsigned nY = 0; nY < g_nYRes; nY++) {
        uchar *imageptr = image.scanLine(nY);

        for (unsigned nX = 0; nX < g_nXRes; nX++) {
            unsigned depth = *pDepth;

            if (depth > m_maxDist)
                depth = m_maxDist;
            if (depth)
                depth = ((m_maxDist - depth) * m_depthComp / m_maxDist);

            // depth: 255: closest
            // depth: 0: furtherst (m_maxDist distance)
            imageptr[0] = depth;
            imageptr[1] = depth;
            imageptr[2] = depth;
            imageptr[3] = 0xff;
            pDepth++;
            imageptr += 4;
            pLabels++;
        }
    }

    return image;
}

// Adjust depth distance
void QKinectWrapper::setMaxDist(unsigned int dist)
{
    m_maxDist = dist;
    m_depthComp = 255;
    if (m_maxDist < 10000)
        m_depthComp = 255.0 * qSqrt(qSqrt(qSqrt(10000.0 / qreal(m_maxDist)))); // compensate for the adjusted depth scale
}
}     // End namespace





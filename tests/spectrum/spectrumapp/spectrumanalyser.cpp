/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#include "spectrumanalyser.h"
#include "utils.h"
#include "fftreal_wrapper.h"

#include <qmath.h>
#include <qmetatype.h>
#include <QAudioFormat>
#include <QThread>

SpectrumAnalyserThread::SpectrumAnalyserThread(QObject *parent)
    : QObject(parent),
      m_fft(new FFTRealWrapper),
      m_numSamples(SpectrumLengthSamples),
      m_windowFunction(DefaultWindowFunction),
      m_window(SpectrumLengthSamples, 0.0),
      m_input(SpectrumLengthSamples, 0.0),
      m_output(SpectrumLengthSamples, 0.0),
      m_spectrum(SpectrumLengthSamples)
    #ifdef SPECTRUM_ANALYSER_SEPARATE_THREAD
    , m_thread(new QThread(this))
    #endif
{
#ifdef SPECTRUM_ANALYSER_SEPARATE_THREAD
    // moveToThread() cannot be called on a QObject with a parent
    setParent(0);
    moveToThread(m_thread);
    m_thread->start();
#endif
    calculateWindow();
}

SpectrumAnalyserThread::~SpectrumAnalyserThread()
{
    delete m_fft;
}

void SpectrumAnalyserThread::setWindowFunction(WindowFunction type)
{
    m_windowFunction = type;
    calculateWindow();
}

void SpectrumAnalyserThread::calculateWindow()
{
    for (int i=0; i<m_numSamples; ++i) {
        DataType x = 0.0;

        switch (m_windowFunction) {
        case NoWindow:
            x = 1.0;
            break;
        case HannWindow:
            x = 0.5 * (1 - qCos((2 * M_PI * i) / (m_numSamples - 1)));
            break;
        default:
            Q_ASSERT(false);
        }

        m_window[i] = x;
    }
}

void SpectrumAnalyserThread::calculateSpectrum(const QByteArray &buffer,
                                               int inputFrequency,
                                               int bytesPerSample)
{
    Q_ASSERT(buffer.size() == m_numSamples * bytesPerSample);

    // Initialize data array
    const char *ptr = buffer.constData();
    for (int i=0; i<m_numSamples; ++i) {
        const qint16 pcmSample = *reinterpret_cast<const qint16*>(ptr);
        // Scale down to range [-1.0, 1.0]
        const DataType realSample = pcmToReal(pcmSample);
        const DataType windowedSample = realSample * m_window[i];
        m_input[i] = windowedSample;
        ptr += bytesPerSample;
    }

    // Calculate the FFT
    m_fft->calculateFFT(m_output.data(), m_input.data());

    // Analyze output to obtain amplitude and phase for each frequency
    for (int i=2; i<=m_numSamples/2; ++i) {
        // Calculate frequency of this complex sample
        m_spectrum[i].frequency = qreal(i * inputFrequency) / (m_numSamples);

        const qreal real = m_output[i];
        qreal imag = 0.0;
        if (i>0 && i<m_numSamples/2)
            imag = m_output[m_numSamples/2 + i];

        const qreal magnitude = sqrt(real*real + imag*imag);
        qreal amplitude = SpectrumAnalyserMultiplier * log(magnitude);

        // Bound amplitude to [0.0, 1.0]
        m_spectrum[i].clipped = (amplitude > 1.0);
        amplitude = qMax(qreal(0.0), amplitude);
        amplitude = qMin(qreal(1.0), amplitude);
        m_spectrum[i].amplitude = amplitude;
    }

    emit calculationComplete(m_spectrum);
}


//=============================================================================
// SpectrumAnalyser
//=============================================================================

SpectrumAnalyser::SpectrumAnalyser(QObject *parent)
    : QObject(parent),
      m_thread(new SpectrumAnalyserThread(this)),
      m_state(Idle)
{
    CHECKED_CONNECT(m_thread, SIGNAL(calculationComplete(FrequencySpectrum)),
                    this, SLOT(calculationComplete(FrequencySpectrum)));
}

SpectrumAnalyser::~SpectrumAnalyser()
{

}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

void SpectrumAnalyser::setWindowFunction(WindowFunction type)
{
    const bool b = QMetaObject::invokeMethod(m_thread, "setWindowFunction",
                                             Qt::AutoConnection,
                                             Q_ARG(WindowFunction, type));
    Q_ASSERT(b);
    Q_UNUSED(b) // suppress warnings in release builds
}

void SpectrumAnalyser::calculate(const QByteArray &buffer,
                                 const QAudioFormat &format)
{
    // QThread::currentThread is marked 'for internal use only', but
    // we're only using it for debug output here, so it's probably OK :)
    SPECTRUMANALYSER_DEBUG << "SpectrumAnalyser::calculate"
                           << QThread::currentThread()
                           << "state" << m_state;

    if (isReady()) {
        Q_ASSERT(isPCMS16LE(format));

        const int bytesPerSample = format.sampleSize() * format.channelCount() / 8;

        m_state = Busy;

        // Invoke SpectrumAnalyserThread::calculateSpectrum using QMetaObject.  If
        // m_thread is in a different thread from the current thread, the
        // calculation will be done in the child thread.
        // Once the calculation is finished, a calculationChanged signal will be
        // emitted by m_thread.
        const bool b = QMetaObject::invokeMethod(m_thread, "calculateSpectrum",
                                                 Qt::AutoConnection,
                                                 Q_ARG(QByteArray, buffer),
                                                 Q_ARG(int, format.sampleRate()),
                                                 Q_ARG(int, bytesPerSample));
        Q_ASSERT(b);
        Q_UNUSED(b) // suppress warnings in release builds
    }
}

bool SpectrumAnalyser::isReady() const
{
    return (Idle == m_state);
}

void SpectrumAnalyser::cancelCalculation()
{
    if (Busy == m_state)
        m_state = Cancelled;
}


//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void SpectrumAnalyser::calculationComplete(const FrequencySpectrum &spectrum)
{
    Q_ASSERT(Idle != m_state);
    if (Busy == m_state)
        emit spectrumChanged(spectrum);
    m_state = Idle;
}

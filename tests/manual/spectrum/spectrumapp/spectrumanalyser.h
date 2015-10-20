/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include <QByteArray>
#include <QObject>
#include <QVector>

#include "frequencyspectrum.h"
#include "spectrum.h"

#include "FFTRealFixLenParam.h"

QT_FORWARD_DECLARE_CLASS(QAudioFormat)
QT_FORWARD_DECLARE_CLASS(QThread)

class FFTRealWrapper;

class SpectrumAnalyserThreadPrivate;

/**
 * Implementation of the spectrum analysis which can be run in a
 * separate thread.
 */
class SpectrumAnalyserThread : public QObject
{
    Q_OBJECT

public:
    SpectrumAnalyserThread(QObject *parent);
    ~SpectrumAnalyserThread();

public Q_SLOTS:
    void setWindowFunction(WindowFunction type);
    void calculateSpectrum(const QByteArray &buffer,
                           int inputFrequency,
                           int bytesPerSample);

Q_SIGNALS:
    void calculationComplete(const FrequencySpectrum &spectrum);

private:
    void calculateWindow();

private:
    FFTRealWrapper*                             m_fft;

    const int                                   m_numSamples;

    WindowFunction                              m_windowFunction;

    typedef FFTRealFixLenParam::DataType        DataType;

    QVector<DataType>                           m_window;

    QVector<DataType>                           m_input;
    QVector<DataType>                           m_output;

    FrequencySpectrum                           m_spectrum;

#ifdef SPECTRUM_ANALYSER_SEPARATE_THREAD
    QThread*                                    m_thread;
#endif
};

/**
 * Class which performs frequency spectrum analysis on a window of
 * audio samples, provided to it by the Engine.
 */
class SpectrumAnalyser : public QObject
{
    Q_OBJECT

public:
    SpectrumAnalyser(QObject *parent = 0);
    ~SpectrumAnalyser();

public:
    /*
     * Set the windowing function which is applied before calculating the FFT
     */
    void setWindowFunction(WindowFunction type);

    /*
     * Calculate a frequency spectrum
     *
     * \param buffer       Audio data
     * \param format       Format of audio data
     *
     * Frequency spectrum is calculated asynchronously.  The result is returned
     * via the spectrumChanged signal.
     *
     * An ongoing calculation can be cancelled by calling cancelCalculation().
     *
     */
    void calculate(const QByteArray &buffer, const QAudioFormat &format);

    /*
     * Check whether the object is ready to perform another calculation
     */
    bool isReady() const;

    /*
     * Cancel an ongoing calculation
     *
     * Note that cancelling is asynchronous.
     */
    void cancelCalculation();

Q_SIGNALS:
    void spectrumChanged(const FrequencySpectrum &spectrum);

private Q_SLOTS:
    void calculationComplete(const FrequencySpectrum &spectrum);

private:
    void calculateWindow();

private:

    SpectrumAnalyserThread*    m_thread;

    enum State {
        Idle,
        Busy,
        Cancelled
    };

    State              m_state;
};

#endif // SPECTRUMANALYSER_H


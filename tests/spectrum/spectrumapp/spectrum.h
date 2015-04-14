/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd
** All rights reserved.
** For any questions to The Qt Company, please use contact form at http://qt.io
**
** This file is part of the Qt Data Visualization module.
**
** Licensees holding valid commercial license for Qt may use this file in
** accordance with the Qt License Agreement provided with the Software
** or, alternatively, in accordance with the terms contained in a written
** agreement between you and The Qt Company.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.io
**
****************************************************************************/

#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "utils.h"
#include "fftreal_wrapper.h" // For FFTLengthPowerOfTwo
#include <qglobal.h>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// Number of audio samples used to calculate the frequency spectrum
const int    SpectrumLengthSamples  = PowerOfTwo<FFTLengthPowerOfTwo>::Result;

// Number of bands in the frequency spectrum
const int    SpectrumNumBands       = 30;

// Lower bound of first band in the spectrum
const qreal  SpectrumLowFreq        = 0.0; // Hz

// Upper band of last band in the spectrum
const qreal  SpectrumHighFreq       = 1000.0; // Hz

// Waveform window size in microseconds
const qint64 WaveformWindowDuration = 500 * 1000;

// Fudge factor used to calculate the spectrum bar heights
const qreal SpectrumAnalyserMultiplier = 0.15;

// Disable message timeout
const int   NullMessageTimeout      = -1;


//-----------------------------------------------------------------------------
// Types and data structures
//-----------------------------------------------------------------------------

enum WindowFunction {
    NoWindow,
    HannWindow
};

const WindowFunction DefaultWindowFunction = HannWindow;

struct Tone
{
    Tone(qreal freq = 0.0, qreal amp = 0.0)
    :   frequency(freq), amplitude(amp)
    { }

    // Start and end frequencies for swept tone generation
    qreal   frequency;

    // Amplitude in range [0.0, 1.0]
    qreal   amplitude;
};

struct SweptTone
{
    SweptTone(qreal start = 0.0, qreal end = 0.0, qreal amp = 0.0)
    :   startFreq(start), endFreq(end), amplitude(amp)
    { Q_ASSERT(end >= start); }

    SweptTone(const Tone &tone)
    :   startFreq(tone.frequency), endFreq(tone.frequency), amplitude(tone.amplitude)
    { }

    // Start and end frequencies for swept tone generation
    qreal   startFreq;
    qreal   endFreq;

    // Amplitude in range [0.0, 1.0]
    qreal   amplitude;
};


//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

// Macro which connects a signal to a slot, and which causes application to
// abort if the connection fails.  This is intended to catch programming errors
// such as mis-typing a signal or slot name.  It is necessary to write our own
// macro to do this - the following idiom
//     Q_ASSERT(connect(source, signal, receiver, slot));
// will not work because Q_ASSERT compiles to a no-op in release builds.

#define CHECKED_CONNECT(source, signal, receiver, slot) \
    if (!connect(source, signal, receiver, slot)) \
        qt_assert_x(Q_FUNC_INFO, "CHECKED_CONNECT failed", __FILE__, __LINE__);

#endif // SPECTRUM_H


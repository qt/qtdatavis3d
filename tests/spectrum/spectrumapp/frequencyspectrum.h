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

#ifndef FREQUENCYSPECTRUM_H
#define FREQUENCYSPECTRUM_H

#include <QtCore/QVector>

/**
 * Represents a frequency spectrum as a series of elements, each of which
 * consists of a frequency, an amplitude and a phase.
 */
class FrequencySpectrum {
public:
    FrequencySpectrum(int numPoints = 0);

    struct Element {
        Element()
        :   frequency(0.0), amplitude(0.0), phase(0.0), clipped(false)
        { }

        /**
         * Frequency in Hertz
         */
        qreal frequency;

        /**
         * Amplitude in range [0.0, 1.0]
         */
        qreal amplitude;

        /**
         * Phase in range [0.0, 2*PI]
         */
        qreal phase;

        /**
         * Indicates whether value has been clipped during spectrum analysis
         */
        bool clipped;
    };

    typedef QVector<Element>::iterator iterator;
    typedef QVector<Element>::const_iterator const_iterator;

    void reset();

    int count() const;
    Element& operator[](int index);
    const Element& operator[](int index) const;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

private:
    QVector<Element> m_elements;

};

#endif // FREQUENCYSPECTRUM_H

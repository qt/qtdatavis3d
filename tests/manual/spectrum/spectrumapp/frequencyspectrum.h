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

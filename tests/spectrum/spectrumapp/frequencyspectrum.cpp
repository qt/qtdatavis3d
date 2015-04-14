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

#include "frequencyspectrum.h"

FrequencySpectrum::FrequencySpectrum(int numPoints)
    :   m_elements(numPoints)
{

}

void FrequencySpectrum::reset()
{
    iterator i = begin();
    for ( ; i != end(); ++i)
        *i = Element();
}

int FrequencySpectrum::count() const
{
    return m_elements.count();
}

FrequencySpectrum::Element &FrequencySpectrum::operator[](int index)
{
    return m_elements[index];
}

const FrequencySpectrum::Element &FrequencySpectrum::operator[](int index) const
{
    return m_elements[index];
}

FrequencySpectrum::iterator FrequencySpectrum::begin()
{
    return m_elements.begin();
}

FrequencySpectrum::iterator FrequencySpectrum::end()
{
    return m_elements.end();
}

FrequencySpectrum::const_iterator FrequencySpectrum::begin() const
{
    return m_elements.begin();
}

FrequencySpectrum::const_iterator FrequencySpectrum::end() const
{
    return m_elements.end();
}

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

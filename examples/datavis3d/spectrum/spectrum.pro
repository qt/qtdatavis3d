include(spectrum.pri)

TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG += ordered

SUBDIRS += 3rdparty/fftreal
SUBDIRS += spectrumapp

TARGET = spectrum


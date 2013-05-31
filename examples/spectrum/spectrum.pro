!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

qtHaveModule(multimedia) {
include(spectrum.pri)

TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG += ordered

SUBDIRS += 3rdparty/fftreal
SUBDIRS += spectrumapp

TARGET = spectrum
}

!include( ../tests.pri ) {
    error( "Couldn't find the tests.pri file!" )
}

QT += core gui graphs

TARGET = minimalSurface
TEMPLATE = app

SOURCES += ../../../src/graphs/doc/snippets/doc_src_q3dsurface_construction.cpp

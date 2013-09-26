!include( ../tests.pri ) {
    error( "Couldn't find the tests.pri file!" )
}

SOURCES += main.cpp \
    surfacedata.cpp
HEADERS += \
    surfacedata.h

QT += widgets

INSTALLS += target

# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
# <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
# QtKinectWrapper   QtKinectWrapper   QtKinectWrapper   QtKinectWrapper
# ##############################################################################
# ##############################################################################
# Kinect: START OF CONFIGURATION BELOW ->
#
# Copy these lines at the end of your the project file of your application (.pro )
# Check the lines indicated "Modify below" and adapt the path to:
# 1) QtKinectWrapper sources (i.e. where the QKinectWrapper.* files lie),
# 2) OpenNI path
# 3) Make sure the OpenNI, PrimeSense NITE, PrimeSense Kinect Sensor are all in your path
#
# -------------------------------- Modify below --------------------------------

# Set QTKINECTWRAPPER_SOURCE_PATH to point to the directory containing the QtKinectWrapper classes
QTKINECTWRAPPER_SOURCE_PATH = QtKinectWrapper

# Set OPENNI_LIBRARY_PATH to point to the directory where openNI is installed. Assumption: the import libraries are in $$OPENNI_LIBRARY_PATH/lib
# Multiple path can be specified (e.g. to handle x86 / x64 differences)
# Quotation marks with spaces
win32 {
OPENNI_LIBRARY_PATH = "C:/Program Files (x86)/OpenNI"
OPENNI_LIBRARY_PATH += "C:/Program Files/OpenNI"
}

# ------------------------------ Copy as-is below ------------------------------
# ##############################################################################
# ##############################################################################
# QtKinectWrapper   QtKinectWrapper   QtKinectWrapper   QtKinectWrapper
# ##############################################################################
# Do not modify: default settings
# ##############################################################################
# Path for Qt wrapper
INCLUDEPATH += $$QTKINECTWRAPPER_SOURCE_PATH
# Sources for Qt wrapper
SOURCES += $$QTKINECTWRAPPER_SOURCE_PATH/QKinectWrapper.cpp
HEADERS += $$QTKINECTWRAPPER_SOURCE_PATH/QKinectWrapper.h



win32 {
	# Set the path to the patched openni/include
	INCLUDEPATH += $$QTKINECTWRAPPER_SOURCE_PATH/OpenNI/Include
	# Set the path to the Qt wrapper
	INCLUDEPATH += $$QTKINECTWRAPPER_SOURCE_PATH

	# Iterate through
	for(l, OPENNI_LIBRARY_PATH):LIBS+=-L$$l/lib
	#LIBS += -L$$OPENNI_LIBRARY_PATH/lib
	LIBS += -lopenNI
}

# ##############################################################################
# ##############################################################################
# QtKinectWrapper   QtKinectWrapper   QtKinectWrapper   QtKinectWrapper
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# The following macros allow certain features and debugging output
# to be disabled / enabled at compile time.

# Debug output from spectrum calculation
#DEFINES += LOG_SPECTRUMANALYSER

# Debug output from engine
#DEFINES += LOG_ENGINE

# Disables rendering of the waveform
DEFINES += DISABLE_WAVEFORM

# Perform spectrum analysis calculation in a separate thread
DEFINES += SPECTRUM_ANALYSER_SEPARATE_THREAD

# Suppress warnings about strncpy potentially being unsafe, emitted by MSVC
win32: DEFINES += _CRT_SECURE_NO_WARNINGS


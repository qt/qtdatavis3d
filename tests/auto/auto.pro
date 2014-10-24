TEMPLATE = subdirs

SUBDIRS += cpptest

qtHaveModule(quick): SUBDIRS += qmltest

installed_cmake.depends = cmake

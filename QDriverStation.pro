#
# Copyright (c) 2015-2021 Alex Spataru <alex_spataru@outlook.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

#-----------------------------------------------------------------------------------------
# Make options
#-----------------------------------------------------------------------------------------

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

CONFIG += c++11

isEmpty(PREFIX) {
    PREFIX = /usr
}

#-------------------------------------------------------------------------------
# Deploy configuration
#-------------------------------------------------------------------------------

TEMPLATE = app
TARGET = QDriverStation
CONFIG += resources_big
CONFIG += qtquickcompiler

QTPLUGIN += qsvg

QT += xml
QT += sql
QT += svg
QT += core
QT += quick
QT += widgets

#-------------------------------------------------------------------------------
# Compiler options
#-------------------------------------------------------------------------------

*g++*: {
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE *= -O3
}

*msvc*: {
    QMAKE_CXXFLAGS_RELEASE -= /O
    QMAKE_CXXFLAGS_RELEASE *= /O2
}

#-------------------------------------------------------------------------------
# Deploy configuration
#-------------------------------------------------------------------------------

win32* {
    LIBS += -lPdh -lgdi32                                    # pthread + gdi
    RC_FILE = etc/deploy/windows/resources/info.rc           # Set applicaiton icon
    OTHER_FILES += etc/deploy/windows/nsis/setup.nsi         # Setup script
}

macx* {
    ICON = etc/deploy/macOS/icon.icns                        # icon file
    RC_FILE = etc/deploy/macOS/icon.icns                     # icon file
    QMAKE_INFO_PLIST = etc/deploy/macOS/info.plist           # Add info.plist file
    CONFIG += sdk_no_version_check                           # Avoid warnings with Big Sur
}

linux:!android {
    TARGET = qdriverstation
    target.path = $$PREFIX/bin
    icon.path = $$PREFIX/share/pixmaps                       # icon instalation path
    desktop.path = $$PREFIX/share/applications               # *.desktop instalation path
    icon.files += etc/deploy/linux/*.png                     # Add application icon
    desktop.files += etc/deploy/linux/*.desktop              # Add *.desktop file
    INSTALLS += target desktop icon                          # make install targets
}

#-------------------------------------------------------------------------------
# Include other libraries
#-------------------------------------------------------------------------------

include ($$PWD/lib/QJoysticks/QJoysticks.pri)
include ($$PWD/lib/LibDS/wrappers/Qt/LibDS-Qt.pri)

#-------------------------------------------------------------------------------
# Import source code and QML
#-------------------------------------------------------------------------------

SOURCES += \
  $$PWD/src/main.cpp \
  $$PWD/src/utilities.cpp \
  $$PWD/src/beeper.cpp \
  $$PWD/src/dashboards.cpp \
  $$PWD/src/shortcuts.cpp
  
HEADERS += \
  $$PWD/src/utilities.h \
  $$PWD/src/beeper.h \
  $$PWD/src/dashboards.h \
  $$PWD/src/versions.h \
  $$PWD/src/shortcuts.h
    
RESOURCES += \
  $$PWD/qml/qml.qrc \
  $$PWD/etc/resources/resources.qrc
             
OTHER_FILES += \
  $$PWD/qml/*.qml \
  $$PWD/qml/*.js \
  $$PWD/qml/Dialogs/*.qml \
  $$PWD/qml/Widgets/*.qml \
  $$PWD/qml/MainWindow/*.qml

#-------------------------------------------------------------------------------
# Deploy files
#-------------------------------------------------------------------------------

OTHER_FILES += \
    deploy/linux/* \
    deploy/macOS/* \
    deploy/windows/nsis/* \
    deploy/windows/resources/*

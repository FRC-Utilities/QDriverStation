#
# Copyright (c) 2015-2016 Alex Spataru <alex_spataru@outlook.com>
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

#-------------------------------------------------------------------------------
# Deploy configuration
#-------------------------------------------------------------------------------

CONFIG += app
CONFIG += qtquickcompiler

TARGET = QDriverStation

QT += qml
QT += quick

win32* {
    LIBS += -lPdh -lgdi32
    RC_FILE = $$PWD/etc/deploy/windows/resources/info.rc
}

macx* {
    ICON = $$PWD/etc/deploy/mac-osx/icon.icns
    RC_FILE = $$PWD/etc/deploy/mac-osx/icon.icns
    QMAKE_INFO_PLIST = $$PWD/etc/deploy/mac-osx/info.plist
}

linux:!android {
    target.path = /usr/bin
    icon.path = /usr/share/pixmaps
    desktop.path = /usr/share/applications
    icon.files += $$PWD/etc/deploy/linux/common/qdriverstation.png
    desktop.files += $$PWD/etc/deploy/linux/common/qdriverstation.desktop

    TARGET = qdriverstation
    INSTALLS += target desktop icon
}

#-------------------------------------------------------------------------------
# Make options
#-------------------------------------------------------------------------------

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

#-------------------------------------------------------------------------------
# Compiler flags
#-------------------------------------------------------------------------------

# Uncomment to break the build and play around with ASM code :)
# QMAKE_CXXFLAGS *= -S

#-------------------------------------------------------------------------------
# Include other libraries
#-------------------------------------------------------------------------------

include ($$PWD/lib/QJoysticks/QJoysticks.pri)
include ($$PWD/lib/LibDS/wrappers/Qt/LibDS-Qt.pri)
include ($$PWD/lib/QSimpleUpdater/QSimpleUpdater.pri)

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
             
OTHER_FILES += $$PWD/qml/*.qml
OTHER_FILES += $$PWD/qml/*.js
OTHER_FILES += $$PWD/qml/Dialogs/*.qml
OTHER_FILES += $$PWD/qml/Widgets/*.qml
OTHER_FILES += $$PWD/qml/MainWindow/*.qml

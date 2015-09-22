#
# This file is part of Open DriverStation
#
# Copyright (c) 2015 WinT 3794 <http:/wint3794.org>
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
#

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------

include ($$PWD/lib/Falcon/Falcon.pri)
include ($$PWD/lib/LibDS/DriverStation.pri)

#-------------------------------------------------------------------------------
# Qt/C++ configuration
#-------------------------------------------------------------------------------

CODECFORTR = UTF-8
CODECFORSRC = UTF-8
TARGET = QDriverStation

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
CONFIG += c++11

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

#-------------------------------------------------------------------------------
# Import source code and resources of the application
#-------------------------------------------------------------------------------

SOURCES += \
    $$PWD/src/mobile/main.cpp \
    $$PWD/src/mobile/Settings.cpp

HEADERS += $$PWD/src/mobile/Settings.h

RESOURCES += $$PWD/etc/resources/mobile/resources.qrc
OTHER_FILES += $$PWD/etc/resources/mobile/.qml

#-------------------------------------------------------------------------------
# Deploy configurations
#-------------------------------------------------------------------------------

win32* {
    RC_FILE = $$PWD/etc/deploy/windows/Info.rc
}

macx* {
    ICON = $$PWD/etc/deploy/mac-osx/AppIcon.icns
    RC_FILE = $$PWD/etc/deploy/mac-osx/AppIcon.icns
    QMAKE_INFO_PLIST = $$PWD/etc/deploy/mac-osx/Info.plist
}

linux:!android {
    target.path = /usr/bin
    TARGET = open-ds
    desktop.path = /usr/share/applications
    desktop.files += $$PWD/etc/deploy/linux/open-ds.desktop
    INSTALLS += target desktop
}

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/etc/deploy/android

    DISTFILES += \
        etc/deploy/android/AndroidManifest.xml \
        etc/deploy/android/res/values/libs.xml \
        etc/deploy/android/build.gradle
}



#
# This file is part of QDriverStation
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

include ($$PWD/lib/LibDS/DriverStation.pri)

#-------------------------------------------------------------------------------
# Qt/C++ configuration
#-------------------------------------------------------------------------------

TARGET = QDriverStation

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

QT += gui
QT += core
QT += widgets
QT += network
CONFIG += c++11

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

#-------------------------------------------------------------------------------
# C++ compiler optimization flags, may make compilation slower
#-------------------------------------------------------------------------------

QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

#-------------------------------------------------------------------------------
# Import source code and resources of the application
#-------------------------------------------------------------------------------

INCLUDEPATH += $$PWD/src/desktop/include/
RESOURCES += $$PWD/etc/resources/desktop/resources.qrc

HEADERS += \
    $$PWD/src/desktop/include/AdvancedSettings.h \
    $$PWD/src/desktop/include/AssemblyInfo.h \
    $$PWD/src/desktop/include/Battery.h \
    $$PWD/src/desktop/include/CpuUsage.h \
    $$PWD/src/desktop/include/Dashboard.h \
    $$PWD/src/desktop/include/GamepadManager.h \
    $$PWD/src/desktop/include/InitTasks.h \
    $$PWD/src/desktop/include/Joysticks.h \
    $$PWD/src/desktop/include/MainWindow.h \
    $$PWD/src/desktop/include/Settings.h \
    $$PWD/src/desktop/include/SmartWindow.h

SOURCES += \
    $$PWD/src/desktop/sources/AdvancedSettings.cpp \
    $$PWD/src/desktop/sources/AssemblyInfo.cpp \
    $$PWD/src/desktop/sources/Battery.cpp \
    $$PWD/src/desktop/sources/CpuUsage.cpp \
    $$PWD/src/desktop/sources/Dashboard.cpp \
    $$PWD/src/desktop/sources/GamepadManager.cpp \
    $$PWD/src/desktop/sources/InitTasks.cpp \
    $$PWD/src/desktop/sources/Joysticks.cpp \
    $$PWD/src/desktop/sources/main.cpp \
    $$PWD/src/desktop/sources/MainWindow.cpp \
    $$PWD/src/desktop/sources/Settings.cpp \
    $$PWD/src/desktop/sources/SmartWindow.cpp

FORMS += \
    $$PWD/src/desktop/forms/Joysticks.ui \
    $$PWD/src/desktop/forms/MainWindow.ui \
    $$PWD/src/desktop/forms/AdvancedSettings.ui

#-------------------------------------------------------------------------------
# SDL boiler-plate
#-------------------------------------------------------------------------------

INCLUDEPATH += $$PWD/lib/SDL/include

win32* {
    DEFINES += SDL_WIN
}

win32-g++* {
    LIBS += -L$$PWD/lib/SDL/bin/windows/mingw/ -lSDL2
}

win32-msvc* {
    contains (QMAKE_TARGET.arch, x86_64) {
        LIBS += -L$$PWD/lib/SDL/bin/windows/msvc/x64/ -lSDL2
    }

    else {
        LIBS += -L$$PWD/lib/SDL/bin/windows/msvc/x86/ -lSDL2
    }
}

macx* {
    LIBS += -L$$PWD/lib/SDL/bin/mac-osx/ -lSDL2

    LIBS += -framework AudioToolbox
    LIBS += -framework AudioUnit
    LIBS += -framework Cocoa
    LIBS += -framework CoreAudio
    LIBS += -framework IOKit
    LIBS += -framework OpenGL
    LIBS += -framework CoreFoundation
    LIBS += -framework Carbon
    LIBS += -framework ForceFeedback

    QMAKE_LFLAGS += -F /System/Library/Frameworks/AudioToolbox.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/AudioUnit.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/Cocoa.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreAudio.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/IOKit.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/OpenGL.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreFoundation.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/Carbon.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/ForceFeedback.framework/
}

linux:!android {
    LIBS += -lSDL2
}

#-------------------------------------------------------------------------------
# Deploy configurations
#-------------------------------------------------------------------------------

win32* {
    LIBS += -lPdh
    RC_FILE = $$PWD/etc/deploy/windows/Info.rc
}

macx* {
    LIBS += -lssl
    ICON = $$PWD/etc/deploy/mac-osx/AppIcon.icns
    RC_FILE = $$PWD/etc/deploy/mac-osx/AppIcon.icns
    QMAKE_INFO_PLIST = $$PWD/etc/deploy/mac-osx/Info.plist
}

linux:!android {
    target.path = /usr/bin
    TARGET = qdriverstation
    icon.path = /usr/share/pixmaps
    desktop.path = /usr/share/applications
    icon.files += $$PWD/etc/deploy/linux/qdriverstation.ico
    desktop.files += $$PWD/etc/deploy/linux/qdriverstation.desktop
    INSTALLS += target desktop icon
}

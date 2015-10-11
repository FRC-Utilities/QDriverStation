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

#-------------------------------------------------------------------------------
# Deploy configuration
#-------------------------------------------------------------------------------

TARGET = QDriverStation

win32* {
    LIBS += -lPdh
    RC_FILE = $$PWD/etc/deploy/windows/info.rc
}

macx* {
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

#-------------------------------------------------------------------------------
# C++ build options
#-------------------------------------------------------------------------------

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

CONFIG += c++11

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

!win32* {
    QMAKE_CXXFLAGS_RELEASE *= -Ofast
}

win32* {
    QMAKE_CXXFLAGS_RELEASE *= -Ot
}

#-------------------------------------------------------------------------------
# SDL configuration code
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
# Include the LibDS library
#-------------------------------------------------------------------------------

include ($$PWD/lib/LibDS/LibDS.pri)

#-------------------------------------------------------------------------------
# Import headers, forms and sources
#-------------------------------------------------------------------------------

INCLUDEPATH += $$PWD/src/headers/

HEADERS += \
    $$PWD/src/headers/AdvancedSettings.h \
    $$PWD/src/headers/AssemblyInfo.h \
    $$PWD/src/headers/Battery.h \
    $$PWD/src/headers/BatteryProgressbar.h \
    $$PWD/src/headers/CpuUsage.h \
    $$PWD/src/headers/Dashboard.h \
    $$PWD/src/headers/GamepadManager.h \
    $$PWD/src/headers/InitTasks.h \
    $$PWD/src/headers/JoysticksWidget.h \
    $$PWD/src/headers/MainWindow.h \
    $$PWD/src/headers/Settings.h \
    $$PWD/src/headers/SmartWindow.h

SOURCES += \
    $$PWD/src/sources/AdvancedSettings.cpp \
    $$PWD/src/sources/AssemblyInfo.cpp \
    $$PWD/src/sources/Battery.cpp \
    $$PWD/src/sources/BatteryProgressbar.cpp \
    $$PWD/src/sources/CpuUsage.cpp \
    $$PWD/src/sources/Dashboard.cpp \
    $$PWD/src/sources/GamepadManager.cpp \
    $$PWD/src/sources/InitTasks.cpp \
    $$PWD/src/sources/JoysticksWidget.cpp \
    $$PWD/src/sources/MainWindow.cpp \
    $$PWD/src/sources/Settings.cpp \
    $$PWD/src/sources/SmartWindow.cpp \
    $$PWD/src/main.cpp

FORMS += \
    $$PWD/src/forms/AdvancedSettings.ui \
    $$PWD/src/forms/JoysticksWidget.ui \
    $$PWD/src/forms/MainWindow.ui

#-------------------------------------------------------------------------------
# Import resources
#-------------------------------------------------------------------------------

RESOURCES += \
    $$PWD/etc/resources/resources.qrc



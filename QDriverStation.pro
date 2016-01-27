#
# Copyright (c) 2015 WinT 3794 <http://wint3794.org>
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
    CONFIG -= windows
    QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS
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
    icon.files += $$PWD/etc/deploy/linux/qdriverstation.png
    desktop.files += $$PWD/etc/deploy/linux/qdriverstation.desktop
    INSTALLS += target desktop icon
}

#-------------------------------------------------------------------------------
# Make options
#-------------------------------------------------------------------------------

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

CONFIG += c++11

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

#-------------------------------------------------------------------------------
# Optimization flags
#-------------------------------------------------------------------------------

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

!win32* {
    QMAKE_CXXFLAGS_RELEASE *= -Ofast
}

win32-gcc* {
    QMAKE_CXXFLAGS_RELEASE *= -Ofast
}

win32-msvc* {
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
# Include other libraries
#-------------------------------------------------------------------------------

include ($$PWD/lib/LibDS/LibDS.pri)
include ($$PWD/lib/QtAwesome/QtAwesome.pri)

#-------------------------------------------------------------------------------
# Import headers, forms and sources
#-------------------------------------------------------------------------------

INCLUDEPATH += src

HEADERS += \
    src/Joysticks/JoystickManager.h \
    src/Joysticks/VirtualJoystick.h \
    src/MainWindow/Components/About.h \
    src/MainWindow/Components/Buttons.h \
    src/MainWindow/Components/CAN_Metrics.h \
    src/MainWindow/Components/Console.h \
    src/MainWindow/Components/Develop.h \
    src/MainWindow/Components/Diagnostics.h \
    src/MainWindow/Components/Joysticks.h \
    src/MainWindow/Components/LeftTab.h \
    src/MainWindow/Components/Operator.h \
    src/MainWindow/Components/Preferences.h \
    src/MainWindow/Components/RightTab.h \
    src/MainWindow/Components/Status.h \
    src/MainWindow/System/Battery.h \
    src/MainWindow/System/CPU.h \
    src/MainWindow/MainWindow.h \
    src/SettingsWindow/SettingsWindow.h \
    src/Updater/Updater.h \
    src/Utilities/AppTheme.h \
    src/Utilities/Global.h \
    src/Utilities/Settings.h \
    src/VJoystickWindow/VJoystickWindow.h \
    src/MainWindow/Hacks/CustomTabStyle.h \
    src/Joysticks/JoysticksCommon.h \
    src/Joysticks/SDL_Joysticks.h \
    src/Utilities/Beeper.h \
    src/MainWindow/Hacks/SmartWindow.h \
    src/Utilities/SoundPlayer.h

SOURCES += \
    src/Joysticks/JoystickManager.cpp \
    src/Joysticks/VirtualJoystick.cpp \
    src/MainWindow/Components/About.cpp \
    src/MainWindow/Components/Buttons.cpp \
    src/MainWindow/Components/CAN_Metrics.cpp \
    src/MainWindow/Components/Console.cpp \
    src/MainWindow/Components/Develop.cpp \
    src/MainWindow/Components/Diagnostics.cpp \
    src/MainWindow/Components/Joysticks.cpp \
    src/MainWindow/Components/LeftTab.cpp \
    src/MainWindow/Components/Operator.cpp \
    src/MainWindow/Components/Preferences.cpp \
    src/MainWindow/Components/RightTab.cpp \
    src/MainWindow/Components/Status.cpp \
    src/MainWindow/System/Battery.cpp \
    src/MainWindow/System/CPU.cpp \
    src/MainWindow/MainWindow.cpp \
    src/SettingsWindow/SettingsWindow.cpp \
    src/Updater/Updater.cpp \
    src/Utilities/AppTheme.cpp \
    src/Utilities/Global.cpp \
    src/Utilities/Settings.cpp \
    src/VJoystickWindow/VJoystickWindow.cpp \
    src/main.cpp \
    src/MainWindow/Hacks/CustomTabStyle.cpp \
    src/Joysticks/SDL_Joysticks.cpp \
    src/Utilities/Beeper.cpp \
    src/MainWindow/Hacks/SmartWindow.cpp \
    src/Utilities/SoundPlayer.cpp

#-------------------------------------------------------------------------------
# Import resources
#-------------------------------------------------------------------------------

RESOURCES += \
    $$PWD/etc/resources/resources.qrc



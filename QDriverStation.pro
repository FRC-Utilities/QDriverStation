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
# Other compilation flags
#-------------------------------------------------------------------------------

QMAKE_CXXFLAGS += -Werror  # Haha
QMAKE_CXXFLAGS += -Wunused

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

INCLUDEPATH += $$PWD/src

HEADERS += \
    $$PWD/src/Joysticks/JoystickManager.h \
    $$PWD/src/Joysticks/VirtualJoystick.h \
    $$PWD/src/MainWindow/Components/About.h \
    $$PWD/src/MainWindow/Components/Buttons.h \
    $$PWD/src/MainWindow/Components/CAN_Metrics.h \
    $$PWD/src/MainWindow/Components/Console.h \
    $$PWD/src/MainWindow/Components/Develop.h \
    $$PWD/src/MainWindow/Components/Diagnostics.h \
    $$PWD/src/MainWindow/Components/Joysticks.h \
    $$PWD/src/MainWindow/Components/LeftTab.h \
    $$PWD/src/MainWindow/Components/Operator.h \
    $$PWD/src/MainWindow/Components/Preferences.h \
    $$PWD/src/MainWindow/Components/RightTab.h \
    $$PWD/src/MainWindow/Components/Status.h \
    $$PWD/src/MainWindow/System/Battery.h \
    $$PWD/src/MainWindow/System/CPU.h \
    $$PWD/src/MainWindow/MainWindow.h \
    $$PWD/src/SettingsWindow/SettingsWindow.h \
    $$PWD/src/Updater/Updater.h \
    $$PWD/src/Global/AppTheme.h \
    $$PWD/src/Global/Global.h \
    $$PWD/src/Global/Settings.h \
    $$PWD/src/VJoystickWindow/VJoystickWindow.h \
    $$PWD/src/MainWindow/Hacks/CustomTabStyle.h \
    $$PWD/src/Joysticks/JoysticksCommon.h \
    $$PWD/src/Joysticks/SDL_Joysticks.h \
    $$PWD/src/Global/Beeper.h \
    $$PWD/src/MainWindow/Hacks/SmartWindow.h \
    $$PWD/src/Global/SoundPlayer.h \
    $$PWD/src/Dashboards/Dashboards.h \
    src/Global/Languages.h \
    src/InfoWindow/ConsoleWidget.h \
    src/InfoWindow/InfoWindow.h

SOURCES += \
    $$PWD/src/Joysticks/JoystickManager.cpp \
    $$PWD/src/Joysticks/VirtualJoystick.cpp \
    $$PWD/src/MainWindow/Components/About.cpp \
    $$PWD/src/MainWindow/Components/Buttons.cpp \
    $$PWD/src/MainWindow/Components/CAN_Metrics.cpp \
    $$PWD/src/MainWindow/Components/Console.cpp \
    $$PWD/src/MainWindow/Components/Develop.cpp \
    $$PWD/src/MainWindow/Components/Diagnostics.cpp \
    $$PWD/src/MainWindow/Components/Joysticks.cpp \
    $$PWD/src/MainWindow/Components/LeftTab.cpp \
    $$PWD/src/MainWindow/Components/Operator.cpp \
    $$PWD/src/MainWindow/Components/Preferences.cpp \
    $$PWD/src/MainWindow/Components/RightTab.cpp \
    $$PWD/src/MainWindow/Components/Status.cpp \
    $$PWD/src/MainWindow/System/Battery.cpp \
    $$PWD/src/MainWindow/System/CPU.cpp \
    $$PWD/src/MainWindow/MainWindow.cpp \
    $$PWD/src/SettingsWindow/SettingsWindow.cpp \
    $$PWD/src/Updater/Updater.cpp \
    $$PWD/src/Global/AppTheme.cpp \
    $$PWD/src/Global/Global.cpp \
    $$PWD/src/Global/Settings.cpp \
    $$PWD/src/VJoystickWindow/VJoystickWindow.cpp \
    $$PWD/src/main.cpp \
    $$PWD/src/MainWindow/Hacks/CustomTabStyle.cpp \
    $$PWD/src/Joysticks/SDL_Joysticks.cpp \
    $$PWD/src/Global/Beeper.cpp \
    $$PWD/src/MainWindow/Hacks/SmartWindow.cpp \
    $$PWD/src/Global/SoundPlayer.cpp \
    $$PWD/src/Dashboards/Dashboards.cpp \
    src/Global/Languages.cpp \
    src/InfoWindow/ConsoleWidget.cpp \
    src/InfoWindow/InfoWindow.cpp

#-------------------------------------------------------------------------------
# Import resources
#-------------------------------------------------------------------------------

RESOURCES += $$PWD/etc/resources/resources.qrc
RESOURCES += $$PWD/etc/translations/translations.qrc

#-------------------------------------------------------------------------------
# Translations
#-------------------------------------------------------------------------------

TRANSLATIONS += $$PWD/etc/translations/qds_es.ts
TRANSLATIONS += $$PWD/etc/translations/qds_en.ts
TRANSLATIONS += $$PWD/etc/translations/qds_de.ts

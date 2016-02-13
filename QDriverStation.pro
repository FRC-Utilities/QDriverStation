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

QMAKE_CXXFLAGS += -Wunused

#-------------------------------------------------------------------------------
# Include other libraries
#-------------------------------------------------------------------------------

include ($$PWD/lib/SDL/SDL.pri)
include ($$PWD/lib/LibDS/LibDS.pri)
include ($$PWD/lib/QtAwesome/QtAwesome.pri)

#-------------------------------------------------------------------------------
# Import headers, forms and sources
#-------------------------------------------------------------------------------

INCLUDEPATH += $$PWD/src

HEADERS += \
    $$PWD/src/Dashboards/Dashboards.h \
    $$PWD/src/Global/AppTheme.h \
    $$PWD/src/Global/Beeper.h \
    $$PWD/src/Global/Global.h \
    $$PWD/src/Global/Languages.h \
    $$PWD/src/Global/Settings.h \
    $$PWD/src/Joysticks/JoystickManager.h \
    $$PWD/src/Joysticks/JoysticksCommon.h \
    $$PWD/src/Joysticks/SDL_Joysticks.h \
    $$PWD/src/Joysticks/VirtualJoystick.h \
    $$PWD/src/MainWindow/Components/About.h \
    $$PWD/src/MainWindow/Components/Buttons.h \
    $$PWD/src/MainWindow/Components/CAN_Metrics.h \
    $$PWD/src/MainWindow/Components/Develop.h \
    $$PWD/src/MainWindow/Components/Diagnostics.h \
    $$PWD/src/MainWindow/Components/Joysticks.h \
    $$PWD/src/MainWindow/Components/LeftTab.h \
    $$PWD/src/MainWindow/Components/Messages.h \
    $$PWD/src/MainWindow/Components/Operator.h \
    $$PWD/src/MainWindow/Components/Preferences.h \
    $$PWD/src/MainWindow/Components/RightTab.h \
    $$PWD/src/MainWindow/Components/Status.h \
    $$PWD/src/MainWindow/Hacks/CustomTabStyle.h \
    $$PWD/src/MainWindow/System/Battery.h \
    $$PWD/src/MainWindow/System/CPU.h \
    $$PWD/src/MainWindow/MainWindow.h \
    $$PWD/src/SettingsWindow/SettingsWindow.h \
    $$PWD/src/Updater/Updater.h \
    $$PWD/src/VJoystickWindow/VJoystickWindow.h

SOURCES += \
    $$PWD/src/Dashboards/Dashboards.cpp \
    $$PWD/src/Global/AppTheme.cpp \
    $$PWD/src/Global/Beeper.cpp \
    $$PWD/src/Global/Global.cpp \
    $$PWD/src/Global/Languages.cpp \
    $$PWD/src/Global/Settings.cpp \
    $$PWD/src/Joysticks/JoystickManager.cpp \
    $$PWD/src/Joysticks/SDL_Joysticks.cpp \
    $$PWD/src/Joysticks/VirtualJoystick.cpp \
    $$PWD/src/MainWindow/Components/About.cpp \
    $$PWD/src/MainWindow/Components/Buttons.cpp \
    $$PWD/src/MainWindow/Components/CAN_Metrics.cpp \
    $$PWD/src/MainWindow/Components/Develop.cpp \
    $$PWD/src/MainWindow/Components/Diagnostics.cpp \
    $$PWD/src/MainWindow/Components/Joysticks.cpp \
    $$PWD/src/MainWindow/Components/LeftTab.cpp \
    $$PWD/src/MainWindow/Components/Messages.cpp \
    $$PWD/src/MainWindow/Components/Operator.cpp \
    $$PWD/src/MainWindow/Components/Preferences.cpp \
    $$PWD/src/MainWindow/Components/RightTab.cpp \
    $$PWD/src/MainWindow/Components/Status.cpp \
    $$PWD/src/MainWindow/Hacks/CustomTabStyle.cpp \
    $$PWD/src/MainWindow/System/Battery.cpp \
    $$PWD/src/MainWindow/System/CPU.cpp \
    $$PWD/src/MainWindow/MainWindow.cpp \
    $$PWD/src/SettingsWindow/SettingsWindow.cpp \
    $$PWD/src/Updater/Updater.cpp \
    $$PWD/src/VJoystickWindow/VJoystickWindow.cpp \
    $$PWD/src/main.cpp


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

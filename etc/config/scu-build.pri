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

DEFINES += SCU_BUILD

QT += gui
QT += core
QT += widgets
QT += network

INCLUDEPATH += $$PWD/../../src/headers/
INCLUDEPATH += $$PWD/../../lib/LibDS/include/

RESOURCES += $$PWD/../../etc/resources/resources.qrc

HEADERS += \
    $$PWD/../../lib/LibDS/include/DriverStation.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_Client.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_Common.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_ElapsedTime.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_Global.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_NetConsole.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_Protocol.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_Protocol2014.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_Protocol2015.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_ProtocolManager.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_Watchdog.h \
    $$PWD/../../lib/LibDS/include/LibDS/DS_Timers.h \
    $$PWD/../../src/headers/AdvancedSettings.h \
    $$PWD/../../src/headers/AssemblyInfo.h \
    $$PWD/../../src/headers/Battery.h \
    $$PWD/../../src/headers/BatteryProgressbar.h \
    $$PWD/../../src/headers/CpuUsage.h \
    $$PWD/../../src/headers/Dashboard.h \
    $$PWD/../../src/headers/GamepadManager.h \
    $$PWD/../../src/headers/InitTasks.h \
    $$PWD/../../src/headers/MainWindow.h \
    $$PWD/../../src/headers/Settings.h \
    $$PWD/../../src/headers/SmartWindow.h \
    $$PWD/../../src/headers/JoysticksWidget.h

SOURCES += $$PWD/../../src/scu.cpp

FORMS += \
    $$PWD/../../src/forms/MainWindow.ui \
    $$PWD/../../src/forms/JoysticksWidget.ui \
    $$PWD/../../src/forms/AdvancedSettings.ui

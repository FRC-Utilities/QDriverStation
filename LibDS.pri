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

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

QT += core
QT += network
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += $$PWD/include

#include ($$PWD/lib/QCustomPlot/QCustomPlot.pri)

FORMS += \
    $$PWD/src/forms/DS_LogWindow.ui

HEADERS += \
    $$PWD/include/DriverStation.h \
    $$PWD/src/headers/DS_Client.h \
    $$PWD/src/headers/DS_Common.h \
    $$PWD/src/headers/DS_LogWindow.h \
    $$PWD/src/headers/DS_NetConsole.h \
    $$PWD/src/headers/DS_RobotManager.h \
    $$PWD/src/headers/DS_NetworkDiagnostics.h \
    $$PWD/src/headers/DS_Protocol.h \
    $$PWD/src/headers/DS_Protocol2015.h \
    $$PWD/src/headers/DS_ElapsedTime.h \
    $$PWD/src/headers/DS_Debug.h

SOURCES += \
    $$PWD/src/sources/DriverStation.cpp \
    $$PWD/src/sources/DS_Client.cpp \
    $$PWD/src/sources/DS_Common.cpp \
    $$PWD/src/sources/DS_LogWindow.cpp \
    $$PWD/src/sources/DS_NetConsole.cpp \
    $$PWD/src/sources/DS_RobotManager.cpp \
    $$PWD/src/sources/DS_NetworkDiagnostics.cpp \
    $$PWD/src/sources/DS_Protocol.cpp \
    $$PWD/src/sources/DS_Protocol2015.cpp \
    $$PWD/src/sources/DS_ElapsedTime.cpp








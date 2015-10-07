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

QT += gui
QT += core
QT += network
QT += widgets

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += $$PWD/include

include ($$PWD/lib/QCustomPlot/QCustomPlot.pri)

FORMS += \
    $$PWD/src/LibDS/ui/DS_LogWindow.ui

SOURCES += \
    $$PWD/src/DriverStation.cpp \
    $$PWD/src/LibDS/DS_Client.cpp \
    $$PWD/src/LibDS/DS_Common.cpp \
    $$PWD/src/LibDS/DS_ElapsedTime.cpp \
    $$PWD/src/LibDS/DS_LogWindow.cpp \
    $$PWD/src/LibDS/DS_NetConsole.cpp \
    $$PWD/src/LibDS/DS_Protocol.cpp \
    $$PWD/src/LibDS/DS_Protocol2014.cpp \
    $$PWD/src/LibDS/DS_Protocol2015.cpp \
    $$PWD/src/LibDS/DS_ProtocolManager.cpp \
    $$PWD/src/LibDS/DS_Watchdog.cpp \
    $$PWD/src/LibDS/DS_Timers.cpp

HEADERS += \
    $$PWD/include/DriverStation.h \
    $$PWD/include/LibDS/DS_Client.h \
    $$PWD/include/LibDS/DS_Common.h \
    $$PWD/include/LibDS/DS_ElapsedTime.h \
    $$PWD/include/LibDS/DS_Global.h \
    $$PWD/include/LibDS/DS_LogWindow.h \
    $$PWD/include/LibDS/DS_NetConsole.h \
    $$PWD/include/LibDS/DS_Protocol.h \
    $$PWD/include/LibDS/DS_Protocol2014.h \
    $$PWD/include/LibDS/DS_Protocol2015.h \
    $$PWD/include/LibDS/DS_ProtocolManager.h \
    $$PWD/include/LibDS/DS_Watchdog.h \
    $$PWD/include/LibDS/DS_Timers.h

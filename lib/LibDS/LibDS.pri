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

CONFIG += c++11
INCLUDEPATH += $$PWD/src

QT += core
QT += network
QT += widgets
QT += multimedia

HEADERS += \
    $$PWD/src/Core/NetConsole.h \
    $$PWD/src/Core/Protocol.h \
    $$PWD/src/Core/Sockets.h \
    $$PWD/src/Core/Watchdog.h \
    $$PWD/src/Protocols/FRC_2014.h \
    $$PWD/src/Protocols/FRC_2015.h \
    $$PWD/src/Protocols/FRC_2016.h \
    $$PWD/src/Utilities/CRC32.h \
    $$PWD/src/DriverStation.h \
    $$PWD/src/Core/DS_Base.h \
    $$PWD/src/Core/DS_Config.h \
    $$PWD/src/Core/DS_Common.h \
    $$PWD/src/Core/RobotLogger.h

SOURCES += \
    $$PWD/src/Core/NetConsole.cpp \
    $$PWD/src/Core/Sockets.cpp \
    $$PWD/src/Core/Watchdog.cpp \
    $$PWD/src/Protocols/FRC_2014.cpp \
    $$PWD/src/Protocols/FRC_2015.cpp \
    $$PWD/src/Protocols/FRC_2016.cpp \
    $$PWD/src/Utilities/CRC32.cpp \
    $$PWD/src/DriverStation.cpp \
    $$PWD/src/Core/DS_Config.cpp \
    $$PWD/src/Core/Logger.cpp \
    $$PWD/src/Core/RobotLogger.cpp

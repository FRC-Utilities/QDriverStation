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
INCLUDEPATH += $$PWD/utils
INCLUDEPATH += $$PWD/include

QT += core
QT += network
QT += widgets
QT += multimedia

QMAKE_CXXFLAGS += -msse4.2

HEADERS += \
    $$PWD/include/LibDS.h \
    $$PWD/include/LibDS/DriverStation.h \
    $$PWD/include/LibDS/Core/Common.h \
    $$PWD/include/LibDS/Core/ElapsedTimer.h \
    $$PWD/include/LibDS/Core/Library.h \
    $$PWD/include/LibDS/Core/NetConsole.h \
    $$PWD/include/LibDS/Core/Watchdog.h \
    $$PWD/include/LibDS/Protocols/FRC_2016.h \
    $$PWD/include/LibDS/Protocols/FRC_2015.h \
    $$PWD/include/LibDS/Protocols/FRC_2014.h \
    $$PWD/include/LibDS/Core/AbstractProtocol.h \
    $$PWD/include/LibDS/Core/SocketManager.h \
    $$PWD/utils/sse4_crc32/crc32c.h

SOURCES += \
    $$PWD/src/DriverStation.cpp \
    $$PWD/src/Core/Common.cpp \
    $$PWD/src/Core/ElapsedTimer.cpp \
    $$PWD/src/Core/NetConsole.cpp \
    $$PWD/src/Core/Watchdog.cpp \
    $$PWD/src/Protocols/FRC_2016.cpp \
    $$PWD/src/Protocols/FRC_2015.cpp \
    $$PWD/src/Protocols/FRC_2014.cpp \
    $$PWD/src/Core/AbstractProtocol.cpp \
    $$PWD/src/Core/SocketManager.cpp \
    $$PWD/utils/sse4_crc32/crc32c_sse42.cpp \
    $$PWD/utils/sse4_crc32/crc32c.cpp

RESOURCES += $$PWD/etc/sounds/libds-sounds.qrc

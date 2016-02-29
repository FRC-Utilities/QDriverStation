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
INCLUDEPATH += $$PWD/include

QT += core
QT += network
QT += widgets
QT += multimedia

HEADERS += \
    $$PWD/include/LibDS.h \
    $$PWD/include/LibDS/DriverStation.h \
    $$PWD/include/LibDS/Core/Client.h \
    $$PWD/include/LibDS/Core/Common.h \
    $$PWD/include/LibDS/Core/ElapsedTimer.h \
    $$PWD/include/LibDS/Core/Library.h \
    $$PWD/include/LibDS/Core/NetConsole.h \
    $$PWD/include/LibDS/Core/ProtocolBase.h \
    $$PWD/include/LibDS/Core/ProtocolManager.h \
    $$PWD/include/LibDS/Core/Watchdog.h \
    $$PWD/include/LibDS/Protocols/FRC/Protocol2016.h \
    $$PWD/include/LibDS/Protocols/FRC/Protocol2015.h \
    $$PWD/include/LibDS/Protocols/FRC/Protocol2014.h

SOURCES += \
    $$PWD/src/DriverStation.cpp \
    $$PWD/src/Core/Client.cpp \
    $$PWD/src/Core/Common.cpp \
    $$PWD/src/Core/ElapsedTimer.cpp \
    $$PWD/src/Core/NetConsole.cpp \
    $$PWD/src/Core/ProtocolBase.cpp \
    $$PWD/src/Core/ProtocolManager.cpp \
    $$PWD/src/Core/Watchdog.cpp \
    $$PWD/src/Protocols/FRC/Protocol2016.cpp \
    $$PWD/src/Protocols/FRC/Protocol2015.cpp \
    $$PWD/src/Protocols/FRC/Protocol2014.cpp

RESOURCES += $$PWD/etc/sounds/libds-sounds.qrc

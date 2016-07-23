#
# Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
#
# This file is part of qMDNS, which is released under the MIT license.
# For more information, please read the LICENSE file in the root directory
# of this project.
#

include ($$PWD/../qMDNS.pri)

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = qmdns-util

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/Console.cpp

HEADERS += \
    $$PWD/Console.h

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj


QT += gui
QT += widgets

android {
  QT += network
}

CONFIG += c++11

INCLUDEPATH += $$PWD

include ($$PWD/../../LibDS-Legacy.pri)

HEADERS += \
    $$PWD/DriverStation.h \
    $$PWD/EventLogger.h

SOURCES += \
    $$PWD/DriverStation.cpp \
    $$PWD/EventLogger.cpp

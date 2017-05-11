QT += gui
QT += widgets

android {
  QT += network
}

CONFIG += c++11

INCLUDEPATH += $$PWD

include ($$PWD/../../LibDS.pri)

HEADERS += \
    $$PWD/DriverStation.h \
    $$PWD/EventLogger.h

SOURCES += \
    $$PWD/DriverStation.cpp \
    $$PWD/EventLogger.cpp

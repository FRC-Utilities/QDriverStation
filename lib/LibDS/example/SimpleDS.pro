QT += gui
QT += widgets

include ($$PWD/../LibDS.pri)

TEMPLATE = app
TARGET = SimpleDS

HEADERS += \
  $$PWD/src/Window.h \
    src/VirtualJoystick.h

SOURCES += \
  $$PWD/src/main.cpp \
  $$PWD/src/Window.cpp \
    src/VirtualJoystick.cpp

FORMS += \
  $$PWD/src/Window.ui

RESOURCES += \
  $$PWD/resources/resources.qrc

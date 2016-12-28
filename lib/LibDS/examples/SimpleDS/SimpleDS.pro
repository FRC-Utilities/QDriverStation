QT += gui
QT += widgets
QT += network

include ($$PWD/../../wrappers/Qt/LibDS-Qt.pri)

TEMPLATE = app
TARGET = SimpleDS

HEADERS += \
  $$PWD/src/Window.h \
  $$PWD/src/VirtualJoystick.h

SOURCES += \
  $$PWD/src/main.cpp \
  $$PWD/src/Window.cpp \
  $$PWD/src/VirtualJoystick.cpp

FORMS += \
  $$PWD/src/Window.ui

RESOURCES += \
  $$PWD/resources/resources.qrc

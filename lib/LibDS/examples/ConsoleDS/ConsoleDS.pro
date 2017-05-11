#-------------------------------------------------------------------------------
# Remove Qt dependency
#-------------------------------------------------------------------------------

CONFIG += console

CONFIG -= qt
CONFIG -= app_bundle

DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

#-------------------------------------------------------------------------------
# Deploy options
#-------------------------------------------------------------------------------

TARGET = console-ds

!win32* {
    target.path = /usr/bin
    INSTALLS += target
}

#-------------------------------------------------------------------------------
# Link with ncurses/pdcurses
#-------------------------------------------------------------------------------

win32* {
    LIBS += -L$$PWD/lib/pdcurses/win32/ -lpdcurses
    INCLUDEPATH += $$PWD/lib/pdcurses/include
} else {
    LIBS += -lncurses
}

#-------------------------------------------------------------------------------
# Include libraries
#-------------------------------------------------------------------------------

include ($$PWD/../../LibDS.pri)
include ($$PWD/lib/sdl/SDL.pri)

#-------------------------------------------------------------------------------
# Import source code
#-------------------------------------------------------------------------------

HEADERS += \
    $$PWD/src/interface.h \
    $$PWD/src/joystick.h

SOURCES += \
    $$PWD/src/interface.c \
    $$PWD/src/joystick.c \
    $$PWD/src/main.c



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

INCLUDEPATH += $$PWD/../../lib/SDL/include

win32* {
    DEFINES += SDL_WIN
}

win32-g++* {
    LIBS += -L$$PWD/../../lib/SDL/bin/windows/mingw/ -lSDL2
}

win32-msvc* {
    contains (QMAKE_TARGET.arch, x86_64) {
        LIBS += -L$$PWD/../../lib/SDL/bin/windows/msvc/x64/ -lSDL2
    }

    else {
        LIBS += -L$$PWD/../../lib/SDL/bin/windows/msvc/x86/ -lSDL2
    }
}

macx* {
    LIBS += -L$$PWD/../../lib/SDL/bin/mac-osx/ -lSDL2

    LIBS += -framework AudioToolbox
    LIBS += -framework AudioUnit
    LIBS += -framework Cocoa
    LIBS += -framework CoreAudio
    LIBS += -framework IOKit
    LIBS += -framework OpenGL
    LIBS += -framework CoreFoundation
    LIBS += -framework Carbon
    LIBS += -framework ForceFeedback

    QMAKE_LFLAGS += -F /System/Library/Frameworks/AudioToolbox.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/AudioUnit.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/Cocoa.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreAudio.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/IOKit.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/OpenGL.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreFoundation.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/Carbon.framework/
    QMAKE_LFLAGS += -F /System/Library/Frameworks/ForceFeedback.framework/
}

linux:!android {
    LIBS += -lSDL2
}

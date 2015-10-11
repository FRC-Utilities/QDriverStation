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

TARGET = QDriverStation

win32* {
    LIBS += -lPdh
    RC_FILE = $$PWD/../deploy/windows/info.rc
}

macx* {
    ICON = $$PWD/../deploy/mac-osx/AppIcon.icns
    RC_FILE = $$PWD/../deploy/mac-osx/AppIcon.icns
    QMAKE_INFO_PLIST = $$PWD/../deploy/mac-osx/Info.plist
}

linux:!android {
    target.path = /usr/bin
    TARGET = qdriverstation
    icon.path = /usr/share/pixmaps
    desktop.path = /usr/share/applications
    icon.files += $$PWD/../deploy/linux/qdriverstation.ico
    desktop.files += $$PWD/../deploy/linux/qdriverstation.desktop
    INSTALLS += target desktop icon
}

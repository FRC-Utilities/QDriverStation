INCLUDEPATH += $$PWD/include

win32-msvc* {
    contains (QMAKE_TARGET.arch, x86_64) {
        LIBS += -L$$PWD/dll/x64/ -lpthreadVC2
    }

    else {
        LIBS += -L$$PWD/dll/x86/ -lpthreadVC2
    }
}

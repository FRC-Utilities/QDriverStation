/*
 * This code is not mine, it was stealed from:
 * http://stackoverflow.com/questions/10110905/simple-wave-generator-with-sdl-in-c
 */

#ifndef _QDS_SHORTCUTS_H
#define _QDS_SHORTCUTS_H

#include <QObject>
#include <QKeyEvent>
#include <QApplication>

class Shortcuts : public QObject
{
    Q_OBJECT

private:
    bool eventFilter (QObject* object, QEvent* event);
};

#endif

/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once
#ifndef _QDRIVER_STATION_JOYSTICKS_KEY_EVENT_FILTER_H
#define _QDRIVER_STATION_JOYSTICKS_KEY_EVENT_FILTER_H

#include <QEvent>
#include <QObject>
#include <QKeyEvent>

/**
 * Allows us to get key presses and key releases from any object in
 * the application.
 *
 * We install this filter in the application itself, so that we can send
 * keyboard information to the virutal joystick, even if the MainWindow
 * is not selected.
 *
 * This filter is also used to detect when the user presses the
 * SHIFT or SPACE keys to trigger the Emergency Stop in the robot.
 */
class KeyEventFilter : public QObject
{
    Q_OBJECT

protected:
    bool eventFilter (QObject* object, QEvent* event);

signals:
    void KeyEvent (QKeyEvent* event, bool pressed);
};

#endif

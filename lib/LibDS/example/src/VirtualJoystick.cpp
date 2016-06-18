/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

#include <DriverStation.h>
#include "VirtualJoystick.h"

VirtualJoystick::VirtualJoystick() {
    qApp->installEventFilter (this);
    DriverStation::getInstance()->registerJoystick (6, 10, 1);
}

void VirtualJoystick::readAxes (int key, bool pressed) {
    int axis = -1;
    float value = pressed ? 1 : 0;

    /* Horizontal axis on thumb 1 */
    if (key == Qt::Key_D)
        axis = 0;
    else if (key == Qt::Key_A) {
        axis = 0;
        value *= -1;
    }

    /* Vertical axis on thumb 1 */
    if (key == Qt::Key_S)
        axis = 1;
    else if (key == Qt::Key_W) {
        axis = 1;
        value *= -1;
    }

    /* Trigger 1 */
    if (key == Qt::Key_E)
        axis = 2;
    else if (key == Qt::Key_Q) {
        axis = 2;
        value *= -1;
    }

    /* Trigger 2 */
    if (key == Qt::Key_O)
        axis = 3;
    else if (key == Qt::Key_U) {
        axis = 3;
        value *= -1;
    }

    /* Horizontal axis on thumb 2 */
    if (key == Qt::Key_L)
        axis = 4;
    else if (key == Qt::Key_J) {
        axis = 4;
        value *= -1;
    }

    /* Vertical axis on thumb 2 */
    if (key == Qt::Key_I)
        axis = 5;
    else if (key == Qt::Key_K) {
        axis = 5;
        value *= -1;
    }

    /* Send value to DS */
    if (axis != -1)
        DriverStation::getInstance()->updateAxis (0, axis, value);
}

void VirtualJoystick::readPOVs (int key, bool pressed) {
    int angle = 0;

    if (key == Qt::Key_Up)
        angle = 360;
    else if (key == Qt::Key_Right)
        angle = 90;
    else if (key == Qt::Key_Left)
        angle = 270;
    else if (key == Qt::Key_Down)
        angle = 180;

    if (!pressed)
        angle = 0;


    DriverStation::getInstance()->updatePOV (0, 0, angle);
}

void VirtualJoystick::readButtons (int key, bool pressed) {
    int button = -1;

    if (key == Qt::Key_0)
        button = 0;
    else if (key == Qt::Key_1)
        button = 1;
    else if (key == Qt::Key_2)
        button = 2;
    else if (key == Qt::Key_3)
        button = 3;
    else if (key == Qt::Key_4)
        button = 4;
    else if (key == Qt::Key_5)
        button = 5;
    else if (key == Qt::Key_6)
        button = 6;
    else if (key == Qt::Key_7)
        button = 7;
    else if (key == Qt::Key_8)
        button = 8;
    else if (key == Qt::Key_9)
        button = 9;

    if (button != -1)
        DriverStation::getInstance()->updateButton (0, button, pressed);
}

void VirtualJoystick::processKeyEvent (QKeyEvent* event, bool pressed) {
    readPOVs (event->key(), pressed);
    readAxes (event->key(), pressed);
    readButtons (event->key(), pressed);
}

bool VirtualJoystick::eventFilter (QObject* object, QEvent* event) {
    Q_UNUSED (object);

    switch (event->type()) {
    case QEvent::KeyPress:
        processKeyEvent (static_cast <QKeyEvent*> (event), true);
        break;
    case QEvent::KeyRelease:
        processKeyEvent (static_cast <QKeyEvent*> (event), false);
        break;
    default:
        break;
    }

    return false;
}

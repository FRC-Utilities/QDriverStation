/*
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "VirtualJoystick.h"

#include <QApplication>
#include <DriverStation.h>

/**
 * Initialize the event filter and register a joystick
 */
VirtualJoystick::VirtualJoystick()
{
    /* Listen for keyboard events */
    qApp->installEventFilter (this);

    /* Register a joystick with 6 axis, 1 POV and 10 buttons */
    DriverStation::getInstance()->resetJoysticks();
    DriverStation::getInstance()->addJoystick (6, 1, 10);
}

/**
 * Obtains the axis values from the pressed keyboard keys
 */
void VirtualJoystick::readAxes (int key, bool pressed)
{
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

    /* Update Driver Station */
    if (axis != -1)
        DriverStation::getInstance()->setJoystickAxis (0, axis, value);
}

/**
 * Obtains the POV/hat angle from the pressed keyboard keys
 */
void VirtualJoystick::readPOVs (int key, bool pressed)
{
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


    /* Update Driver Station */
    DriverStation::getInstance()->setJoystickHat (0, 0, angle);
}

/**
 * Obtains the buttons states from the pressed keyboard keys
 */
void VirtualJoystick::readButtons (int key, bool pressed)
{
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

    /* Update Driver Station */
    if (button != -1)
        DriverStation::getInstance()->setJoystickButton (0, button, pressed);
}

/**
 * Called whenever Qt registers a new keyboard event
 */
void VirtualJoystick::processKeyEvent (QKeyEvent* event, bool pressed)
{
    readPOVs (event->key(), pressed);
    readAxes (event->key(), pressed);
    readButtons (event->key(), pressed);
}

/**
 * Filters all the Qt events and extracts information from any keyboard event
 */
bool VirtualJoystick::eventFilter (QObject* object, QEvent* event)
{
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

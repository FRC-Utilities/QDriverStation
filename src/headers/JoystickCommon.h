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
#ifndef _QDRIVER_STATION_JOYSTICK_COMMON_H
#define _QDRIVER_STATION_JOYSTICK_COMMON_H

#include <QString>

/**
 * Represents a joystick and provides some information about it
 */
struct GM_Joystick {
    int id;              /**< The raw ID of the joystick */
    int numAxes;         /**< The number of axes that the joystick has */
    int numButtons;      /**< The number of buttons that the joystick has */
    QString displayName; /**< The user-friendly name of the joystick*/
};


/**
 * Represents the state of a joystick hat and provides some data about it
 */
struct GM_Hat {
    int id;               /**< The raw ID of the hat */
    short angle;          /**< The pressed angle of the hat */
    GM_Joystick joystick; /**< The joystick that the hat belongs to */
};

/**
 * Represents the state of a joystick axis and provides some data about it
 */
struct GM_Axis {
    int id;               /**< The raw ID of the axis */
    double value;         /**< The value (between -1 and 1) of the axis */
    GM_Joystick joystick; /**< The joystick that the axis belongs to */
};

/**
 * Represents the state of a joystick button and provides some data about it
 */
struct GM_Button {
    int id;               /**< The raw ID of the button */
    bool pressed;         /**< The current state of the button */
    GM_Joystick joystick; /**< The joystick that the button belongs to */
};

#endif


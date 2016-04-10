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

#ifndef _QDS_JOYSTICKS_COMMON_H
#define _QDS_JOYSTICKS_COMMON_H

#include <QString>
#include <QObject>

/**
 * Represents an input device with:
 *   - Its USB/device ID
 *   - Display name
 *   - Number of axes, buttons and POV hats
 */
struct QDS_InputDevice {
    QString         name;
    int             numAxes;
    int             numPOVs;
    QString         device_id;
    int             numButtons;
    bool            blacklisted;
    int             device_number;
};

/**
 * Represents a joystick rumble event that is triggered by
 * the application itself.
 * This structure contains:
 *   - A pointer to the target joystick
 *   - The rumble type (left, right or both)
 */
struct QDS_RumbleRequest {
    bool             leftRumble;
    bool             rightRumble;
    QDS_InputDevice* joystick;
};

/**
 * Represents an axis event that is triggered by a joystick.
 * This structure contains:
 *    - A pointer to the involved joystick
 *    - The axis ID
 *    - The axis value
 */
struct QDS_AxisEvent {
    int              axis;
    float            value;
    QDS_InputDevice* joystick;
};

/**
 * Represents a button event that is triggered by a joystick.
 * This structure contains:
 *    - A pointer to the involved joystick
 *    - The button ID
 *    - The 'pressed' state of the button
 */
struct QDS_ButtonEvent {
    int              button;
    bool             pressed;
    QDS_InputDevice* joystick;
};

/**
 * Represents a POV event that is triggered by a joystick.
 * This structure contains:
 *    - A pointer to the involved joystick
 *    - The POV id
 *    - The angle value of the POV hat
 */
struct QDS_POVEvent {
    int              pov;
    int              angle;
    QDS_InputDevice* joystick;
};

#endif

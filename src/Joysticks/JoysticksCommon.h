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
    int numAxes;
    int numPOVs;
    int numButtons;
    int device_number;

    QString name;
    QString device_id;
};

/**
 * Represents a joystick rumble event that is triggered by
 * the application itself.
 * This structure contains:
 *   - A pointer to the target joystick
 *   - The rumble type (left, right or both)
 */
struct QDS_RumbleRequest {
    QDS_InputDevice* joystick;

    bool leftRumble;
    bool rightRumble;
};

/**
 * Represents an axis event that is triggered by a joystick.
 * This structure contains:
 *    - A pointer to the involved joystick
 *    - The axis ID
 *    - The axis value
 */
struct QDS_AxisEvent {
    QDS_InputDevice joystick;

    int axis;
    float value;
};

/**
 * Represents a button event that is triggered by a joystick.
 * This structure contains:
 *    - A pointer to the involved joystick
 *    - The button ID
 *    - The 'pressed' state of the button
 */
struct QDS_ButtonEvent {
    QDS_InputDevice joystick;

    int button;
    bool pressed;
};

/**
 * Represents a POV event that is triggered by a joystick.
 * This structure contains:
 *    - A pointer to the involved joystick
 *    - The POV id
 *    - The angle value of the POV hat
 */
struct QDS_POVEvent {
    QDS_InputDevice joystick;

    int pov;
    int angle;
};

#endif

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

#ifndef _QJOYSTICKS_COMMON_H
#define _QJOYSTICKS_COMMON_H

#include <QString>

///
/// Represents an input device with:
///  - Its USB/device ID
///  - The Display name
///  - The number of axes, buttons and POVs
///
struct QJoystickDevice {
    int             id;
    QString         name;
    int             numAxes;
    int             numPOVs;
    int             numButtons;
    bool            blacklisted;
};

///
/// Represents a joystick rumble event that is triggered by
/// the application itself.
///
struct QJoystickRumble {
    bool             leftRumble;
    bool             rightRumble;
    QJoystickDevice* joystick;
};

///
/// Represents an axis event that can be triggered by a josytick.
/// This structure contains:
///   - A pointer to the joystick that caused the event
///   - The axis number/ID
///   - The axis value
///
struct QJoystickAxisEvent {
    int              axis;
    float            value;
    QJoystickDevice* joystick;
};

///
/// Represents an button event that can be triggered by a josytick.
/// This structure contains:
///   - A pointer to the joystick that caused the event
///   - The button number/ID
///   - The button state
///
struct QJoystickButtonEvent {
    int              button;
    bool             pressed;
    QJoystickDevice* joystick;
};

///
/// Represents an POV event that can be triggered by a josytick.
/// This structure contains:
///   - A pointer to the joystick that caused the event
///   - The POV number/ID
///   - The POV angle
///
struct QJoystickPOVEvent {
    int              pov;
    int              angle;
    QJoystickDevice* joystick;
};

#endif

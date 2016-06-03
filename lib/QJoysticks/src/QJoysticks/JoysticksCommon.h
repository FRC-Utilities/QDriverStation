/*
 * Copyright (c) 2015-2016 Alex Spataru <alex_spataru@outlook.com>
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

/**
 * @brief The QJoystickDevice struct
 *
 * This structure represents a joystick and its respective properties,
 * including the following:
 *     - A numerical ID
 *     - The joystick display name
 *     - The number of axes operated by the joystick
 *     - The number of buttons operated by the joystick
 *     - The number of POVs operated by the joystick
 *     - A boolean value blacklisting or whitelisting the joystick
 */
struct QJoystickDevice {
    int     id;
    QString name;
    int     numAxes;
    int     numPOVs;
    int     numButtons;
    bool    blacklisted;
};

/**
 * Represents a joystick rumble request.
 * This structure contains:
 *    - A pointer to the joystick that should be rumbled
 *    - The length (in milliseconds) of the rumble effect.
 *    - The strength of the effect (from 0 to 1)
 */
struct QJoystickRumble {
    uint length;
    float strength;
    QJoystickDevice* joystick;
};

/**
 * Represents an POV event that can be triggered by a joystick.
 * This structure contains:
 *    - A pointer to the joystick that triggered the event
 *    - The POV number/ID
 *    - The current POV angle
 */
struct QJoystickPOVEvent {
    int pov;
    int angle;
    QJoystickDevice* joystick;
};

/**
 * Represents an axis event that can be triggered by a joystick.
 * This structure contains:
 *    - A pointer to the joystick that caused the event
 *    - The axis number/ID
 *    - The current axis value
 */
struct QJoystickAxisEvent {
    int axis;
    float value;
    QJoystickDevice* joystick;
};

/**
 * Represents a button event that can be triggered by a joystick.
 * This structure contains:
 *   - A pointer to the joystick that caused the event
 *   - The button number/ID
 *   - The current button state (pressed or not pressed)
 */
struct QJoystickButtonEvent {
    int button;
    bool pressed;
    QJoystickDevice* joystick;
};

#endif

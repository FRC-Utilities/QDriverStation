/*
 * The Driver Station Library (LibDS)
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
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

#ifndef _LIB_DS_JOYSTICKS_H
#define _LIB_DS_JOYSTICKS_H

#ifdef __cplusplus
extern "C" {
#endif

extern void Joysticks_Init (void);
extern void Joysticks_Close (void);

extern int DS_GetJoystickCount (void);
extern int DS_GetJoystickNumHats (int joystick);
extern int DS_GetJoystickNumAxes (int joystick);
extern int DS_GetJoystickNumButtons (int joystick);

extern int DS_GetJoystickHat (int joystick, int hat);
extern float DS_GetJoystickAxis (int joystick, int axis);
extern int DS_GetJoystickButton (int joystick, int button);

extern void DS_JoysticksReset (void);
extern void DS_JoysticksAdd (const int axes, const int hats, const int buttons);
extern void DS_SetJoystickHat (int joystick, int hat, int angle);
extern void DS_SetJoystickAxis (int joystick, int axis, float value);
extern void DS_SetJoystickButton (int joystick, int button, int pressed);

#ifdef __cplusplus
}
#endif

#endif

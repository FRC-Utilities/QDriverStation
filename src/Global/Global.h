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

#ifndef _QDS_GLOBAL_H
#define _QDS_GLOBAL_H

//=============================================================================
// System/library includes
//=============================================================================

#include <QDebug>
#include <QtAwesome.h>
#include <LibDS/DriverStation.h>

//=============================================================================
// Application includes
//=============================================================================

class Beeper;
class Settings;
class AppTheme;
class InfoWindow;
class SettingsWindow;
class VJoystickWindow;
class JoystickManager;

//=============================================================================
// Global variables
//=============================================================================

#define NULL_STR QString ("")
#define  NO_DATA QString ("--.--")

//=============================================================================
// Global Objects
//=============================================================================

extern Beeper* BEEPER();
extern DriverStation* DS();
extern QtAwesome* AWESOME();
extern QThread* SECONDARY_THREAD();
extern InfoWindow* INFORMATION_WINDOW();
extern SettingsWindow* SETTINGS_WINDOW();
extern VJoystickWindow* VJOYSTICK_WINDOW();
extern JoystickManager* JOYSTICK_MANAGER();

//=============================================================================
// Common UI factors
//=============================================================================

extern QMargins NULL_MARGINS();
extern QMargins MAIN_MARGINS();
extern QMargins SMALL_MARGINS();
extern qreal DPI_SCALE (qreal input);

//=============================================================================
// Global functions
//=============================================================================

extern void SDL_INIT();
extern void GLOBAL_INIT();
extern void SHOW_SETTINGS_WINDOW();
extern void SHOW_VIRTUAL_JOYSTICKS();
extern void MORSE_BEEP (QString input, int frequency);

#endif

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

#ifndef _QDS_GLOBAL_H
#define _QDS_GLOBAL_H

//=============================================================================
// System/library includes
//=============================================================================

#include <QDebug>
#include <LibDS.h>
#include <QtAwesome.h>

//=============================================================================
// Define the used classes
//=============================================================================

class Beeper;
class Settings;
class AppTheme;
class QSpacerItem;
class SettingsWindow;
class VJoystickWindow;
class JoystickManager;

//=============================================================================
// Global variables
//=============================================================================

///
/// I don't know why I defined this, don't ask me, the answer is in your
/// heart...
///
const QString NULL_STR = "";

///
/// Used as a placeholder in some widgets when the DS <--> robot communications
/// haven't been established.
///
/// For example, this text is displayed on the voltage label.
///
const QString NO_DATA = "--.--";

//=============================================================================
// Global Objects
//=============================================================================

///
/// Allows us to generate different sound tones to "enchance" user experience
///
extern Beeper* BEEPER();

///
/// Shortcut to DriverStation::getInstance() to make the code more readable
///
extern DriverStation* QDS();

///
/// Allows us to use FontAwesome as an icon source for the application.
/// This function returns a single instance to the "QtAwesome" class.
///
extern QtAwesome* AWESOME();

///
/// Generates an expandable spacer, which are used by most widgets in the
/// QDriverStation to maintain a proportional layout.
///
extern QSpacerItem* SPACER();

///
/// Returns a single instance pointer to the settings window
///
extern SettingsWindow* SETTINGS_WINDOW();

///
/// Returns a single instance pointer to the virtual joystick settings
///
extern VJoystickWindow* VJOYSTICK_WINDOW();

///
/// Returns a single instance pointer to the joystick manager, which,
/// uhm, manages joystick input/output...
///
extern JoystickManager* JOYSTICK_MANAGER();

//=============================================================================
// Common UI factors
//=============================================================================

///
/// Returns a margins object with 0 pixels on each side
///
extern QMargins NULL_MARGINS();

///
/// Returns a margins object with 10 scaled pixels on each side
///
extern QMargins MAIN_MARGINS();

///
/// Returns a margins object with 1 scaled pixel on each side
///
extern QMargins SMALL_MARGINS();

///
/// Scales the given \a input to match the screen resolution and DPI.
/// This function allows the QDriverStation to be resolution independent
/// and be able to look good on high-dpi screens.
///
extern qreal DPI_SCALE (qreal input);

//=============================================================================
// Global functions
//=============================================================================

///
/// Initializes and configures SDL for joystick usage and generating sounds.
///
extern void SDL_INIT();

///
/// Initializes and configures SDL and the LibDS.
///
extern void GLOBAL_INIT();

///
/// Shows the settings window (duh)
///
extern void SHOW_SETTINGS_WINDOW();

///
/// Shows the virtual joystick window (duh)
///
extern void SHOW_VIRTUAL_JOYSTICKS();

///
/// "Talks" the \a input morse code with the given \a frequency.
/// Yeah, this function is relatively useless and dumb, but is used
/// in the status widget to insult you when you try to enable the robot
/// in invalid conditions...
///
extern void MORSE_BEEP (QString input, int frequency);

#endif

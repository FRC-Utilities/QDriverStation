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

//=============================================================================
// System includes
//=============================================================================

#include <SDL.h>
#include <QThread>
#include <QScreen>
#include <QSpacerItem>
#include <QMessageBox>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "Global.h"
#include "Beeper.h"
#include "Settings.h"
#include "AppTheme.h"
#include "Joysticks/JoystickManager.h"
#include "SettingsWindow/SettingsWindow.h"
#include "VJoystickWindow/VJoystickWindow.h"

//=============================================================================
// Global variables
//=============================================================================

static qreal RATIO = -1;

//=============================================================================
// BEEPER
//=============================================================================

Beeper* BEEPER()
{
    static Beeper instance;
    instance.moveToThread (SECONDARY_THREAD());
    return &instance;
}

//=============================================================================
// DS
//=============================================================================

DriverStation* DS()
{
    return DriverStation::getInstance();
}

//=============================================================================
// AWESOME
//=============================================================================

QtAwesome* AWESOME()
{
    static QtAwesome instance;

    instance.initFontAwesome();
    instance.setDefaultOption ("scale-factor", DPI_SCALE (1));
    instance.setDefaultOption ("color", AppTheme::Theme().foreground);

    return &instance;
}

//=============================================================================
// SPACER
//=============================================================================

QSpacerItem* SPACER()
{
    return new QSpacerItem (0, 0,
                            QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);
}

//=============================================================================
// SECONDARY_THREAD
//=============================================================================

QThread* SECONDARY_THREAD()
{
    static QThread instance;
    instance.start (QThread::HighPriority);
    QObject::connect (qApp,      &QApplication::aboutToQuit,
                      &instance, &QThread::quit);

    return &instance;
}

//=============================================================================
// JOYSTICK_MANAGER
//=============================================================================

JoystickManager* JOYSTICK_MANAGER()
{
    static JoystickManager instance;
    instance.moveToThread (SECONDARY_THREAD());
    return &instance;
}

//=============================================================================
// SETTINGS_WINDOW
//=============================================================================

SettingsWindow* SETTINGS_WINDOW()
{
    static SettingsWindow instance;
    return &instance;
}

//=============================================================================
// VJOYSTICK_WINDOW
//=============================================================================

VJoystickWindow* VJOYSTICK_WINDOW()
{
    static VJoystickWindow instance;
    return &instance;
}

//=============================================================================
// NULL_MARGINS
//=============================================================================

QMargins NULL_MARGINS()
{
    return QMargins (0, 0, 0, 0);
}

//=============================================================================
// MAIN_MARGINS
//=============================================================================

QMargins MAIN_MARGINS()
{
    return QMargins (DPI_SCALE (10),
                     DPI_SCALE (10),
                     DPI_SCALE (10),
                     DPI_SCALE (10));
}

//=============================================================================
// SMALL_MARGINS
//=============================================================================

QMargins SMALL_MARGINS()
{
    return QMargins (DPI_SCALE (1),
                     DPI_SCALE (1),
                     DPI_SCALE (1),
                     DPI_SCALE (1));
}

//=============================================================================
// DPI_SCALE
//=============================================================================

qreal DPI_SCALE (qreal input)
{
    if (RATIO == -1)
        {
            RATIO = (QApplication::primaryScreen()->physicalDotsPerInch() / 100) * 0.9;

            if (RATIO < 1.2)
                RATIO = 1;

            DS_LogMessage (kInfoLevel,
                           QString ("UI Scale Ratio set to: %1").arg (RATIO));
        }

    return input * RATIO;
}

//=============================================================================
// SDL_INIT
//=============================================================================

void SDL_INIT()
{
    DS_LogMessage (kInfoLevel, "Initializing SDL...");

    SDL_JoystickEventState (SDL_ENABLE);
    SDL_SetHint (SDL_HINT_XINPUT_ENABLED, "0");
    SDL_SetHint (SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if (SDL_Init (SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_AUDIO) != 0)
        {
            QMessageBox::warning (Q_NULLPTR,
                                  QObject::tr ("QDriverStation"),
                                  QObject::tr ("SDL initailization error: %1")
                                  .arg (SDL_GetError()));

            QMessageBox::warning (Q_NULLPTR,
                                  QObject::tr ("QDriverStation"),
                                  QObject::tr ("You will still be able to use "
                                               "the QDriverStation, however, you "
                                               "MUST be VERY careful while using "
                                               "an external joystick!"));
        }

    DS_LogMessage (kInfoLevel, "SDL Initialized");
}

//=============================================================================
// GLOBAL_INIT
//=============================================================================

void GLOBAL_INIT()
{
    SDL_INIT();

    Q_UNUSED (SETTINGS_WINDOW());
    Q_UNUSED (VJOYSTICK_WINDOW());
}

//=============================================================================
// SHOW_SETTINGS_WINDOW
//=============================================================================

void SHOW_SETTINGS_WINDOW()
{
    SETTINGS_WINDOW()->show();
}

//=============================================================================
// SHOW_VIRTUAL_JOYSTICKS
//=============================================================================

void SHOW_VIRTUAL_JOYSTICKS()
{
    VJOYSTICK_WINDOW()->show();
}

//=============================================================================
// MORSE_BEEP
//=============================================================================

void MORSE_BEEP (QString input, int frequency)
{
    foreach (QString character, input.split (""))
        {
            int time = 0;
            int base = 50;

            if (character == ".")
                time = base;
            else if (character == "-")
                time = base * 3;
            else if (character == " ")
                time = base * 3;
            else if (character == "/")
                time = base * 7;

            BEEPER()->beep (frequency, time);
            BEEPER()->beep (0, base);
        }
}

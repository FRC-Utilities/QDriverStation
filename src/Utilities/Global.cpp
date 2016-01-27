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

//=============================================================================
// Application includes
//=============================================================================

#include "Global.h"
#include "SettingsWindow/SettingsWindow.h"
#include "VJoystickWindow/VJoystickWindow.h"

//=============================================================================
// System includes
//=============================================================================

#include <SDL.h>
#include <QThread>
#include <QScreen>
#include <QMessageBox>
#include <QApplication>

//=============================================================================
// Global variables
//=============================================================================

static qreal RATIO = -1;

//=============================================================================
// Global objects
//=============================================================================

static Beeper* _beeper = Q_NULLPTR;
static QtAwesome* _awesome = Q_NULLPTR;
static QThread* _secondaryThread = Q_NULLPTR;
static SettingsWindow* _settingsWindow = Q_NULLPTR;
static VJoystickWindow* _vjoystickWindow = Q_NULLPTR;
static JoystickManager* _joystickManager = Q_NULLPTR;

//=============================================================================
// BEEPER
//=============================================================================

Beeper* BEEPER()
{
    if (_beeper == Q_NULLPTR)
        {
            _beeper = new Beeper();
            _beeper->moveToThread (SECONDARY_THREAD());
        }

    return _beeper;
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
    if (_awesome == Q_NULLPTR)
        {
            _awesome = new QtAwesome (qApp);
            _awesome->initFontAwesome();
            _awesome->setDefaultOption ("scale-factor", DPI_SCALE (1));
            _awesome->setDefaultOption ("color", AppTheme::Theme().foreground);
        }

    return _awesome;
}

//=============================================================================
// SECONDARY_THREAD
//=============================================================================

QThread* SECONDARY_THREAD()
{
    if (_secondaryThread == Q_NULLPTR)
        _secondaryThread = new QThread();

    return _secondaryThread;
}

//=============================================================================
// JOYSTICK_MANAGER
//=============================================================================

JoystickManager* JOYSTICK_MANAGER()
{
    if (_joystickManager == Q_NULLPTR)
        _joystickManager = new JoystickManager();

    return _joystickManager;
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
            RATIO = (qApp->primaryScreen()->physicalDotsPerInch() / 100) * 0.9;

            if (RATIO < 1.2)
                RATIO = 1;
        }

    return input * RATIO;
}

//=============================================================================
// SDL_INIT
//=============================================================================

void SDL_INIT()
{
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
}

//=============================================================================
// GLOBAL_INIT
//=============================================================================

void GLOBAL_INIT()
{
    SDL_INIT();
    DS()->init();

    if (_settingsWindow == Q_NULLPTR)
        _settingsWindow = new SettingsWindow;

    if (_vjoystickWindow == Q_NULLPTR)
        _vjoystickWindow = new VJoystickWindow;
}

//=============================================================================
// SHOW_SETTINGS_WINDOW
//=============================================================================

void SHOW_SETTINGS_WINDOW()
{
    _settingsWindow->show();
}

//=============================================================================
// SHOW_VIRTUAL_JOYSTICKS
//=============================================================================

void SHOW_VIRTUAL_JOYSTICKS()
{
    _vjoystickWindow->show();
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

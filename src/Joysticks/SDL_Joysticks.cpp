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
// System includes
//=============================================================================

#include <QTimer>
#include <QMessageBox>
#include <QApplication>

//=============================================================================
// Application includes
//=============================================================================

#include "SDL_Joysticks.h"
#include "Utilities/Global.h"

//=============================================================================
// Fix errors/warnings with main() re-define
//=============================================================================

#define SDL_MAIN_HANDLED

//=============================================================================
// SDL_Joysticks::SDL_Joysticks
//=============================================================================

SDL_Joysticks::SDL_Joysticks()
{
    m_tracker = -1;
    connect (DS(), SIGNAL (initialized()), this, SLOT (update()));
}

//=============================================================================
// SDL_Joysticks::~SDL_Joysticks
//=============================================================================

SDL_Joysticks::~SDL_Joysticks()
{
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        SDL_GameControllerClose (SDL_GameControllerOpen (i));

    SDL_Quit();
}

//=============================================================================
// SDL_Joysticks::joysticks
//=============================================================================

QList<QDS_InputDevice> SDL_Joysticks::joysticks()
{
    QList<QDS_InputDevice> list;

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        list.append (getJoystick (i));

    return list;
}

//=============================================================================
// SDL_Joysticks::rumble
//=============================================================================

void SDL_Joysticks::rumble (QDS_RumbleRequest request)
{
    SDL_Haptic* haptic = SDL_HapticOpen (request.joystick->id);

    if (haptic != Q_NULLPTR)
        {
            SDL_HapticRumbleInit (haptic);
            SDL_HapticRumblePlay (haptic, 1, 1000);
        }
}

//=============================================================================
// SDL_Joysticks::update
//=============================================================================

void SDL_Joysticks::update()
{
    SDL_Event event;

    while (SDL_PollEvent (&event))
        {
            switch (event.type)
                {
                case SDL_JOYDEVICEADDED:
                    ++m_tracker;
                    emit countChanged();
                    break;
                case SDL_JOYDEVICEREMOVED:
                    emit countChanged();
                    break;
                case SDL_JOYAXISMOTION:
                    emit axisEvent (getAxisEvent (&event));
                    break;
                case SDL_JOYBUTTONUP:
                    emit buttonEvent (getButtonEvent (&event));
                    break;
                case SDL_JOYBUTTONDOWN:
                    emit buttonEvent (getButtonEvent (&event));
                    break;
                case SDL_JOYHATMOTION:
                    emit POVEvent (getPOVEvent (&event));
                    break;
                }
        }

    QTimer::singleShot (20, this, SLOT (update()));
}

//=============================================================================
// SDL_Joysticks::getDynamicID
//=============================================================================

int SDL_Joysticks::getDynamicID (int id)
{
    id = m_tracker - (id + 1);

    if (id < 0)
        id = abs (id);

    if (id >= SDL_NumJoysticks())
        id -= 1;

    return id;
}

//=============================================================================
// SDL_Joysticks::getJoystick
//=============================================================================

QDS_InputDevice SDL_Joysticks::getJoystick (int id)
{
    QDS_InputDevice joystick;
    SDL_Joystick* sdl_joystick = SDL_JoystickOpen (id);

    joystick.id = getDynamicID (id);

    if (sdl_joystick != Q_NULLPTR)
        {
            joystick.name       = SDL_JoystickNameForIndex (id);
            joystick.numPOVs    = SDL_JoystickNumHats      (sdl_joystick);
            joystick.numAxes    = SDL_JoystickNumAxes      (sdl_joystick);
            joystick.numButtons = SDL_JoystickNumButtons   (sdl_joystick);
        }

    return joystick;
}

//=============================================================================
// SDL_Joysticks::getPOVEvent
//=============================================================================

QDS_POVEvent SDL_Joysticks::getPOVEvent (const SDL_Event* sdl_event)
{
    QDS_POVEvent event;
    event.pov      = sdl_event->jhat.hat;
    event.joystick = getJoystick (sdl_event->jdevice.which);

    switch (sdl_event->jhat.value)
        {
        case SDL_HAT_RIGHTUP:
            event.angle = 45;
            break;
        case SDL_HAT_RIGHTDOWN:
            event.angle = 135;
            break;
        case SDL_HAT_LEFTDOWN:
            event.angle = 225;
            break;
        case SDL_HAT_LEFTUP:
            event.angle = 315;
            break;
        case SDL_HAT_UP:
            event.angle = 0;
            break;
        case SDL_HAT_RIGHT:
            event.angle = 90;
            break;
        case SDL_HAT_DOWN:
            event.angle = 180;
            break;
        case SDL_HAT_LEFT:
            event.angle = 270;
            break;
        default:
            event.angle = -1;
            break;
        }

    return event;
}

//=============================================================================
// SDL_Joysticks::getAxisEvent
//=============================================================================

QDS_AxisEvent SDL_Joysticks::getAxisEvent (const SDL_Event* sdl_event)
{
    QDS_AxisEvent event;

    event.axis     = sdl_event->jaxis.axis;
    event.value    = (double) sdl_event->jaxis.value / 32767;
    event.joystick = getJoystick (sdl_event->jdevice.which);

    return event;
}

//=============================================================================
// SDL_Joysticks::getButtonEvent
//=============================================================================

QDS_ButtonEvent SDL_Joysticks::getButtonEvent (const SDL_Event* sdl_event)
{
    QDS_ButtonEvent event;

    event.button   = sdl_event->jbutton.button;
    event.pressed  = sdl_event->jbutton.state == SDL_PRESSED;
    event.joystick = getJoystick (sdl_event->jdevice.which);

    return event;
}

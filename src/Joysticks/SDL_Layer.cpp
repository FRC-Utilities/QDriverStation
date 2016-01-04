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

#include <QTimer>
#include <QMessageBox>
#include <QApplication>

#include "SDL_Layer.h"

SDL_Layer* SDL_Layer::s_instance = Q_NULLPTR;

//=============================================================================
// SDL_Layer::SDL_Layer
//=============================================================================

SDL_Layer::SDL_Layer()
{
    m_time = 50;
    m_tracker = -1;

    SDL_JoystickEventState (SDL_ENABLE);
    SDL_SetHint (SDL_HINT_XINPUT_ENABLED, "0");
    SDL_SetHint (SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if (SDL_Init (SDL_INIT_JOYSTICK) != 0)
        {
            QMessageBox::critical (Q_NULLPTR, tr ("Fatal Error!"),
                                   tr ("SDL Init Error: %1").arg (SDL_GetError()));

            exit (EXIT_FAILURE);
        }
}

//=============================================================================
// SDL_Layer::~SDL_Layer
//=============================================================================

SDL_Layer::~SDL_Layer()
{
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        SDL_GameControllerClose (SDL_GameControllerOpen (i));

    SDL_Quit();
}

//=============================================================================
// SDL_Layer::GetInstance
//=============================================================================

SDL_Layer* SDL_Layer::GetInstance()
{
    if (s_instance == Q_NULLPTR)
        s_instance = new SDL_Layer;

    return s_instance;
}

//=============================================================================
// SDL_Layer::GetNumAxes
//=============================================================================

int SDL_Layer::GetNumAxes (int js)
{
    return SDL_JoystickNumAxes (SDL_JoystickOpen (js));
}

//=============================================================================
// SDL_Layer::GetNumHats
//=============================================================================

int SDL_Layer::GetNumHats (int js)
{
    return SDL_JoystickNumHats (SDL_JoystickOpen (js));
}

//=============================================================================
// SDL_Layer::GetNumButtons
//=============================================================================

int SDL_Layer::GetNumButtons (int js)
{
    return SDL_JoystickNumButtons (SDL_JoystickOpen (js));
}

//=============================================================================
// SDL_Layer::GetJoystickName
//=============================================================================

QString SDL_Layer::GetJoystickName (int js)
{
    return SDL_JoystickNameForIndex (js);
}

//=============================================================================
// SDL_Layer::JoystickList
//=============================================================================

QStringList SDL_Layer::JoystickList()
{
    QStringList list;

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        list.append (GetJoystickName (i));

    return list;
}

//=============================================================================
// SDL_Layer::Init
//=============================================================================

void SDL_Layer::Init()
{
    QTimer::singleShot (500, Qt::CoarseTimer, this, SLOT (ReadSdlEvents()));
}

//=============================================================================
// SDL_Layer::SetUpdateInterval
//=============================================================================

void SDL_Layer::SetUpdateInterval (int time)
{
    if (time >= 0)
        m_time = time;
}

//=============================================================================
// SDL_Layer::Rumble
//=============================================================================

void SDL_Layer::Rumble (int js, int time)
{
    SDL_InitSubSystem (SDL_INIT_HAPTIC);
    SDL_Haptic* haptic = SDL_HapticOpen (js);

    if (haptic != Q_NULLPTR)
        {
            SDL_HapticRumbleInit (haptic);
            SDL_HapticRumblePlay (haptic, 1, time);
        }
}

//=============================================================================
// SDL_Layer::GetHat
//=============================================================================

GM_Hat SDL_Layer::GetHat (const SDL_Event* event)
{
    GM_Hat hat;

    hat.id = event->jhat.hat;
    hat.value = event->jhat.value;
    hat.joystick = GetJoystick (event);

    switch (hat.value)
        {
        case SDL_HAT_RIGHTUP:
            hat.angle = 45;
            break;
        case SDL_HAT_RIGHTDOWN:
            hat.angle = 135;
            break;
        case SDL_HAT_LEFTDOWN:
            hat.angle = 225;
            break;
        case SDL_HAT_LEFTUP:
            hat.angle = 315;
            break;
        case SDL_HAT_UP:
            hat.angle = 0;
            break;
        case SDL_HAT_RIGHT:
            hat.angle = 90;
            break;
        case SDL_HAT_DOWN:
            hat.angle = 180;
            break;
        case SDL_HAT_LEFT:
            hat.angle = 270;
            break;
        default:
            hat.angle = -1;
            break;
        }

    return hat;
}

//=============================================================================
// SDL_Layer::GetAxis
//=============================================================================

GM_Axis SDL_Layer::GetAxis (const SDL_Event* event)
{
    GM_Axis axis;

    axis.id = event->jaxis.axis;
    axis.joystick = GetJoystick (event);
    axis.value = ScaleAxisOutput (event->jaxis.value);

    return axis;
}

//=============================================================================
// SDL_Layer::GetButton
//=============================================================================

GM_Button SDL_Layer::GetButton (const SDL_Event* event)
{
    GM_Button button;

    button.id = event->jbutton.button;
    button.joystick = GetJoystick (event);
    button.pressed = event->jbutton.state == SDL_PRESSED;

    return button;
}

//=============================================================================
// SDL_Layer::GetJoystick
//=============================================================================

GM_Joystick SDL_Layer::GetJoystick (const SDL_Event* event)
{
    GM_Joystick stick;

    stick.id = GetDynamicID (event->jdevice.which);
    stick.numAxes = GetNumAxes (event->jdevice.which);
    stick.numButtons = GetNumButtons (event->jdevice.which);
    stick.displayName = GetJoystickName (event->jdevice.which);

    return stick;
}

//=============================================================================
// SDL_Layer::GetDynamicID
//=============================================================================

int SDL_Layer::GetDynamicID (int id)
{
    id = m_tracker - (id + 1);
    if (id < 0) id = abs (id);
    if (id >= SDL_NumJoysticks()) id -= 1;

    return id;
}

//=============================================================================
// SDL_Layer::ScaleAxisOutput
//=============================================================================

double SDL_Layer::ScaleAxisOutput (double input)
{
    return input /= 32767;
}

//=============================================================================
// SDL_Layer::ReadSdlEvents
//=============================================================================

void SDL_Layer::ReadSdlEvents()
{
    SDL_Event event;
    while (SDL_PollEvent (&event))
        {
            switch (event.type)
                {
                case SDL_JOYDEVICEADDED:
                    ++m_tracker;
                    emit CountChanged (JoystickList());
                    emit CountChanged (SDL_NumJoysticks());
                    break;
                case SDL_JOYDEVICEREMOVED:
                    emit CountChanged (JoystickList());
                    emit CountChanged (SDL_NumJoysticks());
                    break;
                case SDL_JOYAXISMOTION:
                    OnAxisEvent (&event);
                    break;
                case SDL_JOYBUTTONUP:
                    OnButtonEvent (&event);
                    break;
                case SDL_JOYBUTTONDOWN:
                    OnButtonEvent (&event);
                    break;
                case SDL_JOYHATMOTION:
                    OnHatEvent (&event);
                    break;
                }
        }

    QTimer::singleShot (m_time, this, SLOT (ReadSdlEvents()));
}

//=============================================================================
// SDL_Layer::OnHatEvent
//=============================================================================

void SDL_Layer::OnHatEvent (const SDL_Event* event)
{
    emit HatEvent (GetHat (event));
}

//=============================================================================
// SDL_Layer::OnAxisEvent
//=============================================================================

void SDL_Layer::OnAxisEvent (const SDL_Event* event)
{
    emit AxisEvent (GetAxis (event));
}

//=============================================================================
// SDL_Layer::OnButtonEvent
//=============================================================================

void SDL_Layer::OnButtonEvent (const SDL_Event* event)
{
    emit ButtonEvent (GetButton (event));
}

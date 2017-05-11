/*
 * Copyright (C) 2015-2016 Alex Spataru <alex_spataru@outlook>
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

#include "interface.h"

#include <SDL.h>
#include <LibDS.h>
#include <stdio.h>
#include <pthread.h>

#define INVALID_ID     -1
#define SDL_AXIS_RANGE 0x8000

/**
 * Allows the \c update_joysticks() function to know if its
 * safe to check for new SDL events
 */
static int initialized = 0;

/**
 * SDL2 does not adjust the joystick IDs with the 'real' USB
 * ID's, which causes a lot of trouble when updating the
 * joystick values with the Driver Station
 */
static int joystick_tracker = -1;

/**
 * Calculates the dynamic ID of the given joystick
 */
static int get_id (const int id)
{
    int joystick = id;
    joystick = abs (joystick_tracker - (joystick + 1));

    if (joystick >= SDL_NumJoysticks())
        joystick -= 1;

    return joystick;
}

/**
 * Registers all the detected SDL joysticks with the Driver Station
 */
static void register_joysticks (void)
{
    DS_JoysticksReset();

    int i;
    for (i = 0; i < SDL_NumJoysticks(); ++i) {
        SDL_Joystick* joystick = SDL_JoystickOpen (i);

        if (joystick) {
            DS_JoysticksAdd (SDL_JoystickNumAxes (joystick),
                             SDL_JoystickNumHats (joystick),
                             SDL_JoystickNumButtons (joystick));
        }
    }
}

/**
 * Extracts the hat/POV information from the given \a event
 * and updates the state of the Driver Station joysticks
 */
static void process_hat_event (SDL_Event* event)
{
    if (!event)
        return;

    int angle = 0;
    int hat = event->jhat.hat;
    int joystick = get_id (event->jhat.which);

    switch (event->jhat.value) {
    case SDL_HAT_RIGHTUP:
        angle = 45;
        break;
    case SDL_HAT_RIGHTDOWN:
        angle = 135;
        break;
    case SDL_HAT_LEFTDOWN:
        angle = 225;
        break;
    case SDL_HAT_LEFTUP:
        angle = 315;
        break;
    case SDL_HAT_UP:
        angle = 0;
        break;
    case SDL_HAT_RIGHT:
        angle = 90;
        break;
    case SDL_HAT_DOWN:
        angle = 180;
        break;
    case SDL_HAT_LEFT:
        angle = 270;
        break;
    default:
        angle = -1;
        break;
    }

    if (joystick > INVALID_ID)
        DS_SetJoystickHat (joystick, hat, angle);
}

/**
 * Extracts the axis information from the given \a event and updates
 * the state of the Driver Station joysticks
 */
static void process_axis_event (SDL_Event* event)
{
    if (!event)
        return;

    int axis = event->jaxis.axis;
    int joystick = get_id (event->jaxis.which);
    double value = ((double) (event->jaxis.value)) / SDL_AXIS_RANGE;

    if (joystick > INVALID_ID)
        DS_SetJoystickAxis (joystick, axis, value);
}

/**
 * Extracts the button information from the given \a event
 * and updates the state of the Driver Station joysticks
 */
static void process_button_event (SDL_Event* event)
{
    if (!event)
        return;

    int button = event->jbutton.button;
    int joystick = get_id (event->jbutton.which);
    int pressed = (event->jbutton.state == SDL_PRESSED);

    if (joystick > INVALID_ID)
        DS_SetJoystickButton (joystick, button, pressed);
}

/**
 * Initializes SDL in another thread (to avoid slowing down the startup time)
 */
void init_joysticks (void)
{
    if (SDL_Init (SDL_INIT_JOYSTICK) == 0) {
        initialized = 1;
        SDL_JoystickEventState (SDL_ENABLE);
    }

    else {
        printf ("Cannot initialize SDL!");
        exit (EXIT_FAILURE);
    }
}

/**
 * Instructs the SDL library to begin closing down
 */
void close_joysticks (void)
{
    int i;
    for (i = 0; i < SDL_NumJoysticks(); ++i)
        SDL_JoystickClose (SDL_JoystickOpen (i));

    SDL_Quit();
}

/**
 * Queries for new SDL joystick events and updates the
 * Driver Station with the new joystick information.
 */
void update_joysticks (void)
{
    if (!initialized)
        return;

    SDL_Event event;
    while (SDL_PollEvent (&event)) {
        switch (event.type) {
        case SDL_JOYDEVICEADDED:
            ++joystick_tracker;
            register_joysticks();
            break;
        case SDL_JOYDEVICEREMOVED:
            register_joysticks();
            break;
        case SDL_JOYAXISMOTION:
            process_axis_event (&event);
            break;
        case SDL_JOYHATMOTION:
            process_hat_event (&event);
            break;
        case SDL_JOYBUTTONDOWN:
            process_button_event (&event);
            break;
        case SDL_JOYBUTTONUP:
            process_button_event (&event);
            break;
        default:
            break;
        }
    }
}

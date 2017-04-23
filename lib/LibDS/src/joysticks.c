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

#include "DS_Array.h"
#include "DS_Config.h"
#include "DS_Events.h"
#include "DS_Joysticks.h"

#include <stdio.h>

/**
 * Represents a joystick and its information
 */
typedef struct _joystick {
    int* hats;       /**< An array with the hat angles */
    float* axes;    /**< An array with the axis values */
    int* buttons;    /**< An array with the button states */
    int num_axes;    /**< The number of axes of the joystick */
    int num_hats;    /**< The number of hats of the joystick */
    int num_buttons; /**< The number of buttons of the joystick */
} DS_Joystick;

/**
 * Holds all the joysticks
 */
static DS_Array array;

/**
 * Registers a joystick event to the LibDS event system
 */
static void register_event()
{
    DS_Event event;
    event.joystick.count = DS_GetJoystickCount();
    event.joystick.type = DS_JOYSTICK_COUNT_CHANGED;
    DS_AddEvent (&event);
}

/**
 * Returns the joystick structure at the given index
 * If the joystick does not exist, this function shall return \c NULL
 */
static DS_Joystick* get_joystick (int joystick)
{
    if ((int) array.used > joystick)
        return (DS_Joystick*) array.data [joystick];

    return NULL;
}

/**
 * Returns \c true if the given \a joystick exists and is valid
 */
static int joystick_exists (int joystick)
{
    if (DS_GetJoystickCount() > joystick)
        return get_joystick (joystick) != NULL;

    return 0;
}

/**
 * Initializes the joystick array, with an initial support for 6 joysticks
 */
void Joysticks_Init (void)
{
    DS_ArrayInit (&array, 6);
}

/**
 * De-allocates the joystick array data
 */
void Joysticks_Close (void)
{
    DS_ArrayFree (&array);
    register_event();
}

/**
 * Returns the number of joysticks registered with the LibDS
 */
int DS_GetJoystickCount (void)
{
    return (int) array.used;
}

/**
 * Returns the number of hats that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DS_GetJoystickNumHats (int joystick)
{
    if (joystick_exists (joystick))
        return get_joystick (joystick)->num_hats;

    return 0;
}

/**
 * Returns the number of axes that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DS_GetJoystickNumAxes (int joystick)
{
    if (joystick_exists (joystick))
        return get_joystick (joystick)->num_axes;

    return 0;
}

/**
 * Returns the number of buttons that the given \a joystick has.
 * If the joystick does not exist, this function will return \c 0
 */
int DS_GetJoystickNumButtons (int joystick)
{
    if (joystick_exists (joystick))
        return get_joystick (joystick)->num_buttons;

    return 0;
}

/**
 * Returns the value that the given \a hat in the given \a joystick has.
 * If the joystick or hat do not exist, this function will return \c 0
 *
 * \note Regardless of protocol implementation, this function will return
 *       a neutral value if the robot is disabled. This is for additional
 *       safety!
 */
int DS_GetJoystickHat (int joystick, int hat)
{
    if (CFG_GetRobotEnabled() && joystick_exists (joystick)) {
        DS_Joystick* stick = get_joystick (joystick);

        if (stick->num_hats > hat)
            return stick->hats [hat];
    }

    return 0;
}

/**
 * Returns the value that the given \a axis in the given \a joystick has.
 * If the joystick or axis do not exist, this function will return \c 0
 *
 * \note Regardless of protocol implementation, this function will return
 *       a neutral value if the robot is disabled. This is for additional
 *       safety!
 */
float DS_GetJoystickAxis (int joystick, int axis)
{
    if (CFG_GetRobotEnabled() && joystick_exists (joystick)) {
        DS_Joystick* stick = get_joystick (joystick);

        if (stick->num_axes > axis)
            return stick->axes [axis];
    }

    return 0;
}

/**
 * Returns the value that the given \a button in the given \a joystick has.
 * If the joystick or button do not exist, this function will return \c 0
 *
 * \note Regardless of protocol implementation, this function will return
 *       a neutral value if the robot is disabled. This is for additional
 *       safety!
 */
int DS_GetJoystickButton (int joystick, int button)
{
    if (CFG_GetRobotEnabled() && joystick_exists (joystick)) {
        DS_Joystick* stick = get_joystick (joystick);

        if (stick->num_buttons > button)
            return stick->buttons [button];
    }

    return 0;
}

/**
 * Removes all the registered joysticks from the LibDS
 */
void DS_JoysticksReset (void)
{
    DS_ArrayFree (&array);
    DS_ArrayInit (&array, 6);

    register_event();
}

/**
 * Registers a new joystick with the given number of \a axes, \a hats and
 * \a buttons. All joystick values are set to a neutral state to ensure
 * safe operation of the robot.
 */
void DS_JoysticksAdd (const int axes, const int hats, const int buttons)
{
    /* Joystick is empty */
    if (axes <= 0 && hats <= 0 && buttons <= 0) {
        fprintf (stderr, "DS_JoystickAdd: Cannot register empty joystick!\n");
        return;
    }

    /* Allocate memory for a new joystick */
    DS_Joystick* joystick = (DS_Joystick*) calloc (1, sizeof (DS_Joystick));

    /* Set joystick properties */
    joystick->num_axes = axes;
    joystick->num_hats = hats;
    joystick->num_buttons = buttons;

    /* Set joystick value arrays */
    joystick->hats = calloc (hats, sizeof (int));
    joystick->axes = calloc (axes, sizeof (float));
    joystick->buttons = calloc (buttons, sizeof (int));

    /* Register the new joystick in the joystick list */
    DS_ArrayInsert (&array, (void*) joystick);

    /* Emit the joystick count changed event */
    register_event();
}

/**
 * Updates the \a angle of the given \a hat in the given \a joystick
 */
void DS_SetJoystickHat (int joystick, int hat, int angle)
{
    if (joystick_exists (joystick)) {
        DS_Joystick* stick = get_joystick (joystick);

        if (stick->num_hats > hat)
            stick->hats [hat] = angle;
    }
}

/**
 * Updates the \a value of the given \a axis in the given \a joystick
 */
void DS_SetJoystickAxis (int joystick, int axis, float value)
{
    if (joystick_exists (joystick)) {
        DS_Joystick* stick = get_joystick (joystick);

        if (stick->num_axes > axis)
            stick->axes [axis] = value;
    }
}

/**
 * Updates the \a pressed state of the given \a button in the given \a joystick
 */
void DS_SetJoystickButton (int joystick, int button, int pressed)
{
    if (joystick_exists (joystick)) {
        DS_Joystick* stick = get_joystick (joystick);

        if (stick->num_buttons > button)
            stick->buttons [button] = (pressed > 0) ? 1 : 0;
    }
}

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

#include "DS_Utils.h"
#include "DS_Array.h"
#include "DS_Timer.h"

#include <stdio.h>
#include <assert.h>

#if defined _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

static DS_Array timers;
static int running = 0;

/**
 * Updates the properties of the given \a timer
 * This function is called in a separate thread for each timer that we use.
 * This is **not** the best way to implement a timer, but its effective,
 * easy to understand and efficient. Also, this implementation does not need
 * to play with the kernel/OS to compare elapsed times periodically.
 */
static void* update_timer (void* ptr)
{
    assert (ptr);
    DS_Timer* timer = (DS_Timer*) ptr;

    while (running == 1) {
        if (timer->enabled && timer->time > 0 && !timer->expired) {
            timer->elapsed += timer->precision;

            if (timer->elapsed >= timer->time)
                timer->expired = 1;
        }

        DS_Sleep (timer->precision);
    }

    return NULL;
}

/**
 * Initializes the timer array, which is used to keep track of all the timers
 * used by the library. We need to do this so that we can terminate each timer
 * thread once the module is closed.
 */
void Timers_Init (void)
{
    running = 1;
    DS_ArrayInit (&timers, 10);
}

/**
 * Breaks all the timer loops and stops every thread used by this module
 */
void Timers_Close (void)
{
    running = 0;
    DS_ArrayFree (&timers);
}

/**
 * Pauses the execution state of the program/thread for the given
 * number of \a millisecs.
 *
 * We use this function to update each timer based on its precision
 */
void DS_Sleep (const int millisecs)
{
#if defined _WIN32
    Sleep (millisecs);
#else
    usleep (millisecs * 1000);
#endif
}

/**
 * Resets and disables the given \a timer
 */
void DS_TimerStop (DS_Timer* timer)
{
    assert (timer);

    timer->enabled = 0;
    timer->expired = 0;
    timer->elapsed = 0;
}

/**
 * Resets and enables the given \a timer
 */
void DS_TimerStart (DS_Timer* timer)
{
    assert (timer);

    timer->enabled = 1;
    timer->expired = 0;
    timer->elapsed = 0;
}

/**
 * Resets the elapsed time and expired state of the given \a timer
 */
void DS_TimerReset (DS_Timer* timer)
{
    assert (timer);

    timer->expired = 0;
    timer->elapsed = 0;
}

/**
 * Initializes the given \a timer with the given \a time and \a precision.
 * The timers are updated using a threaded while loop (that sleeps the number
 * of milliseconds specified with \a precision). As stated before, this is
 * probably not the best timer implementation out there, but its simple and
 * it works perfectly for this library.
 *
 * A word of advice, using a higher \a precision (lower value in msecs) will
 * result in increased CPU usage, this thing does not have morals and will eat
 * the whole cake if you allow it.
 */
void DS_TimerInit (DS_Timer* timer, const int time, const int precision)
{
    /* Check if timer pointer is valid */
    assert (timer);

    /* Timer has already been initialized, fuck off */
    if (timer->initialized)
        return;

    /* Configure the timer */
    timer->enabled = 0;
    timer->expired = 0;
    timer->elapsed = 0;
    timer->time = time;
    timer->initialized = 1;
    timer->precision = precision;

    /* Configure the thread */
    pthread_t thread;
    int error = pthread_create (&thread, NULL,
                                &update_timer, (void*) timer);

    /* Check if thread was started */
    assert (!error);
}

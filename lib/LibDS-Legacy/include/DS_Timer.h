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

#ifndef _LIB_DS_TIMER_H
#define _LIB_DS_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

/**
 * Represents a tiemr and its properties
 */
typedef struct _timer {
    int time;         /**< The time to wait until the timer expires */
    int expired;      /**< Set to \c 1 if \a elapsed is greater than \a time */
    int enabled;      /**< Enabled state of the timer */
    int elapsed;      /**< Number of milliseconds elapsed since last reset */
    int precision;    /**< The update interval (in milliseconds) */
    int initialized;  /**< Set to \c 1 if the timer has been initialized */
} DS_Timer;

extern void Timers_Init (void);
extern void Timers_Close (void);
extern void DS_Sleep (const int millisecs);
extern void DS_TimerStop (DS_Timer* timer);
extern void DS_TimerStart (DS_Timer* timer);
extern void DS_TimerReset (DS_Timer* timer);
extern void DS_TimerInit (DS_Timer* timer, const int time, const int precision);

#ifdef __cplusplus
}
#endif

#endif

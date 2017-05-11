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

#include "LibDS.h"
#include "DS_Config.h"

static int init = 0;

/**
 * Initializes all the modules of the LibDS library, you should call this
 * function before your application begins interacting with the different
 * modules of the LibDS.
 */
void DS_Init (void)
{
    if (!DS_Initialized()) {
        init = 1;

        Timers_Init();
        Client_Init();
        Events_Init();
        Sockets_Init();
        Joysticks_Init();
        Protocols_Init();
    }
}

/**
 * Closes all the modules of the LibDS library, you should call this before
 * exiting your application. Failure to do this may result with socket
 * problems (regardless if you are using the offical DS or not), memory
 * problems and increased CPU usage (due to threads managed by the LibDS)
 */
void DS_Close (void)
{
    if (DS_Initialized()) {
        init = 0;

        Timers_Close();
        Sockets_Close();
        Protocols_Close();
        Joysticks_Close();

        Events_Close();
        Client_Close();
    }
}

/**
 * Returns \c 1 if the DS is initialized, \c 0 if not
 */
int DS_Initialized (void)
{
    return init;
}

/**
 * Returns the current version of LibDS
 * This is defined in the source to avoid reporting wrong versions when
 * importing LibDS as a library (e.g. a DLL) and using non-corresponding
 * header files
 */
char* DS_GetVersion (void)
{
    return "2.2";
}

/**
 * Returns the date when the library was compiled
 */
char* DS_GetBuildDate (void)
{
    return __DATE__;
}

/**
 * Returns the time when the library was compiled
 */
char* DS_GetBuildTime (void)
{
    return __TIME__;
}

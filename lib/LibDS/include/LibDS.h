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

#ifndef _LIB_DS_MAIN_H
#define _LIB_DS_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DS_Timer.h"
#include "DS_Types.h"
#include "DS_Utils.h"
#include "DS_Events.h"
#include "DS_Client.h"
#include "DS_Socket.h"
#include "DS_Protocol.h"
#include "DS_Joysticks.h"
#include "DS_DefaultProtocols.h"

extern void DS_Init (void);
extern void DS_Close (void);
extern int DS_Initialized (void);

extern char* DS_GetVersion (void);
extern char* DS_GetBuildDate (void);
extern char* DS_GetBuildTime (void);

#ifdef __cplusplus
}
#endif

#endif

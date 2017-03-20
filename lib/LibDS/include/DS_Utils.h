/*
 * The Driver Station Library (LibDS)
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

#ifndef _LIB_DS_UTILS_H
#define _LIB_DS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <bstrlib.h>
#include <bstraux.h>

/*
 * You may find these useful
 */
#define DS_Max(a,b) ((a) > (b) ? a : b)
#define DS_Min(a,b) ((a) < (b) ? a : b)
#define DS_FallBackAddress bfromcstr ("0.0.0.0");

/*
 * Smarter memory de-allocation functions
 */
#define DS_FREE(ptr)    if (ptr) { free     (ptr); ptr = NULL; }
#define DS_FREESTR(str) if (str) { bstrFree (str); DS_FREE (str); }

extern int DS_StopThread (pthread_t* thread);
extern uint8_t DS_GetFByte (float val, float max);
extern int DS_StringIsEmpty (const bstring string);
extern bstring DS_GetEmptyString (const int length);
extern uint32_t DS_CRC32 (const void* buf, size_t size);
extern bstring DS_GetStaticIP (const int net, const int team, const int host);

#ifdef __cplusplus
}
#endif

#endif

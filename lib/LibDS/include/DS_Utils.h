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

#ifndef _LIB_DS_UTILS_H
#define _LIB_DS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "DS_String.h"

/*
 * You may find these useful
 */
#define DS_FallBackAddress "0.0.0.0"
#define DS_Max(a,b) ((a) > (b) ? a : b)
#define DS_Min(a,b) ((a) < (b) ? a : b)
#define DS_FREE(p) if (p) { free (p); p = NULL; }

/*
 * Icon types for message boxes
 */
typedef enum {
    DS_ICON_INFORMATION,
    DS_ICON_WARNING,
    DS_ICON_ERROR,
} DS_IconType;

/*
 * Misc functions
 */
extern uint32_t DS_CRC32 (const void* buf, size_t size);
extern uint8_t DS_FloatToByte (const float val, const float max);
extern DS_String DS_GetStaticIP (const int net, const int team, const int host);
extern void DS_ShowMessageBox (const DS_String* caption,
                               const DS_String* message,
                               const DS_IconType icon);

#ifdef __cplusplus
}
#endif

#endif

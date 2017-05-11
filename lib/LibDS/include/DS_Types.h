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

#ifndef _LIB_DS_TYPES_H
#define _LIB_DS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DS_CONTROL_TEST,
    DS_CONTROL_AUTONOMOUS,
    DS_CONTROL_TELEOPERATED,
} DS_ControlMode;

typedef enum {
    DS_ALLIANCE_RED,
    DS_ALLIANCE_BLUE,
} DS_Alliance;

typedef enum {
    DS_POSITION_1,
    DS_POSITION_2,
    DS_POSITION_3,
} DS_Position;

typedef enum {
    DS_SOCKET_UDP,
    DS_SOCKET_TCP,
} DS_SocketType;

#ifdef __cplusplus
}
#endif

#endif

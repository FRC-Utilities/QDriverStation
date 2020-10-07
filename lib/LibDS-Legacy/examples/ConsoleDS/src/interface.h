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

#ifndef _CONSOLE_DS_INTERFACE_H
#define _CONSOLE_DS_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

extern void init_interface (void);
extern void close_interface (void);
extern void update_interface (void);
extern void update_status_label (void);

extern void set_can (const int can);
extern void set_cpu (const int cpu);
extern void set_ram (const int ram);
extern void set_disk (const int disk);
extern void set_enabled (const int enabled);
extern void set_robot_code (const int code);
extern void set_robot_comms (const int comms);
extern void set_voltage (const double voltage);
extern void set_has_joysticks (const int joysticks);

#ifdef __cplusplus
}
#endif

#endif

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

#ifndef _LIB_DS_PROTOCOL_H
#define _LIB_DS_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DS_Socket.h"

typedef struct _protocol {
    sds (*fms_address)();
    sds (*radio_address)();
    sds (*robot_address)();

    sds (*create_fms_packet)();
    sds (*create_radio_packet)();
    sds (*create_robot_packet)();

    int (*read_fms_packet) (const sds);
    int (*read_radio_packet) (const sds);
    int (*read_robot_packet) (const sds);

    void (*reset_fms)();
    void (*reset_radio)();
    void (*reset_robot)();

    void (*reboot_robot)();
    void (*restart_robot_code)();

    int fms_interval;
    int radio_interval;
    int robot_interval;

    int max_joysticks;
    int max_axis_count;
    int max_hat_count;
    int max_button_count;
    float max_battery_voltage;

    DS_Socket fms_socket;
    DS_Socket radio_socket;
    DS_Socket robot_socket;
    DS_Socket netconsole_socket;
} DS_Protocol;

extern void Protocols_Init();
extern void Protocols_Close();
extern void DS_ConfigureProtocol (DS_Protocol* ptr);

extern int DS_SentFMSPackets();
extern int DS_SentRadioPackets();
extern int DS_SentRobotPackets();

extern int DS_ReceivedFMSPackets();
extern int DS_ReceivedRadioPackets();
extern int DS_ReceivedRobotPackets();

extern DS_Protocol* DS_CurrentProtocol();

#ifdef __cplusplus
}
#endif

#endif

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

#ifndef _LIB_DS_PROTOCOL_H
#define _LIB_DS_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DS_Socket.h"
#include "DS_String.h"

typedef struct _protocol {
    DS_String name;
    DS_String (*fms_address) (void);
    DS_String (*radio_address) (void);
    DS_String (*robot_address) (void);

    DS_String (*create_fms_packet) (void);
    DS_String (*create_radio_packet) (void);
    DS_String (*create_robot_packet) (void);

    int (*read_fms_packet) (const DS_String*);
    int (*read_radio_packet) (const DS_String*);
    int (*read_robot_packet) (const DS_String*);

    void (*reset_fms) (void);
    void (*reset_radio) (void);
    void (*reset_robot) (void);

    void (*reboot_robot) (void);
    void (*restart_robot_code) (void);

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
extern void DS_ConfigureProtocol (const DS_Protocol* ptr);

extern unsigned long DS_SentFMSBytes();
extern unsigned long DS_SentRadioBytes();
extern unsigned long DS_SentRobotBytes();

extern unsigned long DS_ReceivedFMSBytes();
extern unsigned long DS_ReceivedRadioBytes();
extern unsigned long DS_ReceivedRobotBytes();

extern int DS_SentFMSPackets();
extern int DS_SentRadioPackets();
extern int DS_SentRobotPackets();

extern int DS_ReceivedFMSPackets();
extern int DS_ReceivedRadioPackets();
extern int DS_ReceivedRobotPackets();

extern void DS_ResetFMSPackets();
extern void DS_ResetRadioPackets();
extern void DS_ResetRobotPackets();

extern DS_Protocol* DS_CurrentProtocol();

#ifdef __cplusplus
}
#endif

#endif

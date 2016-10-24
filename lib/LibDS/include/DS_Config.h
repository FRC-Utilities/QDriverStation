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

#ifndef _LIB_DS_INTERNAL_H
#define _LIB_DS_INTERNAL_H

#include <stdint.h>

#include "DS_Types.h"
#include "DS_Socket.h"

/*
 * WARNING:
 *
 * DO NOT use this header in your application unless you are implementing your
 * own protocols. Only include this header where it is required, do not include
 * it for the lolz.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define RECONFIGURE_FMS   0x01
#define RECONFIGURE_RADIO 0x02
#define RECONFIGURE_ROBOT 0x04
#define RECONFIGURE_ALL   0x01 | 0x02 | 0x04

/* Misc */
extern void CFG_ReconfigureAddresses (const int flags);

/* Getters */
extern int CFG_GetTeamNumber();
extern int CFG_GetRobotCode();
extern int CFG_GetRobotEnabled();
extern int CFG_GetRobotCPUUsage();
extern int CFG_GetRobotRAMUsage();
extern int CFG_GetCANUtilization();
extern int CFG_GetRobotDiskUsage();
extern float CFG_GetRobotVoltage();
extern DS_Alliance CFG_GetAlliance();
extern DS_Position CFG_GetPosition();
extern int CFG_GetEmergencyStopped();
extern int CFG_GetFMSCommunications();
extern int CFG_GetRadioCommunications();
extern int CFG_GetRobotCommunications();
extern DS_ControlMode CFG_GetControlMode();

/* Setters */
extern void CFG_SetRobotCode (const int code);
extern void CFG_SetTeamNumber (const int number);
extern void CFG_SetRobotEnabled (const int enabled);
extern void CFG_SetRobotCPUUsage (const int percent);
extern void CFG_SetRobotRAMUsage (const int percent);
extern void CFG_SetRobotDiskUsage (const int percent);
extern void CFG_SetRobotVoltage (const float voltage);
extern void CFG_SetEmergencyStopped (const int stopped);
extern void CFG_SetAlliance (const DS_Alliance alliance);
extern void CFG_SetPosition (const DS_Position position);
extern void CFG_SetCANUtilization (const int utilization);
extern void CFG_SetControlMode (const DS_ControlMode mode);
extern void CFG_SetFMSCommunications (const int communications);
extern void CFG_SetRadioCommunications (const int communications);
extern void CFG_SetRobotCommunications (const int communications);

/* Watchdog functions */
extern void CFG_FMSWatchdogExpired();
extern void CFG_RadioWatchdogExpired();
extern void CFG_RobotWatchdogExpired();

#ifdef __cplusplus
}
#endif

#endif

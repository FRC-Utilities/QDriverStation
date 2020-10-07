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

#ifndef _LIB_DS_CLIENT_H
#define _LIB_DS_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DS_Types.h"

/* Init/Close functions */
extern void Client_Init (void);
extern void Client_Close (void);

/* User-set addresses */
extern char* DS_GetCustomFMSAddress (void);
extern char* DS_GetCustomRadioAddress (void);
extern char* DS_GetCustomRobotAddress (void);

/* Protocol-set addresses */
extern char* DS_GetDefaultFMSAddress (void);
extern char* DS_GetDefaultRadioAddress (void);
extern char* DS_GetDefaultRobotAddress (void);

/* Used addresses */
extern char* DS_GetAppliedFMSAddress (void);
extern char* DS_GetAppliedRadioAddress (void);
extern char* DS_GetAppliedRobotAddress (void);

/* Game data */
extern char* DS_GetGameData (void);

/* Status string */
extern char* DS_GetStatusString (void);

/* Getters */
extern int DS_GetTeamNumber (void);
extern int DS_GetRobotCode (void);
extern int DS_GetCanBeEnabled (void);
extern int DS_GetRobotEnabled (void);
extern int DS_GetRobotCPUUsage (void);
extern int DS_GetRobotRAMUsage (void);
extern int DS_GetRobotDiskUsage (void);
extern float DS_GetRobotVoltage (void);
extern DS_Alliance DS_GetAlliance (void);
extern DS_Position DS_GetPosition (void);
extern int DS_GetEmergencyStopped (void);
extern int DS_GetFMSCommunications (void);
extern int DS_GetRadioCommunications (void);
extern int DS_GetRobotCommunications (void);
extern int DS_GetRobotCANUtilization (void);
extern DS_ControlMode DS_GetControlMode (void);
extern float DS_GetMaximumBatteryVoltage (void);

/* Setters */
extern void DS_RebootRobot (void);
extern void DS_RestartRobotCode (void);
extern void DS_SetGameData (const char* data);
extern void DS_SetTeamNumber (const int team);
extern void DS_SetRobotEnabled (const int enabled);
extern void DS_SetEmergencyStopped (const int stop);
extern void DS_SetAlliance (const DS_Alliance alliance);
extern void DS_SetPosition (const DS_Position position);
extern void DS_SetControlMode (const DS_ControlMode mode);
extern void DS_SetCustomFMSAddress (const char* address);
extern void DS_SetCustomRadioAddress (const char* address);
extern void DS_SetCustomRobotAddress (const char* address);
extern void DS_SendNetConsoleMessage (const char* message);

#ifdef __cplusplus
}
#endif

#endif

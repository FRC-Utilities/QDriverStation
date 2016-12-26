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

#ifndef _LIB_DS_CLIENT_H
#define _LIB_DS_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bstrlib.h>
#include "DS_Types.h"

/* Init/Close functions */
extern void Client_Init();
extern void Client_Close();

/* User-set addresses */
extern bstring DS_GetCustomFMSAddress();
extern bstring DS_GetCustomRadioAddress();
extern bstring DS_GetCustomRobotAddress();

/* Protocol-set addresses */
extern bstring DS_GetDefaultFMSAddress();
extern bstring DS_GetDefaultRadioAddress();
extern bstring DS_GetDefaultRobotAddress();

/* Used addresses */
extern bstring DS_GetAppliedFMSAddress();
extern bstring DS_GetAppliedRadioAddress();
extern bstring DS_GetAppliedRobotAddress();

/* Status string */
extern bstring DS_GetStatusString();

/* Getters */
extern int DS_GetTeamNumber();
extern int DS_GetRobotCode();
extern int DS_GetCanBeEnabled();
extern int DS_GetRobotEnabled();
extern int DS_GetRobotCPUUsage();
extern int DS_GetRobotRAMUsage();
extern int DS_GetRobotDiskUsage();
extern float DS_GetRobotVoltage();
extern DS_Alliance DS_GetAlliance();
extern DS_Position DS_GetPosition();
extern int DS_GetEmergencyStopped();
extern int DS_GetFMSCommunications();
extern int DS_GetRadioCommunications();
extern int DS_GetRobotCommunications();
extern int DS_GetRobotCANUtilization();
extern DS_ControlMode DS_GetControlMode();
extern float DS_GetMaximumBatteryVoltage();

/* Setters */
extern void DS_RebootRobot();
extern void DS_RestartRobotCode();
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

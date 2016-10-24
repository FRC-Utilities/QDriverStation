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

#include "DS_Utils.h"
#include "DS_Client.h"
#include "DS_Config.h"
#include "DS_Protocol.h"

#include <stdio.h>
#include <string.h>

static sds status_string;
static sds custom_fms_address;
static sds custom_radio_address;
static sds custom_robot_address;

/**
 * Allocates memory for the members of the client module
 */
void Client_Init()
{
    status_string = sdsempty();
    custom_fms_address = sdsempty();
    custom_radio_address = sdsempty();
    custom_robot_address = sdsempty();
}

/**
 * Frees up the memory used by the members of the client module
 */
void Client_Close()
{
    DS_FREESTR (status_string);
    DS_FREESTR (custom_fms_address);
    DS_FREESTR (custom_radio_address);
    DS_FREESTR (custom_robot_address);
}

/**
 * Returns the user-set FMS address.
 * This value may be empty, if that's the case, then the Driver Station will
 * use the addresses specified by the currently loaded protocol.
 */
sds DS_GetCustomFMSAddress()
{
    return custom_fms_address;
}

/**
 * Returns the user-set radio address.
 * This value may be empty, if that's the case, then the Driver Station will
 * use the addresses specified by the currently loaded protocol.
 */
sds DS_GetCustomRadioAddress()
{
    return custom_radio_address;
}

/**
 * Returns the user-set robot address
 * This value may be empty, if that's the case, then the Driver Station will
 * use the addresses specified by the currently loaded protocol.
 */
sds DS_GetCustomRobotAddress()
{
    return custom_robot_address;
}

/**
 * Returns the protocol-set FMS address, this address may change when the team
 * number is changed, if your application relies on this value, consider
 * updating in regurarly or using the events system of the LibDS.
 */
sds DS_GetDefaultFMSAddress()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->fms_address();

    return sdsempty();
}

/**
 * Returns the protocol-set radio address, this address may change when the
 * team number is changed, if your application relies on this value, consider
 * updating in regurarly or using the events system of the LibDS.
 */
sds DS_GetDefaultRadioAddress()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->radio_address();

    return sdsempty();
}

/**
 * Returns the protocol-set robot address, this address may change when the
 * team number is changed, if your application relies on this value, consider
 * updating in regurarly or using the events system of the LibDS.
 */
sds DS_GetDefaultRobotAddress()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->robot_address();

    return sdsempty();
}

/**
 * Returns the address used to communicate with the FMS.
 * If the user-set address is not empty, then this function will return the
 * user-set address. Otherwise, this function will return the address
 * specified  by the currently loaded protocol.
 */
sds DS_GetAppliedFMSAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomFMSAddress()))
        return DS_GetDefaultFMSAddress();

    return DS_GetCustomFMSAddress();
}

/**
 * Returns the address used to communicate with the bridge.
 * If the user-set address is not empty, then this function will return the
 * user-set address. Otherwise, this function will return the address
 * specified  by the currently loaded protocol.
 */
sds DS_GetAppliedRadioAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomRadioAddress()))
        return DS_GetDefaultRadioAddress();

    return DS_GetCustomRadioAddress();
}

/**
 * Returns the address used to communicate with the robot.
 * If the user-set address is not empty, then this function will return the
 * user-set address. Otherwise, this function will return the address
 * specified  by the currently loaded protocol.
 */
sds DS_GetAppliedRobotAddress()
{
    if (DS_StringIsEmpty (DS_GetCustomRobotAddress()))
        return DS_GetDefaultRobotAddress();

    return DS_GetCustomRobotAddress();
}

/**
 * Returns the current status of the robot/DS.
 * This string is meant to be used directly by the clien application,
 * the DS has no real use for it.
 *
 * Possible return values are:
 *    - No Robot Communications
 *    - No Robot Code
 *    - Emergency Stopped
 *    - Teleoperated Enabled/Disabled
 *    - Autonomous Enabled/Disabled
 *    - Test Enabled/Disabled
 */
sds DS_GetStatusString()
{
    DS_FREESTR (status_string);

    if (!DS_GetRobotCommunications())
        status_string = sdsnew ("No Robot Communications");

    else if (!DS_GetRobotCode())
        status_string = sdsnew ("No Robot Code");

    else if (DS_GetEmergencyStopped())
        status_string = sdsnew ("Emergency Stopped");

    else {
        switch (DS_GetControlMode()) {
        case DS_CONTROL_TELEOPERATED:
            status_string = sdsnew ("Teleoperated");
            break;
        case DS_CONTROL_AUTONOMOUS:
            status_string = sdsnew ("Autonomous");
            break;
        case DS_CONTROL_TEST:
            status_string = sdsnew ("Test");
            break;
        }

        sds enabled = sdsnew (DS_GetRobotEnabled() ? "Enabled" : "Disabled");
        status_string = sdscatprintf (status_string, " %s", enabled);
        DS_FREESTR (enabled);
    }

    return status_string;
}

/**
 * Returns the current team number
 */
int DS_GetTeamNumber()
{
    return CFG_GetTeamNumber();
}

/**
 * Returns \c 1 if the robot code is running
 */
int DS_GetRobotCode()
{
    return CFG_GetRobotCode();
}

/**
 * Returns \c 1 if the conditions to enable a robot are met.
 * We consider these parameters to determine if it is safe to enable a robot:
 *    - We have communications with the robot
 *    - The robot is **not** emergency stopped
 *    - The protocol reports that the robot code is running
 */
int DS_GetCanBeEnabled()
{
    return DS_GetRobotCode()
            && !DS_GetEmergencyStopped()
            && DS_GetRobotCommunications();
}

/**
 * Returns \c 1 if the robot can be enabled, otherwise, this function will
 * return \c 0.
 */
int DS_GetRobotEnabled()
{
    return CFG_GetRobotEnabled();
}

/**
 * Returns the current CPU usage of the robot
 */
int DS_GetRobotCPUUsage()
{
    return CFG_GetRobotCPUUsage();
}

/**
 * Returns the current RAM usage of the robot
 */
int DS_GetRobotRAMUsage()
{
    return CFG_GetRobotRAMUsage();
}

/**
 * Returns the current disk usage of the robot
 */
int DS_GetRobotDiskUsage()
{
    return CFG_GetRobotDiskUsage();
}

/**
 * Returns the current voltage of the robot
 */
float DS_GetRobotVoltage()
{
    return CFG_GetRobotVoltage();
}

/**
 * Returns the current alliance of the robot.
 * This value can be changed by the user or the FMS.
 */
DS_Alliance DS_GetAlliance()
{
    return CFG_GetAlliance();
}

/**
 * Returns the current position of the robot.
 * This value can be changed by the user or the FMS.
 */
DS_Position DS_GetPosition()
{
    return CFG_GetPosition();
}

/**
 * Returns \c 1 if the robot is emergency stopped
 */
int DS_GetEmergencyStopped()
{
    return CFG_GetEmergencyStopped();
}

/**
 * Returns \c 1 if the LibDS has communications with the FMS
 */
int DS_GetFMSCommunications()
{
    return CFG_GetFMSCommunications();
}

/**
 * Returns \c 1 if the LibDS has communications with the radio
 */
int DS_GetRadioCommunications()
{
    return CFG_GetRadioCommunications();
}

/**
 * Returns \c 1 if the LibDS has communications with the robot
 */
int DS_GetRobotCommunications()
{
    return CFG_GetRobotCommunications();
}

/**
 * Returns the current CAN utilization of the robot
 */
int DS_GetRobotCANUtilization()
{
    return CFG_GetCANUtilization();
}

/**
 * Returns the current control mode of the robot
 */
DS_ControlMode DS_GetControlMode()
{
    return CFG_GetControlMode();
}

/**
 * Returns the maximum battery voltage specified by the current protocol
 */
float DS_GetMaximumBatteryVoltage()
{
    if (DS_CurrentProtocol())
        return DS_CurrentProtocol()->max_battery_voltage;

    return 0.0;
}

/**
 * Instructs the current protocol to reboot the robot
 */
void DS_RebootRobot()
{
    if (DS_CurrentProtocol())
        DS_CurrentProtocol()->reboot_robot();
}

/**
 * Instructs the current protocol to restart the robot code
 */
void DS_RestartRobotCode()
{
    if (DS_CurrentProtocol())
        DS_CurrentProtocol()->restart_robot_code();
}

/**
 * Changes the \a team number, which is used by the protocols to find the robot
 * and (in some cases), to construct the packet
 */
void DS_SetTeamNumber (const int team)
{
    CFG_SetTeamNumber (team);
}

/**
 * Changes the \a enabled state of the robot
 */
void DS_SetRobotEnabled (const int enabled)
{
    CFG_SetRobotEnabled (enabled);
}

/**
 * Changes the emergency stop state of the robot
 */
void DS_SetEmergencyStopped (const int stop)
{
    CFG_SetEmergencyStopped (stop);
}

/**
 * Changes the team \a alliance of the robot
 */
void DS_SetAlliance (const DS_Alliance alliance)
{
    CFG_SetAlliance (alliance);
}

/**
 * Changes the robot's \a position in the field
 */
void DS_SetPosition (const DS_Position position)
{
    CFG_SetPosition (position);
}

/**
 * Changes the control \a mode of the robot
 */
void DS_SetControlMode (const DS_ControlMode mode)
{
    CFG_SetControlMode (mode);
}

/**
 * Changes the \a address used to communicate with the FMS
 */
void DS_SetCustomFMSAddress (const char* address)
{
    if (strlen (address) > 0) {
        DS_FREESTR (custom_fms_address);
        custom_fms_address = sdsnew (address);

        CFG_ReconfigureAddresses (RECONFIGURE_FMS);
    }
}

/**
 * Changes the \a address used to communicate with the radio
 */
void DS_SetCustomRadioAddress (const char* address)
{
    if (strlen (address) > 0) {
        DS_FREESTR (custom_radio_address);
        custom_radio_address = sdsnew (address);

        CFG_ReconfigureAddresses (RECONFIGURE_RADIO);
    }
}

/**
 * Changes the \a address used to communicate with the robot
 */
void DS_SetCustomRobotAddress (const char* address)
{
    if (strlen (address) > 0) {
        DS_FREESTR (custom_robot_address);
        custom_robot_address = sdsnew (address);

        CFG_ReconfigureAddresses (RECONFIGURE_ROBOT);
    }
}

/**
 * Sends the given \a message to the NetConsole of the robot
 */
void DS_SendNetConsoleMessage (const char* message)
{
    if (DS_CurrentProtocol())
        DS_SocketSend (&DS_CurrentProtocol()->netconsole_socket,
                       sdsnew (message));
}

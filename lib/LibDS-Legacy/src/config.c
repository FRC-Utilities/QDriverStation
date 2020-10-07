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

#include "DS_Utils.h"
#include "DS_Client.h"
#include "DS_Events.h"
#include "DS_Config.h"
#include "DS_Protocol.h"

#include <math.h>
#include <string.h>
#include <assert.h>

/*
 * These variables hold the state(s) of the LibDS and its modules
 */
static int team = 0;
static int cpu_usage = -1;
static int ram_usage = -1;
static int disk_usage = -1;
static int robot_code = -1;
static DS_String game_data;
static int robot_enabled = -1;
static int can_utilization = -1;
static float robot_voltage = -1;
static int emergency_stopped = -1;
static int fms_communications = -1;
static int radio_communications = -1;
static int robot_communications = -1;
static DS_Position robot_position = DS_POSITION_1;
static DS_Alliance robot_alliance = DS_ALLIANCE_RED;
static DS_ControlMode control_mode = DS_CONTROL_TELEOPERATED;

/**
 * Ensures that the given \a input number is either \c 0 or \c 1
 */
static int to_boolean (const int input)
{
    return (input > 0) ? 1 : 0;
}

/**
 * Ensures that the given \a input stays within the range established by the
 * \a min and \a max numbers
 *
 * \param input the number to check
 * \param min the minimum value that the input number should have
 * \param max the maximun value that the input number should have
 */
static int respect_range (const int input, const int min, const int max)
{
    if (input < min)
        return min;

    else if (input > max)
        return max;

    return input;
}

/**
 * Creates and fills a robot event with the given \a type header
 */
static void create_robot_event (const DS_EventType type)
{
    DS_Event event;

    event.robot.type = type;
    event.robot.code = CFG_GetRobotCode();
    event.robot.mode = CFG_GetControlMode();
    event.robot.enabled = CFG_GetRobotEnabled();
    event.robot.voltage = CFG_GetRobotVoltage();
    event.robot.can_util = CFG_GetCANUtilization();
    event.robot.cpu_usage = CFG_GetRobotCPUUsage();
    event.robot.ram_usage = CFG_GetRobotRAMUsage();
    event.robot.disk_usage = CFG_GetRobotDiskUsage();
    event.robot.estopped = CFG_GetEmergencyStopped();
    event.robot.connected = CFG_GetRobotCommunications();

    DS_AddEvent (&event);
}

/**
 * Notifies the user about something through the NetConsole
 */
void CFG_AddNotification (const DS_String* msg)
{
    /* Check arguments */
    assert (msg);

    /* Create and display notification string */
    char* cstr = DS_StrToChar (msg);
    DS_String str = DS_StrFormat ("<font color=#888>** LibDS: %s</font>", cstr);
    CFG_AddNetConsoleMessage (&str);

    /* Delete the string buffer */
    DS_StrRmBuf (&str);
    DS_FREE (cstr);
}

/**
 * Notifies the application of a new NetConsole message through the
 * DS events system
 *
 * \a msg the message to display
 */
void CFG_AddNetConsoleMessage (const DS_String* msg)
{
    /* Check arguments */
    assert (msg);

    /* Register new NetConsole event */
    DS_Event event;
    event.netconsole.type = DS_NETCONSOLE_NEW_MESSAGE;
    event.netconsole.message = DS_StrToChar (msg);
    DS_AddEvent (&event);
}

/**
 * Re-applies the network addresses of the FMS, radio and robot.
 * This function is called when the team number is changed or when a watchdog
 * expires (to force the sockets module to perform a lookup)
 */
void CFG_ReconfigureAddresses (const int flags)
{
    if (!DS_CurrentProtocol())
        return;

    if (flags & RECONFIGURE_FMS) {
        char* address = DS_GetAppliedFMSAddress();
        DS_SocketChangeAddress (&DS_CurrentProtocol()->fms_socket, address);
        DS_FREE (address);
    }

    if (flags & RECONFIGURE_RADIO) {
        char* address = DS_GetAppliedRadioAddress();
        DS_SocketChangeAddress (&DS_CurrentProtocol()->radio_socket, address);
        DS_FREE (address);
    }

    if (flags & RECONFIGURE_ROBOT) {
        char* address = DS_GetAppliedRobotAddress();
        DS_SocketChangeAddress (&DS_CurrentProtocol()->robot_socket, address);
        DS_FREE (address);
    }
}

/**
 * Returns the current team number, which may be used by the protocols to
 * specifiy the default addresses and generate specialized packets
 */
int CFG_GetTeamNumber (void)
{
    return DS_Max (team, 0);
}

/**
 * Returns \c 0 if there is no robot code running, otherwise, it returns \c 1
 */
int CFG_GetRobotCode (void)
{
    return robot_code == 1;
}

/**
 * Returns \c 1 if the robot is enabled, otherwise, it returns \c 0
 */
int CFG_GetRobotEnabled (void)
{
    return robot_enabled == 1;
}

/**
 * Returns the current CPU usage of the robot
 */
int CFG_GetRobotCPUUsage (void)
{
    return DS_Max (cpu_usage, 0);
}

/**
 * Returns the current RAM usage of the robot
 */
int CFG_GetRobotRAMUsage (void)
{
    return DS_Max (ram_usage, 0);
}

/**
 * Returns the current utilization of the robot's CAN-BUS
 */
int CFG_GetCANUtilization (void)
{
    return DS_Max (can_utilization, 0);
}

/**
 * Returns the current disk usage of the robot
 */
int CFG_GetRobotDiskUsage (void)
{
    return DS_Max (disk_usage, 0);
}

/**
 * Returns the current voltage of the robot
 */
float CFG_GetRobotVoltage (void)
{
    return DS_Max (robot_voltage, 0);
}

/**
 * Returns the current game data string
 */
DS_String* CFG_GetGameData (void)
{
    return &game_data;
}

/**
 * Returns the current alliance of the robot, possible values are:
 *    - \c DS_ALLIANCE_RED
 *    - \c DS_ALLIANCE_BLUE
 */
DS_Alliance CFG_GetAlliance (void)
{
    return robot_alliance;
}

/**
 * Returns the current team position of the robot, possible values are:
 *    - \c DS_POSITION_1
 *    - \c DS_POSITION_2
 *    - \c DS_POSITION_3
 */
DS_Position CFG_GetPosition (void)
{
    return robot_position;
}

/**
 * Returns \c 1 if the robot is emergency stopped, otherwise, it returns \c 0
 */
int CFG_GetEmergencyStopped (void)
{
    return emergency_stopped == 1;
}

/**
 * Returns \c 1 if the client has communications with the FMS, otherwise,
 * it returns \c 0
 */
int CFG_GetFMSCommunications (void)
{
    return fms_communications == 1;
}

/**
 * Returns \c 1 if the client has communications with the radio, otherwise,
 * it returns \c 0
 */
int CFG_GetRadioCommunications (void)
{
    return radio_communications == 1;
}

/**
 * Returns \c 1 if the client has communications with the robot, otherwise,
 * it returns \c 0
 */
int CFG_GetRobotCommunications (void)
{
    return robot_communications == 1;
}

/**
 * Returns the current control mode of the robot, possible values are:
 *    - \c DS_CONTROL_TEST
 *    - \c DS_CONTROL_AUTONOMOUS
 *    - \c DS_CONTROL_TELEOPERATED
 */
DS_ControlMode CFG_GetControlMode (void)
{
    return control_mode;
}

/**
 * Updates the available state of the robot code
 */
void CFG_SetRobotCode (const int code)
{
    if (robot_code != to_boolean (code)) {
        robot_code = to_boolean (code);
        create_robot_event (DS_ROBOT_CODE_CHANGED);
        create_robot_event (DS_STATUS_STRING_CHANGED);
    }
}

/**
 * Updates the game \a data string
 */
void CFG_SetGameData (const char* data)
{
    /* Check arguments */
    assert (data);

    /* Update game data */
    DS_StrRmBuf (&game_data);
    game_data = DS_StrNew (data);
}

/**
 * Updates the team \a number
 */
void CFG_SetTeamNumber (const int number)
{
    if (team != number) {
        team = number;
        CFG_ReconfigureAddresses (RECONFIGURE_ALL);
    }
}

/**
 * Updates the robot's \a enabled state
 */
void CFG_SetRobotEnabled (const int enabled)
{
    if (robot_enabled != to_boolean (enabled)) {
        robot_enabled = to_boolean (enabled) && !CFG_GetEmergencyStopped();
        create_robot_event (DS_ROBOT_ENABLED_CHANGED);
        create_robot_event (DS_STATUS_STRING_CHANGED);
    }
}

/**
 * Updates the robot's CPU usage.
 * You must input a value between \c 0 and \c 100
 */
void CFG_SetRobotCPUUsage (const int percent)
{
    if (cpu_usage != percent) {
        cpu_usage = respect_range (percent, 0, 100);
        create_robot_event (DS_ROBOT_CPU_INFO_CHANGED);
    }
}

/**
 * Updates the robot's RAM/memory usage.
 * You must input a value between \c 0 and \c 100
 */
void CFG_SetRobotRAMUsage (const int percent)
{
    if (ram_usage != percent) {
        ram_usage = respect_range (percent, 0, 100);
        create_robot_event (DS_ROBOT_RAM_INFO_CHANGED);
    }
}

/**
 * Updates the robot's disk usage.
 * You must input a value between \c 0 and \c 100
 */
void CFG_SetRobotDiskUsage (const int percent)
{
    if (disk_usage != percent) {
        disk_usage = respect_range (percent, 0, 100);
        create_robot_event (DS_ROBOT_DISK_INFO_CHANGED);
    }
}

/**
 * Updates the robot's \a voltage, there are no range limits
 */
void CFG_SetRobotVoltage (const float voltage)
{
    if (robot_voltage != voltage) {
        robot_voltage = roundf (voltage * 100) / 100;
        create_robot_event (DS_ROBOT_VOLTAGE_CHANGED);
    }
}

/**
 * Updates the emergency \a stopped state of the robot.
 */
void CFG_SetEmergencyStopped (const int stopped)
{
    if (emergency_stopped != to_boolean (stopped)) {
        emergency_stopped = to_boolean (stopped);
        create_robot_event (DS_ROBOT_ESTOP_CHANGED);
        create_robot_event (DS_STATUS_STRING_CHANGED);
    }
}

/**
 * Changes the \a alliance of the robot
 */
void CFG_SetAlliance (const DS_Alliance alliance)
{
    if (robot_alliance != alliance) {
        robot_alliance = alliance;
        create_robot_event (DS_ROBOT_STATION_CHANGED);
    }
}

/**
 * Changes the \a position of the robot
 */
void CFG_SetPosition (const DS_Position position)
{
    if (robot_position != position) {
        robot_position = position;
        create_robot_event (DS_ROBOT_STATION_CHANGED);
    }
}

/**
 * Updates the CAN \a utilization
 */
void CFG_SetCANUtilization (const int utilization)
{
    if (can_utilization != utilization) {
        can_utilization = utilization;
        create_robot_event (DS_ROBOT_CAN_UTIL_CHANGED);
    }
}

/**
 * Changes the control \a mode of the robot
 */
void CFG_SetControlMode (const DS_ControlMode mode)
{
    if (control_mode != mode) {
        control_mode = mode;
        create_robot_event (DS_ROBOT_MODE_CHANGED);
        create_robot_event (DS_STATUS_STRING_CHANGED);
    }
}

/**
 * Updates the state of the FMS communications.
 */
void CFG_SetFMSCommunications (const int communications)
{
    if (fms_communications != to_boolean (communications)) {
        fms_communications = to_boolean (communications);

        DS_Event event;
        event.fms.type = DS_FMS_COMMS_CHANGED;
        event.fms.connected = fms_communications;
        DS_AddEvent (&event);

        DS_ResetFMSPackets();
    }
}

/**
 * Updates the state of the radio communications.
 */
void CFG_SetRadioCommunications (const int communications)
{
    if (radio_communications != to_boolean (communications)) {
        radio_communications = to_boolean (communications);

        DS_Event event;
        event.radio.type = DS_RADIO_COMMS_CHANGED;
        event.radio.connected = fms_communications;
        DS_AddEvent (&event);

        DS_ResetRadioPackets();
    }
}

/**
 * Updates the state of the robot communications.
 */
void CFG_SetRobotCommunications (const int communications)
{
    if (robot_communications != to_boolean (communications)) {
        robot_communications = to_boolean (communications);
        create_robot_event (DS_ROBOT_COMMS_CHANGED);
        create_robot_event (DS_STATUS_STRING_CHANGED);

        DS_ResetRobotPackets();
    }
}

/**
 * Called when the FMS watchdog expires
 */
void CFG_FMSWatchdogExpired (void)
{
    CFG_SetFMSCommunications (0);
    CFG_ReconfigureAddresses (RECONFIGURE_FMS);
}

/**
 * Called when the radio watchdog expires
 */
void CFG_RadioWatchdogExpired (void)
{
    CFG_SetRadioCommunications (0);
    CFG_ReconfigureAddresses (RECONFIGURE_RADIO);
}

/**
 * Called when the robot watchdog expires
 */
void CFG_RobotWatchdogExpired (void)
{
    /* Reset everything to safe state */
    CFG_SetRobotCode (0);
    CFG_SetRobotVoltage (0);
    CFG_SetRobotEnabled (0);
    CFG_SetRobotCPUUsage (0);
    CFG_SetRobotRAMUsage (0);
    CFG_SetRobotDiskUsage (0);
    CFG_SetEmergencyStopped (0);
    CFG_SetRobotCommunications (0);

    /* Force the sockets to perform another lookup */
    CFG_ReconfigureAddresses (RECONFIGURE_ROBOT);

    /* Update the status label */
    create_robot_event (DS_STATUS_STRING_CHANGED);
}

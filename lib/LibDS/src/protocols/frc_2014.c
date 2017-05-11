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

#include <math.h>

#include "DS_Utils.h"
#include "DS_Config.h"
#include "DS_Protocol.h"
#include "DS_Joysticks.h"
#include "DS_DefaultProtocols.h"

/*
 * Protocol bytes
 */
static const uint8_t cEnabled          = 0x20;
static const uint8_t cTestMode         = 0x02;
static const uint8_t cAutonomous       = 0x10;
static const uint8_t cTeleoperated     = 0x00;
static const uint8_t cFMS_Attached     = 0x08;
static const uint8_t cResyncComms      = 0x04;
static const uint8_t cRebootRobot      = 0x80;
static const uint8_t cEmergencyStopOn  = 0x00;
static const uint8_t cEmergencyStopOff = 0x40;
static const uint8_t cPosition1        = 0x31;
static const uint8_t cPosition2        = 0x32;
static const uint8_t cPosition3        = 0x33;
static const uint8_t cAllianceRed      = 0x52;
static const uint8_t cAllianceBlue     = 0x42;
static const uint8_t cFMSAutonomous    = 0x53;
static const uint8_t cFMSTeleoperated  = 0x43;

/*
 * Sent robot packet counters, they are used as packet IDs
 */
static unsigned int sent_robot_packets = 0;

/*
 * Joystick properties
 */
static int max_axes = 6;
static int max_hats = 0;
static int max_buttons = 10;
static int max_joysticks = 4;

/*
 * Control code flags
 */
static int resync = 1;
static int reboot = 0;
static int restart_code = 0;

/**
 * Gets the alliance type from the received \a byte
 * This function is used to update the robot configuration when receiving data
 * from the FMS
 */
static DS_Alliance get_alliance (const uint8_t byte)
{
    if (byte == cAllianceRed)
        return DS_ALLIANCE_RED;

    return DS_ALLIANCE_BLUE;
}

/**
 * Gets the position type from the received \a byte
 * This function is used to update the robot configuration when receiving data
 * from the FMS
 */
static DS_Position get_position (const uint8_t byte)
{
    if (byte == cPosition1)
        return DS_POSITION_1;

    if (byte == cPosition2)
        return DS_POSITION_2;

    if (byte == cPosition3)
        return DS_POSITION_3;

    return DS_POSITION_1;
}

/**
 * Returns the control code sent to the robot. The control code holds the
 * following information:
 *     - The emergency stop state
 *     - The enabled state of the robot
 *     - The control mode of the robot
 *     - The FMS communication state (the robot wants it)
 *     - Extra commands to the robot (e.g. reboot & resync)
 */
static uint8_t get_control_code (void)
{
    uint8_t code = cEmergencyStopOff;
    uint8_t enabled = CFG_GetRobotEnabled() ? cEnabled : 0x00;

    /* Get the control mode (Test, Auto or TeleOp) */
    switch (CFG_GetControlMode()) {
    case DS_CONTROL_TEST:
        code |= enabled + cTestMode;
        break;
    case DS_CONTROL_AUTONOMOUS:
        code |= enabled + cAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        code |= enabled + cTeleoperated;
        break;
    default:
        code = cEmergencyStopOff;
        break;
    }

    /* Resync robot communications */
    if (resync)
        code |= cResyncComms;

    /* Let robot know if we are connected to FMS */
    if (CFG_GetFMSCommunications())
        code |= cFMS_Attached;

    /* Set the emergency stop state */
    if (CFG_GetEmergencyStopped())
        code = cEmergencyStopOn;

    /* Send the reboot code if required */
    if (reboot)
        code = cRebootRobot;

    return code;
}

/**
 * Returns the alliance code sent to the robot.
 * The robot application can use this information to adjust its programming for
 * the current alliance.
 */
static uint8_t get_alliance_code (void)
{
    if (CFG_GetAlliance() == DS_ALLIANCE_RED)
        return cAllianceRed;

    return cAllianceBlue;
}

/**
 * Returns the alliance position code sent to the robot.
 */
static uint8_t get_position_code (void)
{
    uint8_t code = cPosition1;

    switch (CFG_GetPosition()) {
    case DS_POSITION_1:
        code = cPosition1;
        break;
    case DS_POSITION_2:
        code = cPosition2;
        break;
    case DS_POSITION_3:
        code = cPosition3;
        break;
    }

    return code;
}

/**
 * Returns the (number?) of digital inputs connected to the computer.
 */
static uint8_t get_digital_inputs (void)
{
    return 0x00;
}

/**
 * Adds joystick information to a DS-to-robot packet, beginning at the given
 * \a offset in the data packet.
 *
 * The 2014 communication protocol records the data for all four joysticks,
 * if a joystick or joystick member is not present, we will send a neutral
 * value (\c 0.00 for axes, \c 0 for buttons).
 *
 * Axis value range is -127 to 128, the robot program will then adjust those
 * values to a float range from -1 to 1.
 *
 * Button states are stored in a similar way as enumerated flags in a C/C++
 * program.
 */
static DS_String get_joystick_data (void)
{
    /* Initialize variables */
    int i = 0;
    int j = 0;
    DS_String buf = DS_StrNewLen (0);

    /* Add data for every joystick */
    for (i = 0; i < max_joysticks; ++i) {
        /* Add axis data */
        for (j = 0; j < max_axes; ++j)
            DS_StrAppend (&buf, DS_FloatToByte (DS_GetJoystickAxis (i, j), 1));

        /* Generate button data */
        uint16_t button_flags = 0;
        for (j = 0; j < max_buttons; ++j)
            button_flags += (uint16_t) DS_GetJoystickButton (i, j) ? j * j : 0;

        /* Add button data */
        DS_StrAppend (&buf, (button_flags & 0xff00) >> 8);
        DS_StrAppend (&buf, (button_flags & 0xff));
    }

    return buf;
}

/**
 * The FMS address is not defined, it will be assigned automatically when the
 * DS receives a FMS packet
 */
static DS_String fms_address (void)
{
    return DS_StrNew (DS_FallBackAddress);
}

/**
 * The 2014 control systems assigns the radio IP in 10.te.am.1
 */
static DS_String radio_address (void)
{
    return DS_GetStaticIP (10, CFG_GetTeamNumber(), 1);
}

/**
 * The 2014 control systems assigns the radio IP in 10.te.am.2
 */
static DS_String robot_address (void)
{
    return DS_GetStaticIP (10, CFG_GetTeamNumber(), 2);
}

/**
 * Generates an empty (ignored) FMS packet.
 */
static DS_String create_fms_packet (void)
{
    return  DS_StrNewLen (0);
}

/**
 * Generates an empty (ignored) radio packet.
 */
static DS_String create_radio_packet (void)
{
    return  DS_StrNewLen (0);
}

/**
 * Generates a DS-to-robot packet. The packet is 1024 bytes long and contains
 * the following data:
 *     - The packet index / ID
 *     - The team number
 *     - The control code (which includes e-stop and other commands)
 *     - The alliance and position
 *     - Joystick values
 *     - (Number?) of digital inputs
 *     - The version of the FRC Driver Station
 *     - The CRC32 checksum of the packet
 */
static DS_String create_robot_packet (void)
{
    /* Create initial packet */
    DS_String data = DS_StrNewLen (8);

    /* Add packet index */
    DS_StrSetChar (&data, 0, (sent_robot_packets & 0xff00) >> 8);
    DS_StrSetChar (&data, 1, (sent_robot_packets & 0xff));

    /* Add control code and digital inputs */
    DS_StrSetChar (&data, 2, get_control_code());
    DS_StrSetChar (&data, 3,  get_digital_inputs());

    /* Add team number */
    DS_StrSetChar (&data, 4, (CFG_GetTeamNumber() & 0xff00) >> 8);
    DS_StrSetChar (&data, 5, (CFG_GetTeamNumber() & 0xff));

    /* Add alliance and position */
    DS_StrSetChar (&data, 6, get_alliance_code());
    DS_StrSetChar (&data, 7, get_position_code());

    /* Add joystick data */
    DS_String jsData = get_joystick_data();
    DS_StrJoin (&data, &jsData);

    /* Now resize the datagram to 1024 bytes */
    DS_StrResize (&data, 1024);

    /* Add FRC Driver Station version (same as FRC DS 17.01) */
    DS_StrSetChar (&data, 72, (uint8_t) 0x31);
    DS_StrSetChar (&data, 73, (uint8_t) 0x34);
    DS_StrSetChar (&data, 74, (uint8_t) 0x30);
    DS_StrSetChar (&data, 75, (uint8_t) 0x32);
    DS_StrSetChar (&data, 76, (uint8_t) 0x31);
    DS_StrSetChar (&data, 77, (uint8_t) 0x37);
    DS_StrSetChar (&data, 78, (uint8_t) 0x30);
    DS_StrSetChar (&data, 79, (uint8_t) 0x30);

    /* Add CRC32 checksum */
    uint32_t checksum = DS_CRC32 (data.buf, DS_StrLen (&data));
    DS_StrSetChar (&data, 1020, (checksum & 0xff000000) >> 24);
    DS_StrSetChar (&data, 1021, (checksum & 0xff0000) >> 16);
    DS_StrSetChar (&data, 1022, (checksum & 0xff00) >> 8);
    DS_StrSetChar (&data, 1023, (checksum & 0xff));

    /* Increase sent robot packets */
    ++sent_robot_packets;

    /* Return address of data */
    return data;
}

/**
 * Gets the team station and the robot control mode from the FMS
 */
static int read_fms_packet (const DS_String* data)
{
    /* Data pointer is invalid */
    if (!data)
        return 0;

    /* Packet is too small */
    if (DS_StrLen (data) < 5)
        return 0;

    /* Read FMS packet */
    uint8_t robotmod = (uint8_t) DS_StrCharAt (data, 2);
    uint8_t alliance = (uint8_t) DS_StrCharAt (data, 3);
    uint8_t position = (uint8_t) DS_StrCharAt (data, 4);

    /* Switch to autonomous */
    if (robotmod & cFMSAutonomous)
        CFG_SetControlMode (DS_CONTROL_AUTONOMOUS);

    /* Switch to teleoperated */
    if (robotmod & cFMSTeleoperated)
        CFG_SetControlMode (DS_CONTROL_TELEOPERATED);

    /* Enable (or disable) the robot */
    CFG_SetRobotEnabled (robotmod & cEnabled);

    /* Set team station */
    CFG_SetAlliance (get_alliance (alliance));
    CFG_SetPosition (get_position (position));

    return 1;
}

/**
 * Since the DS does not interact directly with the radio/bridge, any incoming
 * packets shall be ignored.
 */
static int read_radio_packet (const DS_String* data)
{
    (void) data;
    return 0;
}

/**
 * Interprets the given robot packet \a data and updates the emergency stop
 * state and the robot voltage values.
 */
int read_robot_packet (const DS_String* data)
{
    /* Data pointer is invalid */
    if (!data)
        return 0;

    /* Packet is too small */
    if (DS_StrLen (data) < 1024)
        return 0;

    /* Calculate voltage using the rule of three */
    uint8_t upper = ((uint8_t) DS_StrCharAt (data, 1) * 12) / 0x12;
    uint8_t lower = ((uint8_t) DS_StrCharAt (data, 2) * 12) / 0x12;

    /* Construct the voltage float */
    float voltage = ((float) upper) + ((float) lower / 0xff);
    CFG_SetRobotVoltage (voltage);

    /* Check if robot is e-stopped */
    CFG_SetEmergencyStopped ((uint8_t) DS_StrCharAt (data, 0) == cEmergencyStopOn);

    /* Assume that robot code is present (issue #31 in QDriverStation) */
    CFG_SetRobotCode (1);

    /* Packet read successfully */
    return 1;
}

/**
 * Called when the FMS watchdog expires, does nothing...
 */
static void reset_fms (void)
{
    /* Nothing to do */
}

/**
 * Called when the radio watchdog expires, does nothing...
 */
static void reset_radio (void)
{
    /* Nothing to do */
}

/**
 * Called when the robot watchdog expires. This function resets the control
 * flags sent to the robot.
 */
static void reset_robot (void)
{
    resync = 1;
    reboot = 0;
    restart_code = 0;
}

/**
 * Updates the flags used to create the control mode byte to instruct the
 * cRIO to reboot itself
 */
static void reboot_robot (void)
{
    reboot = 1;
}

/**
 * Updates the flags used to create the control mode byte to instruct the
 * cRIO to restart the robot code process
 */
void restart_robot_code (void)
{
    restart_code = 1;
}

/**
 * Initializes and configures the FRC 2014 communication protocol
 */
DS_Protocol DS_GetProtocolFRC_2014 (void)
{
    /* Initialize pointers */
    DS_Protocol protocol;

    /* Set protocol name */
    protocol.name = DS_StrNew ("FRC 2014");

    /* Set address functions */
    protocol.fms_address = &fms_address;
    protocol.radio_address = &radio_address;
    protocol.robot_address = &robot_address;

    /* Set packet generator functions */
    protocol.create_fms_packet = &create_fms_packet;
    protocol.create_radio_packet = &create_radio_packet;
    protocol.create_robot_packet = &create_robot_packet;

    /* Set packet interpretation functions */
    protocol.read_fms_packet = &read_fms_packet;
    protocol.read_radio_packet = &read_radio_packet;
    protocol.read_robot_packet = &read_robot_packet;

    /* Set reset functions */
    protocol.reset_fms = &reset_fms;
    protocol.reset_radio = &reset_radio;
    protocol.reset_robot = &reset_robot;

    /* Set misc. functions */
    protocol.max_battery_voltage = 13;
    protocol.reboot_robot = &reboot_robot;
    protocol.restart_robot_code = &restart_robot_code;

    /* Set packet intervals */
    protocol.fms_interval = 500;
    protocol.radio_interval = 0;
    protocol.robot_interval = 20;

    /* Set joystick properties */
    protocol.max_hat_count = max_hats;
    protocol.max_axis_count = max_axes;
    protocol.max_joysticks = max_joysticks;
    protocol.max_button_count = max_buttons;

    /* Define FMS socket properties */
    protocol.fms_socket = *DS_SocketEmpty();
    protocol.fms_socket.disabled = 0;
    protocol.fms_socket.in_port = 1120;
    protocol.fms_socket.out_port = 1160;
    protocol.fms_socket.type = DS_SOCKET_UDP;

    /* Define radio socket properties */
    protocol.radio_socket = *DS_SocketEmpty();
    protocol.radio_socket.disabled = 1;

    /* Define robot socket properties */
    protocol.robot_socket = *DS_SocketEmpty();
    protocol.robot_socket.disabled = 0;
    protocol.robot_socket.in_port = 1150;
    protocol.robot_socket.out_port = 1110;
    protocol.robot_socket.type = DS_SOCKET_UDP;

    /* Define netconsole socket properties */
    protocol.netconsole_socket = *DS_SocketEmpty();
    protocol.netconsole_socket.disabled = 1;

    /* Return the pointer */
    return protocol;
}

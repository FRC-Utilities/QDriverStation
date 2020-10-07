/*
 * The Driver Station Library (LibDS)
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted; free of charge; to any person obtaining
 * a copy of this software and associated documentation files (the "Software");
 * to deal in the Software without restriction; including without limitation
 * the rights to use; copy; modify; merge; publish; distribute; sublicense;
 * and/or sell copies of the Software; and to permit persons to whom the
 * Software is furnished to do so; subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS"; WITHOUT WARRANTY OF ANY KIND; EXPRESS
 * OR IMPLIED; INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY;
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM; DAMAGES OR OTHER
 * LIABILITY; WHETHER IN AN ACTION OF CONTRACT; TORT OR OTHERWISE; ARISING
 * FROM; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "DS_Utils.h"
#include "DS_Config.h"
#include "DS_Protocol.h"
#include "DS_Joysticks.h"
#include "DS_DefaultProtocols.h"

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#if defined _WIN32
    #include <windows.h>
#endif

/*
 * Protocol bytes
 */
static const uint8_t cTest               = 0x01;
static const uint8_t cEnabled            = 0x04;
static const uint8_t cAutonomous         = 0x02;
static const uint8_t cTeleoperated       = 0x00;
static const uint8_t cFMS_Attached       = 0x08;
static const uint8_t cEmergencyStop      = 0x80;
static const uint8_t cRequestReboot      = 0x08;
static const uint8_t cRequestNormal      = 0x80;
static const uint8_t cRequestUnconnected = 0x00;
static const uint8_t cRequestRestartCode = 0x04;
static const uint8_t cFMS_RadioPing      = 0x10;
static const uint8_t cFMS_RobotPing      = 0x08;
static const uint8_t cFMS_RobotComms     = 0x20;
static const uint8_t cFMS_DS_Version     = 0x00;
static const uint8_t cTagDate            = 0x0f;
static const uint8_t cTagGeneral         = 0x01;
static const uint8_t cTagJoystick        = 0x0c;
static const uint8_t cTagTimezone        = 0x10;
static const uint8_t cRed1               = 0x00;
static const uint8_t cRed2               = 0x01;
static const uint8_t cRed3               = 0x02;
static const uint8_t cBlue1              = 0x03;
static const uint8_t cBlue2              = 0x04;
static const uint8_t cBlue3              = 0x05;
static const uint8_t cRTagCANInfo        = 0x0e;
static const uint8_t cRTagCPUInfo        = 0x05;
static const uint8_t cRTagRAMInfo        = 0x06;
static const uint8_t cRTagDiskInfo       = 0x04;
static const uint8_t cRequestTime        = 0x01;
static const uint8_t cRobotHasCode       = 0x20;

/*
 * Sent robot and FMS packet counters
 */
static unsigned int send_time_data = 0;
static unsigned int sent_fms_packets = 0;
static unsigned int sent_robot_packets = 0;

/*
 * Control code flags
 */
static int reboot = 0;
static int restart_code = 0;

/**
 * Obtains the voltage float from the given \a upper and \a lower bytes
 */
static float decode_voltage (uint8_t upper, uint8_t lower)
{
    return ((float) upper) + ((float) lower / 0xff);
}

/**
 * Encodes the \a voltage float in the given \a upper and \a lower bytes
 */
static void encode_voltage (float voltage, uint8_t* upper, uint8_t* lower)
{
    if (upper && lower) {
        *upper = (uint8_t) (voltage);
        *lower = (uint8_t) (voltage - (int) voltage) * 100;
    }
}

/**
 * Returns the control code sent to the FMS. This code is very similar to
 * the control code sent to the robot, however, it contains addional/extra
 * information regarding the robot radio.
 *
 * This code contains the following information:
 *    - The control mode of the robot (teleop, autonomous, test)
 *    - The enabled state of the robot
 *    - The FMS attached keyword
 *    - Robot radio connected?
 *    - The operation state (e-stop, normal)
 */
static uint8_t fms_control_code (void)
{
    uint8_t code = 0;

    /* Let the FMS know the operational status of the robot */
    switch (CFG_GetControlMode()) {
    case DS_CONTROL_TEST:
        code |= cTest;
        break;
    case DS_CONTROL_AUTONOMOUS:
        code |= cAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        code |= cTeleoperated;
        break;
    }

    /* Let the FMS know if robot is e-stopped */
    if (CFG_GetEmergencyStopped())
        code |= cEmergencyStop;

    /* Let the FMS know if the robot is enabled */
    if (CFG_GetRobotEnabled())
        code |= cEnabled;

    /* Let the FMS know if we are connected to radio */
    if (CFG_GetRadioCommunications())
        code |= cFMS_RadioPing;

    /* Let the FMS know if we are connected to robot */
    if (CFG_GetRobotCommunications()) {
        code |= cFMS_RobotComms;
        code |= cFMS_RobotPing;
    }

    return code;
}

/**
 * Returns the control code sent to the robot, it contains:
 *    - The control mode of the robot (teleop, autonomous, test)
 *    - The enabled state of the robot
 *    - The FMS attached keyword
 *    - The operation state (e-stop, normal)
 */
static uint8_t get_control_code (void)
{
    uint8_t code = 0;

    /* Get current control mode (Test, Auto or Teleop) */
    switch (CFG_GetControlMode()) {
    case DS_CONTROL_TEST:
        code |= cTest;
        break;
    case DS_CONTROL_AUTONOMOUS:
        code |= cAutonomous;
        break;
    case DS_CONTROL_TELEOPERATED:
        code |= cTeleoperated;
        break;
    default:
        break;
    }

    /* Let the robot know if we are connected to the FMS */
    if (CFG_GetFMSCommunications())
        code |= cFMS_Attached;

    /* Let the robot know if it should e-stop right now */
    if (CFG_GetEmergencyStopped())
        code |= cEmergencyStop;

    /* Append the robot enabled state */
    if (CFG_GetRobotEnabled())
        code |= cEnabled;

    return code;
}

/**
 * Generates the request code sent to the robot, which may instruct it to:
 *    - Operate normally
 *    - Reboot the roboRIO
 *    - Restart the robot code process
 */
static uint8_t get_request_code (void)
{
    uint8_t code = cRequestNormal;

    /* Robot has comms, check if we need to send additional flags */
    if (CFG_GetRobotCommunications()) {
        if (reboot)
            code = cRequestReboot;
        else if (restart_code)
            code = cRequestRestartCode;
    }

    /* Send disconnected state flag (may trigger resync) */
    else
        code = cRequestUnconnected;

    return code;
}

/**
 * Returns the team station code sent to the robot.
 * This value may be used by the robot program to use specialized autonomous
 * modes or adjust sensor input.
 */
static uint8_t get_station_code (void)
{
    /* Current config is set to position 1 */
    if (CFG_GetPosition() == DS_POSITION_1) {
        if (CFG_GetAlliance() == DS_ALLIANCE_RED)
            return cRed1;
        else
            return cBlue1;
    }

    /* Current config is set to position 2 */
    if (CFG_GetPosition() == DS_POSITION_2) {
        if (CFG_GetAlliance() == DS_ALLIANCE_RED)
            return cRed2;
        else
            return cBlue2;
    }

    /* Current config is set to position 3 */
    if (CFG_GetPosition() == DS_POSITION_3) {
        if (CFG_GetAlliance() == DS_ALLIANCE_RED)
            return cRed3;
        else
            return cBlue3;
    }

    return cRed1;
}

/**
 * Returns the size of the given \a joystick. This function is used to generate
 * joystick data (which is sent to the robot) and to resize the client->robot
 * datagram automatically.
 */
static uint8_t get_joystick_size (const int joystick)
{
    int header_size = 2;
    int button_data = 3;
    int axis_data = DS_GetJoystickNumAxes (joystick) + 1;
    int hat_data = (DS_GetJoystickNumHats (joystick) * 2) + 1;

    return header_size + button_data + axis_data + hat_data;
}

/**
 * Returns information regarding the current date and time and the timezone
 * of the client computer.
 *
 * The robot may ask for this information in some cases (e.g. when initializing
 * the robot code).
 */
static DS_String get_timezone_data (void)
{
    DS_String data = DS_StrNewLen (14);

    /* Get current time */
    time_t rt = 0;
    uint32_t ms = 0;
    struct tm timeinfo;

#if defined _WIN32
    localtime_s (&timeinfo, &rt);
#else
    localtime_r (&rt, &timeinfo);
#endif

#if defined _WIN32
    /* Get timezone information */
    TIME_ZONE_INFORMATION info;
    GetTimeZoneInformation (&info);

    /* Convert the wchar to a standard string */
    size_t len = wcslen (info.StandardName) + 1;
    char* str = calloc (len, sizeof (char));
    wcstombs_s (NULL, str, len, info.StandardName, wcslen (info.StandardName));

    /* Convert the obtained cstring to a bstring */
    DS_String tz = DS_StrNew (str);
    free (str);

    /* Get milliseconds */
    GetSystemTime (&info.StandardDate);
    ms = (uint32_t) info.StandardDate.wMilliseconds;
#else
    /* Timezone is stored directly in time_t structure */
    DS_String tz = DS_StrNew (timeinfo.tm_zone);
#endif

    /* Encode date/time in datagram */
    DS_StrSetChar (&data, 0,  (uint8_t) 0x0b);
    DS_StrSetChar (&data, 1,  (uint8_t) cTagDate);
    DS_StrSetChar (&data, 2,  (uint8_t) (ms >> 24));
    DS_StrSetChar (&data, 3,  (uint8_t) (ms >> 16));
    DS_StrSetChar (&data, 4,  (uint8_t) (ms >> 8));
    DS_StrSetChar (&data, 5,  (uint8_t) (ms));
    DS_StrSetChar (&data, 6,  (uint8_t) timeinfo.tm_sec);
    DS_StrSetChar (&data, 7,  (uint8_t) timeinfo.tm_min);
    DS_StrSetChar (&data, 8,  (uint8_t) timeinfo.tm_hour);
    DS_StrSetChar (&data, 9,  (uint8_t) timeinfo.tm_yday);
    DS_StrSetChar (&data, 10, (uint8_t) timeinfo.tm_mon);
    DS_StrSetChar (&data, 11, (uint8_t) timeinfo.tm_year);

    /* Add timezone length and tag */
    DS_StrSetChar (&data, 12, DS_StrLen (&tz));
    DS_StrSetChar (&data, 13, cTagTimezone);

    /* Add timezone string */
    DS_StrJoin (&data, &tz);

    /* Return the obtained data */
    return data;
}

/**
 * Constructs a joystick information structure for every attached joystick.
 * Unlike the 2014 protocol, the 2015 protocol only generates joystick data
 * for the attached joysticks.
 */
static DS_String get_joystick_data (void)
{
    /* Initialize the variables */
    int i = 0;
    int j = 0;
    DS_String data = DS_StrNewLen (0);

    /* Generate data for each joystick */
    for (i = 0; i < DS_GetJoystickCount(); ++i) {
        DS_StrAppend (&data, get_joystick_size (i));
        DS_StrAppend (&data, cTagJoystick);

        /* Add axis data */
        DS_StrAppend (&data, DS_GetJoystickNumAxes (i));
        for (j = 0; j < DS_GetJoystickNumAxes (i); ++j)
            DS_StrAppend (&data, DS_FloatToByte (DS_GetJoystickAxis (i, j), 1));

        /* Generate button data */
        uint16_t button_flags = 0;
        for (j = 0; j < DS_GetJoystickNumButtons (i); ++j)
            button_flags += DS_GetJoystickButton (i, j) ? (int) pow (2, j) : 0;

        /* Add button data */
        DS_StrAppend (&data, DS_GetJoystickNumButtons (i));
        DS_StrAppend (&data, (uint8_t) (button_flags >> 8));
        DS_StrAppend (&data, (uint8_t) (button_flags));

        /* Add hat data */
        DS_StrAppend (&data, DS_GetJoystickNumHats (i));
        for (j = 0; j < DS_GetJoystickNumHats (i); ++j) {
            DS_StrAppend (&data, (uint8_t) (DS_GetJoystickHat (i, j) >> 8));
            DS_StrAppend (&data, (uint8_t) (DS_GetJoystickHat (i, j)));
        }
    }

    /* Return obtained data */
    return data;
}

/**
 * Obtains the CPU, RAM, Disk and CAN information from the robot packet
 */
static void read_extended (const DS_String* data, const int offset)
{
    /* Check if data pointer is valid */
    if (!data)
        return;

    /* Get header tag */
    uint8_t tag = (uint8_t) DS_StrCharAt (data, offset + 1);

    /* Get CAN information */
    if (tag == cRTagCANInfo)
        CFG_SetCANUtilization ((uint8_t) DS_StrCharAt (data, 10));

    /* Get CPU usage */
    else if (tag == cRTagCPUInfo)
        CFG_SetRobotCPUUsage ((uint8_t) DS_StrCharAt (data, 3));

    /* Get RAM usage */
    else if (tag == cRTagRAMInfo)
        CFG_SetRobotRAMUsage ((uint8_t) DS_StrCharAt (data, 4));

    /* Get disk usage */
    else if (tag == cRTagDiskInfo)
        CFG_SetRobotDiskUsage ((uint8_t)  DS_StrCharAt (data, 4));
}

/**
 * Gets the alliance type from the received \a byte
 * This function is used to update the robot configuration when receiving data
 * from the FMS
 */
static DS_Alliance get_alliance (const uint8_t byte)
{
    if (byte == cBlue1 || byte == cBlue2 || byte == cBlue3)
        return DS_ALLIANCE_BLUE;

    return DS_ALLIANCE_RED;
}

/**
 * Gets the position type from the received \a byte
 * This function is used to update the robot configuration when receiving data
 * from the FMS
 */
static DS_Position get_position (const uint8_t byte)
{
    if (byte == cRed1 || byte == cBlue1)
        return DS_POSITION_1;

    else if (byte == cRed2 || byte == cBlue2)
        return DS_POSITION_2;

    else if (byte == cRed3 || byte == cBlue3)
        return DS_POSITION_3;

    return DS_POSITION_1;
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
 * The 2015 control system assigns the radio IP in 10.te.am.1
 */
static DS_String radio_address (void)
{
    return DS_GetStaticIP (10, CFG_GetTeamNumber(), 1);
}

/**
 * The 2015 control system assigns the robot address at roboRIO-TEAM.local
 */
static DS_String robot_address (void)
{
    return DS_StrFormat ("roboRIO-%d.local", CFG_GetTeamNumber());
}

/**
 * Generates a packet that the DS will send to the FMS, it contains:
 *    - The FMS packet index
 *    - The robot voltage
 *    - Robot control code
 *    - DS version
 *    - Radio and robot ping flags
 *    - The team number
 */
static DS_String create_fms_packet (void)
{
    /* Create an 8-byte long packet */
    DS_String data = DS_StrNewLen (8);

    /* Get voltage bytes */
    uint8_t integer = 0;
    uint8_t decimal = 0;
    encode_voltage (CFG_GetRobotVoltage(), &integer, &decimal);

    /* Add FMS packet count */
    DS_StrSetChar (&data, 0, (sent_fms_packets >> 8));
    DS_StrSetChar (&data, 1, (sent_fms_packets));

    /* Add DS version and FMS control code */
    DS_StrSetChar (&data, 2, cFMS_DS_Version);
    DS_StrSetChar (&data, 3, fms_control_code());

    /* Add team number */
    DS_StrSetChar (&data, 4, (CFG_GetTeamNumber() >> 8));
    DS_StrSetChar (&data, 5, (CFG_GetTeamNumber()));

    /* Add robot voltage */
    DS_StrSetChar (&data, 6, integer);
    DS_StrSetChar (&data, 7, decimal);

    /* Increase FMS packet counter */
    ++sent_fms_packets;

    return data;
}

/**
 * The 2015 communication protocol does not involve sending specialized packets
 * to the DS Radio / Bridge. For that reason, the 2015 communication protocol
 * generates empty radio packets.
 */
static DS_String create_radio_packet (void)
{
    return  DS_StrNewLen (0);
}

/**
 * Generates a packet that the DS will send to the robot, it contains the
 * following information:
 *    - Packet index / ID
 *    - Control code (control modes, e-stop state, etc)
 *    - Request code (robot reboot, restart code, normal operation, etc)
 *    - Team station (alliance & position)
 *    - Date and time data (if robot requests it)
 *    - Joystick information (if the robot does not want date/time)
 */
static DS_String create_robot_packet (void)
{
    DS_String data = DS_StrNewLen (6);

    /* Add packet index */
    DS_StrSetChar (&data, 0, (sent_robot_packets >> 8));
    DS_StrSetChar (&data, 1, (sent_robot_packets));

    /* Add packet header */
    DS_StrSetChar (&data, 2, cTagGeneral);

    /* Add control code, request flags and team station */
    DS_StrSetChar (&data, 3, get_control_code());
    DS_StrSetChar (&data, 4, get_request_code());
    DS_StrSetChar (&data, 5, get_station_code());

    /* Add timezone data (if robot wants it) */
    if (send_time_data) {
        DS_String tz = get_timezone_data();
        DS_StrJoin (&data, &tz);
    }

    /* Add joystick data */
    else if (sent_robot_packets > 5) {
        DS_String js = get_joystick_data();
        DS_StrJoin (&data, &js);
    }

    /* Increase robot packet counter */
    ++sent_robot_packets;

    return data;
}

/**
 * Interprets the packet and follows the instructions sent by the FMS.
 * Possible instructions are:
 *   - Change robot control mode
 *   - Change robot enabled status
 *   - Change team alliance
 *   - Change team position
 */
static int read_fms_packet (const DS_String* data)
{
    /* Data pointer is invalid */
    if (!data)
        return 0;

    /* Packet is too small */
    if (DS_StrLen (data) < 22)
        return 0;

    /* Read FMS packet */
    uint8_t control = (uint8_t) DS_StrCharAt (data, 3);
    uint8_t station = (uint8_t) DS_StrCharAt (data, 5);

    /* Change robot enabled state based on what FMS tells us to do*/
    CFG_SetRobotEnabled (control & cEnabled);

    /* Get FMS robot mode */
    if (control & cTeleoperated)
        CFG_SetControlMode (DS_CONTROL_TELEOPERATED);
    else if (control & cAutonomous)
        CFG_SetControlMode (DS_CONTROL_AUTONOMOUS);
    else if (control & cTest)
        CFG_SetControlMode (DS_CONTROL_TEST);

    /* Update to correct alliance and position */
    CFG_SetAlliance (get_alliance (station));
    CFG_SetPosition (get_position (station));

    /* After this, we have more information about the current match,
     * but we do not really use it, so... */

    /* Packet read successfully */
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
 * Interprets the packet and obtains the following information:
 *    - The user code state of the robot
 *    - If the robot needs to get the current date/time from the client
 *    - The emergency stop state of the robot
 *    - The robot voltage
 *    - Extended information (CPU usage, RAM usage, Disk Usage and CAN status)
 */
static int read_robot_packet (const DS_String* data)
{
    /* Data pointer is invalid */
    if (!data)
        return 0;

    /* Packet is too small */
    if (DS_StrLen (data) < 7)
        return 0;

    /* Read robot packet */
    uint8_t control = (uint8_t) DS_StrCharAt (data, 3);
    uint8_t rstatus = (uint8_t) DS_StrCharAt (data, 4);
    uint8_t request = (uint8_t) DS_StrCharAt (data, 7);

    /* Update client information */
    CFG_SetRobotCode (rstatus & cRobotHasCode);
    CFG_SetEmergencyStopped (control & cEmergencyStop);

    /* Update date/time request flag */
    send_time_data = (request == cRequestTime);

    /* Calculate the voltage */
    uint8_t upper = (uint8_t) DS_StrCharAt (data, 5);
    uint8_t lower = (uint8_t) DS_StrCharAt (data, 6);
    CFG_SetRobotVoltage (decode_voltage (upper, lower));

    /* This is an extended packet, read its extra data */
    if (DS_StrLen (data) > 9)
        read_extended (data, 8);

    /* Packet read, feed the watchdog some meat */
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
 * Called when the robot watchdog expires, resets the control code flags
 */
static void reset_robot (void)
{
    reboot = 0;
    restart_code = 0;
    send_time_data = 0;
}

/**
 * Updates the control code flags to instruct the roboRIO to reboot itself
 */
static void reboot_robot (void)
{
    reboot = 1;
}

/**
 * Updates the control code flags to instruct the robot to restart the
 * robot code process
 */
static void restart_robot_code (void)
{
    restart_code = 1;
}

/**
 * Initializes the 2015 FRC Communication Protocol
 */
DS_Protocol DS_GetProtocolFRC_2015 (void)
{
    /* Initialize structure */
    DS_Protocol protocol;

    /* Set protocol name */
    protocol.name = DS_StrNew ("FRC 2015");

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
    protocol.max_joysticks = 6;
    protocol.max_hat_count = 1;
    protocol.max_axis_count = 6;
    protocol.max_button_count = 10;

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
    protocol.netconsole_socket.disabled = 0;
    protocol.netconsole_socket.broadcast = 1;
    protocol.netconsole_socket.in_port = 6666;
    protocol.netconsole_socket.out_port = 6668;
    protocol.netconsole_socket.type = DS_SOCKET_UDP;

    /* Return the protocol */
    return protocol;
}

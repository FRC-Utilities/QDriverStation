/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _DRIVER_STATION_COMMON_H
#define _DRIVER_STATION_COMMON_H

#include <QString>
#include <QObject>

/**
 * Represents the available operation modes of the robot.
 * If you have ever participated in a FRC team, you will understand the
 * importance of defining how the robot will behave.
 */
enum DS_ControlMode {
    DS_ControlTest          = 0x05, /**< Driver moves each component manually */
    DS_ControlTeleOp        = 0x04, /**< The robot moves based on user input */
    DS_ControlDisabled      = 0x00, /**< The robot is idle and does nothing */
    DS_ControlAutonomous    = 0x06, /**< The robot does pre-programmed tasks */
    DS_ControlEmergencyStop = 0x80  /**< The robot stops and needs to reboot*/
};

/**
 * Represents the current operation mode or status of the roboRIO.
 * This is used to know what the roboRIO is currently doing when it sends us
 * a robot packet
 */
enum DS_ProgramMode {
    DS_ProgramNoCode     = 0x00, /**< It seems that there is no user code */
    DS_ProgramDisabled   = 0x01, /**< The robot is disabled */
    DS_ProgramTeleOp     = 0x02, /**< The robot is in tele-operated mode */
    DS_ProgramAutonomous = 0x04, /**< The robot is in autonomous mode */
    DS_ProgramTest       = 0x08  /**< The robot is in test mode */
};

/**
 * Represents the possible control bytes that we can send to the robot
 * to reboot the RoboRIO or to restart the robot code
 */
enum DS_Status {
    DS_StatusNormal = 0x10,      /**< Do not reboot or do anything */
    DS_StatusRestartCode = 0x14, /**< Restart the robot code */
    DS_StatusRebootRobot = 0x18  /**< Reboot the RoboRIO */
};

/**
 * Represents the available alliances that the robot can have.
 * Its important to specify which alliance we use in order to tell
 * the robot program 'where it is' and communicate with the FMS correctly
 */
enum DS_Alliance {
    DS_AllianceRed1  = 0x00, /**< Red 1, use control bit 0x00 */
    DS_AllianceRed2  = 0x01, /**< Red 2, use control bit 0x01 */
    DS_AllianceRed3  = 0x02, /**< Red 3, use control bit 0x02 */
    DS_AllianceBlue1 = 0x03, /**< Blue 1, use control bit 0x03 */
    DS_AllianceBlue2 = 0x04, /**< Blue 2, use control bit 0x04 */
    DS_AllianceBlue3 = 0x05  /**< Blue 3, use control bit 0x05 */
};

/**
 * Represents the ping data of a packet
 */
struct DS_PingData {
    int byte1; /**< The first byte of the packet */
    int byte2; /**< The second byte of the packet */

    /**
     * Generates the correct values for \c byte1 and \c byte2 based on the
     * value of the given \a index
     */
    void generatePingData (int index);

    /**
     * Returns the index of the packet as a normal \c int
     */
    int getPingIndex();
};

/**
 * Represents a packet sent by the roboRIO to the client
 */
struct DS_RobotPacket {
    bool hasCode;
    double voltage;
    int commVersion;
    bool requestDataTime;
    QString voltageString;

    DS_PingData pongData;
    DS_ControlMode controlMode;
    DS_ProgramMode programMode;
};

/**
 * Represents a control packet, which tells the robot:
 *     - The control mode to use
 *     - The alliance and position to use
 *     - The packet count (generated automatically)
 *     - The status (e.g: reboot or just be stay still)
 */
struct DS_ClientPacket {
    DS_Status status;     /**< The status that we want the robot to have */
    DS_ControlMode mode;  /**< The mode that we want the robot to have */
    DS_Alliance alliance; /**< The alliance that we want the robot to have */
};

/**
 * Represents a joystick and its current state.
 * The idea behind this is that the program will record the current state
 * of a joystick and fill this structure with its data, which then
 * will be processed by the library and sent to the robot
 */
struct DS_JoystickData {
    /* We must figure this one out */
};

/**
 * Returns a calculated IP address based on the team address.
 *
 * For example:
 *     - \c DS_GetStaticIp(3794, 1) would return \c 10.37.94.1
 *     - \c DS_GetStaticIp(3794, 2) would return \c 10.37.94.2
 *     - \c DS_GetStaticIp( 118, 3) would return \c 10.01.18.3
 *     - And so on...
 */
QString DS_GetStaticIp (int team, int host);

/**
 * Returns an user-friendly string given the inputed robot control mode
 */
QString DS_GetControlModeString (DS_ControlMode mode);

#endif /* _DRIVER_STATION_COMMON_H */

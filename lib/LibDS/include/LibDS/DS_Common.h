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

#ifndef _LIB_DS_COMMON_H
#define _LIB_DS_COMMON_H

#include <QString>
#include <QObject>

#include "DS_Global.h"

/**
 * Represents the available operation modes of the robot.
 * If you have ever participated in a FRC team, you will understand the
 * importance of defining how the robot will behave.
 */
enum DS_ControlMode {
    DS_ControlTest,
    DS_ControlTeleOp,
    DS_ControlDisabled,
    DS_ControlAutonomous,
    DS_ControlEmergencyStop
};

/**
 * Represents the available alliances that the robot can have.
 * Its important to specify which alliance we use in order to tell
 * the robot program 'where it is' and communicate with the FMS correctly
 */
enum DS_Alliance {
    DS_AllianceRed1,
    DS_AllianceRed2,
    DS_AllianceRed3,
    DS_AllianceBlue1,
    DS_AllianceBlue2,
    DS_AllianceBlue3
};

/**
 * @brief The DS_Joystick struct
 */
struct LIB_DS_DECL DS_Joystick {
    int numAxes;
    int numButtons;
    int numPovHats;

    int* povHats;
    double* axes;
    bool* buttons;
};

/**
 * Represents the ping data of a packet
 */
struct LIB_DS_DECL DS_PingData {
    short byte1; /**< The first byte of the packet */
    short byte2; /**< The second byte of the packet */

    /**
     * Generates the correct values for \c byte1 and \c byte2 based on the
     * value of the given \a index
     */
    void generatePingData (int index);

    /**
     * Returns the index of the packet as a normal \c int
     */
    short getPingIndex();
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
QString LIB_DS_DECL DS_GetStaticIp (int team, int host);

/**
 * Returns an user-friendly string given the inputed robot control mode
 */
QString LIB_DS_DECL DS_GetControlModeString (DS_ControlMode mode);

#endif

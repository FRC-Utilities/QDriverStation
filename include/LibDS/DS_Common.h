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

#pragma once
#ifndef _LIB_DS_COMMON_H
#define _LIB_DS_COMMON_H

#include "DS_Global.h"

#include <QString>
#include <QDateTime>
#include <QUdpSocket>
#include <QStringList>

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
    DS_ControlEmergencyStop,
    DS_ControlNoCommunication
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
 * Generates a byte array from the \a input data
 */
template <class t>
QByteArray  LIB_DS_DECL DS_ToBytes (t input)
{
    QByteArray data;

    for (int i = sizeof (input); i != 0; --i)
        data.append ((char) (input >> ((i - 1) * 8)) & 0xff);

    return data;
}

/**
 * Returns the current timezone code by calculating the difference between
 * the system timezone and the UTC timezone
 */
QString LIB_DS_DECL DS_GetTimezoneCode();

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

/**
 * Reads the contents of the \a socket and returns its data
 */
QByteArray LIB_DS_DECL DS_GetSocketData (QUdpSocket* socket);

#endif

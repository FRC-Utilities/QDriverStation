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

#include <QtMath>
#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QUdpSocket>
#include <QStringList>

#include "LibDS/Core/Library.h"

/**
 * Represents the available operation modes of the robot.
 * If you have ever participated in a FRC team, you will understand the
 * importance of defining how the robot will behave.
 */
enum DS_ControlMode {
    kControlTest,           /**< Individual actuators can be moved */
    kControlDisabled,       /**< Robot is idle */
    kControlAutonomous,     /**< Robot takes over the world */
    kControlTeleoperated,   /**< User moves the robot */
    kControlEmergencyStop,  /**< Forced robot stop */
};

/**
 * Represents the available alliances that the robot can have.
 * Its important to specify which alliance we use in order to tell
 * the robot program 'where it is' and communicate with the FMS correctly
 */
enum DS_Alliance {
    kAllianceRed1  = 0x00,  /** Red alliance, position 1 */
    kAllianceRed2  = 0x01,  /** Red alliance, position 2 */
    kAllianceRed3  = 0x02,  /** Red alliance, position 3 */
    kAllianceBlue1 = 0x03,  /** Blue alliance, position 1 */
    kAllianceBlue2 = 0x04,  /** Blue alliance, position 2 */
    kAllianceBlue3 = 0x05   /** Blue alliance, position 3 */
};

/**
 * Represents the current status of the communications
 */
enum DS_CommunicationStatus {
    kFull    = 0x00,        /** The DS is communicating with the robot */
    kPartial = 0x01,        /** The robot responds ping requests, but does not respond to DS */
    kFailing = 0x02         /** The robot does not respond to ping requests */
};

/**
 * @brief The DS_Joystick struct
 */
struct LIB_DS_DECL DS_Joystick {
    int numAxes;
    int numButtons;
    int numPovHats;

    bool* buttons;
    int* povHats;
    double* axes;
};

/**
 * Converts an \c int to two \c bytes, this
 * is used by the protocols to send numbers that
 * are greater than 255 (the maximum value of a byte)
 */
struct LIB_DS_DECL ShortToBytes {
    quint8 byte1 = 0;
    quint8 byte2 = 0;

    /**
     * Calculates the respective values for \c byte1 and
     * \c byte2 using the input \a data
     */
    void setData (int data);
};

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

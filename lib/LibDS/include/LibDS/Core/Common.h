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

#ifndef _LDS_COMMON_H
#define _LDS_COMMON_H

#include <QtMath>
#include <QString>
#include <QDateTime>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QStringList>

#include "LibDS/Core/Library.h"

/**
 * Use this when the protocol itself is not bound to a specific port
 */
#define DS_PROTOCOL_NO_PORT -1

/**
 * Represents the available operation modes of the robot.
 */
typedef enum
{
    kControlTest         =  1, /**< Individual actuators can be moved */
    kControlAutonomous   =  2, /**< Robot takes over the world */
    kControlTeleoperated =  3, /**< User moves the robot */
    kControlInvalid      = -1, /**< Blame the programmers if this is used */
} DS_ControlMode;

/**
 * Represents the available alliances that the robot can have.
 * Its important to specify which alliance we use in order to tell
 * the robot program 'where it is' and communicate with the FMS correctly
 */
typedef enum
{
    kAllianceRed1  = 0,  /** Red alliance, position 1 */
    kAllianceRed2  = 1,  /** Red alliance, position 2 */
    kAllianceRed3  = 2,  /** Red alliance, position 3 */
    kAllianceBlue1 = 3,  /** Blue alliance, position 1 */
    kAllianceBlue2 = 4,  /** Blue alliance, position 2 */
    kAllianceBlue3 = 5,  /** Blue alliance, position 3 */
} DS_Alliance;

/**
 * Represents the current status of the communications
 */
typedef enum
{
    kFull    = 0, /** The DS is communicating with the robot */
    kPartial = 1, /** The robot responds ping requests, but does not respond to DS */
    kFailing = 2, /** The robot does not respond to ping requests */
} DS_CommStatus;

/**
 * Used for the library's logger function
 */
typedef enum
{
    kLibLevel      = 0, /** Look! A plane */
    kInfoLevel     = 1, /** I am dangerous, I like it */
    kWarnLevel     = 2, /** Watch the birdie */
    kErrorLevel    = 3, /** I can't shoot, so let's have a little fun */
    kCriticalLevel = 4, /** Is this your idea of fun? */

} DS_MessageType;

/**
 * Represents a joystick in the DS
 */
typedef struct
{
    QString name;

    int numAxes;
    int numPOVs;
    int numButtons;

    int* POVs;
    float* axes;
    bool* buttons;
} DS_Joystick;

/**
 * Represents a rumble request event
 */
typedef struct
{
    int joystickID;
    bool leftRumble;
    bool rightRumble;
} DS_RumbleRequest;

/**
 * Represents a set of CAN information
 */
typedef struct
{
    quint8 util;
    quint8 busOff;
    quint8 txFull;
    quint8 receive;
    quint8 transmit;
} DS_CAN;

/**
 * Returns the current timezone code by calculating the difference between
 * the system timezone and the UTC timezone
 */
QString LIB_DS_DECL DS_GetTimezoneCode();

/**
 * Sends a message, which will be interpreted by the NetConsole
 */
void LIB_DS_DECL DS_SendMessage (QString message);

/**
 * Logs a message to the console and a logging file
 */
void LIB_DS_DECL DS_LogMessage (DS_MessageType type, QString message);

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
 * Returns a calculated IP address based on the team address.
 *
 * For example:
 *     - \c DS_GetStaticIp(10, 3794, 1) would return \c 10.37.94.1
 *     - \c DS_GetStaticIp(177, 3794, 2) would return \c 177.37.94.2
 *     - \c DS_GetStaticIp(10m 118, 3) would return \c 10.01.18.3
 *     - And so on...
 */
QString LIB_DS_DECL DS_GetStaticIp (int net, int team, int host);

/**
 * Returns an user-friendly string given the inputed robot control mode
 */
QString LIB_DS_DECL DS_GetControlModeString (DS_ControlMode mode);

/**
 * Reads the contents of the \a socket and returns its data
 */
QByteArray LIB_DS_DECL DS_GetSocketData (QUdpSocket* socket);

/**
 * Reads the contents of the \a socket and returns its data
 */
QByteArray LIB_DS_DECL DS_GetSocketData (QTcpSocket* socket);

/**
 * Parses the input \a data into two bytes
 */
QByteArray LIB_DS_DECL DS_ToBytes (int data);

#endif

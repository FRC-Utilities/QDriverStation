/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

namespace DS {
///
/// Use this value when you have no idea which port to use on a protocol
///
const int INVALID_PORT = 0;

///
/// Represents the available operation modes of the robot.
/// Note that this values are meant for the library and client to
/// use and do not represent in any way the 'raw' bytes sent to the robot.
///
typedef enum {
    kControlTest         = 0, /// Test
    kControlAutonomous   = 1, /// Autonomous
    kControlTeleoperated = 2, /// Operator control
    kControlInvalid      = 3, /// Reserved for internal use
} ControlMode;

///
/// Represents the available alliances and positions during a match
///
typedef enum {
    kAllianceRed1        = 0, /// Red 1
    kAllianceRed2        = 1, /// Red 2
    kAllianceRed3        = 2, /// Red 3
    kAllianceBlue1       = 3, /// Blue 1
    kAllianceBlue2       = 4, /// Blue 2
    kAllianceBlue3       = 5, /// Blue 3
} Alliance;

///
/// Represents the status of the DS <--> Robot communications
///
typedef enum {
    kFull                = 0, /// We can move the robot
    kPartial             = 1, /// We can ping the robot, but not move it
    kFailing             = 2, /// We cannot ping nor move the robot
} DS_CommStatus;

///
/// Used to differentiate between the different error levels of log messages
///
typedef enum {
    kLibLevel            = 0, /// Used exclusively by the LibDS, do not use it on the client
    kInfoLevel           = 1, /// Indicates normal operations (e.g. open a window)
    kWarnLevel           = 2, /// Indicates a condition in which we should take some care
    kErrorLevel          = 3, /// Indicates a non-fatal error
    kCriticalLevel       = 4, /// Indicates a fatal error
} ErrorLevel;

///
/// Used by the library to store information about a joystick or any other
/// input device used to control the robot.
///
/// Please note that the LibDS only deals with the "networking" part of
/// a FRC Driver Station, the rest must be implemented by the client.
///
typedef struct {
    QString name;             /// Display name          [Optional]
    int     numAxes;          /// Number of axes        [Required]
    int     numPOVs;          /// Number of POVs        [Required]
    int     numButtons;       /// Number of buttons     [Required]
    int*    POVs;             /// Stores POV values     [Updated automatically]
    float*  axes;             /// Stores axis values    [Updated automatically]
    bool*   buttons;          /// Stores button values  [Updated automatically]
} Joystick;

///
/// Represents a joystick rumble request issued by the robot code
///
typedef struct {

} RumbleRequest;

///
/// Represents the received CAN status information from a robot packet
///
typedef struct {
    quint8 util;              /// Utilization %
    quint8 busOff;            /// Bus off (error count)
    quint8 txFull;            /// TX Full
    quint8 receive;           /// Recieve %
    quint8 transmit;          /// Transmission %
} CAN;

///
/// Returns the current timezone code by calculating the difference between
/// the system timezone and the UTC timezone
///
QString LIB_DS_DECL Timezone();

///
/// Sends a message through the DriverStation to the Client, which should interpret
/// it as a message from the NetConsole.
///
/// This is used by the library to send warnings and status messages to the
/// user through a simple and integrated manner.
///
void LIB_DS_DECL SendMessage (QString message);

///
/// Logs the message according to its \a type or error level
///
void LIB_DS_DECL Log (ErrorLevel type, QString message);

///
/// Returns a calculated IP address based on the team address.
///
/// For example:
///    - \c DS_GetStaticIp(3794, 1) would return \c 10.37.94.1
///    - \c DS_GetStaticIp(3794, 2) would return \c 10.37.94.2
///    - \c DS_GetStaticIp( 118, 3) would return \c 10.01.18.3
///    - And so on...
///
QString LIB_DS_DECL StaticIP (int team, int host);

///
/// Returns a calculated IP address based on the team address.
///
/// For example:
///    - \c DS_GetStaticIp(10, 3794, 1) would return \c 10.37.94.1
///    - \c DS_GetStaticIp(177, 3794, 2) would return \c 177.37.94.2
///    - \c DS_GetStaticIp(10m 118, 3) would return \c 10.01.18.3
///    - And so on...
///
QString LIB_DS_DECL StaticIP (int net, int team, int host);

///
/// Returns an user-friendly string given the inputed robot control mode.
/// You can use this function to update the status widget of the
/// client implementation.
///
QString LIB_DS_DECL CM_String (ControlMode mode);

///
/// Reads the stored data in the \a socket.
/// This function is used internaly by the library, however, you may use
/// this function for anything that your client may need.
///
QByteArray LIB_DS_DECL ReadSocket (QUdpSocket* socket);

///
/// Reads the stored data in the \a socket.
/// This function is used internaly by the library, however, you may use
/// this function for anything that your client may need.
///
QByteArray LIB_DS_DECL ReadSocket (QTcpSocket* socket);

///
/// Parses the input number into two bytes. This function is used
/// by several protocols during the packet generation process.
///
/// Example I/O of this function would be:
///
/// | Input | Output    |
/// |-------|-----------|
/// |  255  | 0xFF 0x00 |
/// |    1  | 0x00 0x01 |
/// |  256  | 0xFF 0x01 |
///
QByteArray LIB_DS_DECL ToBytes (int data);
}

#endif

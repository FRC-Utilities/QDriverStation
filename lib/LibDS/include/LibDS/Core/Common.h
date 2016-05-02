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
    kControlTest         = 0,
    kControlAutonomous   = 1,
    kControlTeleoperated = 2,
    kControlInvalid      = 3,
} ControlMode;

///
/// Represents the possible socket types used by the protocol
///
typedef enum {
    kUdpSocket           = 0,
    kTcpSocket           = 1,
} SocketType;

///
/// Represents the available alliances and positions during a match
///
typedef enum {
    kAllianceRed1        = 0,
    kAllianceRed2        = 1,
    kAllianceRed3        = 2,
    kAllianceBlue1       = 3,
    kAllianceBlue2       = 4,
    kAllianceBlue3       = 5,
} Alliance;

///
/// Represents the status of the DS <--> Robot communications
///
typedef enum {
    kFull                = 0,
    kPartial             = 1,
    kFailing             = 2,
} DS_CommStatus;

///
/// Used to differentiate between the different error levels of log messages
///
typedef enum {
    kLibLevel            = 0x01,
    kInfoLevel           = 0x02,
    kWarnLevel           = 0x04,
    kErrorLevel          = 0x08,
    kCriticalLevel       = 0x10,
} ErrorLevel;

///
/// Used by the library to store information about a joystick or any other
/// input device used to control the robot.
///
/// Please note that the LibDS only deals with the "networking" part of
/// a FRC Driver Station, the rest must be implemented by the client.
///
typedef struct {
    QString name;
    int     numAxes;
    int     numPOVs;
    int     numButtons;
    int*    POVs;
    float*  axes;
    bool*   buttons;
} Joystick;

///
/// Represents a joystick rumble request issued by the robot code
///
typedef struct {
    int  joystick;
    bool left;
    bool right;
} RumbleRequest;

///
/// Represents the received CAN status information from a robot packet
///
typedef struct {
    quint8 util;
    quint8 busOff;
    quint8 txFull;
    quint8 receive;
    quint8 transmit;
} CAN_Information;

///
/// Returns the current timezone code by calculating the difference between
/// the system timezone and the UTC timezone
///
QString LIB_DS_DECL timezone();

///
/// Sends a message through the DriverStation to the Client, which should interpret
/// it as a message from the NetConsole.
///
/// This is used by the library to send warnings and status messages to the
/// user through a simple and integrated manner.
///
void LIB_DS_DECL sendMessage (QString message);

///
/// Logs the message according to its \a type or error level
///
void LIB_DS_DECL log (int type, QString message);

///
/// Returns a calculated IP address based on the team address.
///
/// For example:
///    - \c DS_GetStaticIp (3794, 1) would return \c 10.37.94.1
///    - \c DS_GetStaticIp (3794, 2) would return \c 10.37.94.2
///    - \c DS_GetStaticIp ( 118, 3) would return \c 10.01.18.3
///    - And so on...
///
QString LIB_DS_DECL getStaticIP (int team, int host);

///
/// Returns a calculated IP address based on the team address.
///
/// For example:
///    - \c DS_GetStaticIp ( 10, 3794, 1) would return \c 10.37.94.1
///    - \c DS_GetStaticIp (177, 3794, 2) would return \c 177.37.94.2
///    - \c DS_GetStaticIp ( 10,  118, 3) would return \c 10.01.18.3
///    - And so on...
///
QString LIB_DS_DECL getStaticIP (int net, int team, int host);

///
/// Returns an user-friendly string given the inputed robot control mode.
/// You can use this function to update the status widget of the
/// client implementation.
///
inline QString controlModeString (ControlMode mode) {
    QString string;

    switch (mode) {
    case kControlTest:
        string = "Test";
        break;
    case kControlTeleoperated:
        string = "Teleoperated";
        break;
    case kControlAutonomous:
        string = "Autonomous";
        break;
    case kControlInvalid:
        string = "";
        break;
    }

    return string;
}

///
/// Reads the stored data in the \a socket.
/// This function is used internaly by the library, however, you may use
/// this function for anything that your client may need.
///
inline QByteArray readSocket (QUdpSocket* socket) {
    QByteArray buffer;

    buffer.resize (socket->pendingDatagramSize());
    socket->readDatagram (buffer.data(), buffer.size());

    return buffer;
}

///
/// Reads the stored data in the \a socket.
/// This function is used internaly by the library, however, you may use
/// this function for anything that your client may need.
///
inline QByteArray readSocket (QTcpSocket* socket) {
    return socket->readAll();
}

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
inline QByteArray intToBytes (int number) {
    QByteArray array;
    array.append (((number & 0xff00) >> 8));
    array.append (((number & 0xff)));
    return array;
}

///
/// Parses the input number into two bytes. This function is used by several
/// protocols to send the robot's battery voltage to the FMS.
///
inline QByteArray floatToBytes (float number) {
    int integer = (int) number;
    int decimal = number - integer;

    QByteArray array;
    array.append (integer);
    array.append (decimal);
    return array;
}
}

#endif

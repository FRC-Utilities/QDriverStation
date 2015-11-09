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
#ifndef _LIB_DS_PROTOCOL_2015_H
#define _LIB_DS_PROTOCOL_2015_H

#include "LibDS/DS_Protocol.h"

/**
 * \namespace DS_Protocol2015_Definitions
 * Defines the values used by the protocol to communicate with the robot
 */
namespace DS_Protocol2015_Definitions
{
/* Control Modes */
const short CONTROL_DISABLED = 0x01u;
const short CONTROL_TELEOP   = 0x04u;
const short CONTROL_TEST     = 0x05u;
const short CONTROL_AUTO     = 0x06u;
const short CONTROL_ESTOP    = 0x80u;
const short CONTROL_NO_COMM  = 0x02u;

/* Headers for client packets */
const short HEADER_GENERAL  = 0x01u;
const short HEADER_JOYSTICK = 0x0Cu;
const short HEADER_TIME     = 0x0Fu;
const short HEADER_TIMEZONE = 0x10u;

/* Robot instructions */
const short STATUS_OK        = 0x10u;
const short STATUS_NULL      = 0x00u;
const short STATUS_REBOOT    = 0x18u;
const short STATUS_KILL_CODE = 0x14u;

/* Robot program statuses */
const short PROGRAM_NO_CODE      = 0x00u;
const short PROGRAM_AUTO         = 0x30u;
const short PROGRAM_DISABLED     = 0x31u;
const short PROGRAM_TELEOP       = 0x32u;
const short PROGRAM_TEST         = 0x38u;
const short PROGRAM_REQUEST_TIME = 0x01u;

/* Alliance codes */
const short ALLIANCE_RED1  = 0x00u;
const short ALLIANCE_RED2  = 0x01u;
const short ALLIANCE_RED3  = 0x02u;
const short ALLIANCE_BLUE1 = 0x03u;
const short ALLIANCE_BLUE2 = 0x04u;
const short ALLIANCE_BLUE3 = 0x05u;

/* Tags, used for reading additional data from robot packets */
const short TAG_CPU = 0x05u;
const short TAG_RAM = 0x06u;

/* Robot information paths */
const QString FTP_PCM_PATH = "/tmp/frc_versions/PCM-0-versions.ini";
const QString FTP_PDP_PATH = "/tmp/frc_versions/PDP-0-versions.ini";
const QString FTP_LIB_PATH = "/tmp/frc_versions/FRC_Lib_Version.ini";
}

/**
 * \class DS_Protocol2015
 *
 * Implements the 2015 communication protocol.
 */
class LIB_DS_DECL DS_Protocol2015 : public DS_Protocol
{
    Q_OBJECT

public:
    explicit DS_Protocol2015();

public slots:
    /**
     * Changes the status byte to instruct the roboRIO to reboot
     */
    void reboot();

    /**
     * Returns the port in the roboRIO in which we send DS packets
     */
    int robotPort();

    /**
     * Returns the port in which we receive data from the roboRIO
     */
    int clientPort();

    /**
     * Changes the status byte to instruct the roboRIO to kill the user code
     * and start it again
     */
    void restartCode();

    /**
     * Generates a client packet, which is later sent to the robot.
     * If the control mode is in TeleOp, we also send joystick input data
     */
    QByteArray getClientPacket();

private:
    /**
     * Used for sending specific instructions to the roboRIO, such as restarting
     * the robot code or rebooting the system
     */
    int m_status;

    /**
     * If set to \c true, the client will generate a packet that contains the
     * timezone of the computer. This value is automatically updated when a
     * robot packet is received
     */
    bool m_sendDateTime;

    /**
     * Used for downloading and analyzing robot information through a FTP
     * connection with the roboRIO
     */
    QNetworkAccessManager m_manager;

private slots:
    /**
     * Resets the internal values, disables the robot and begins
     * searching for it over the network
     */
    void resetProtocol();

    /**
     * Returns the address of the robot radio (e.g 10.xx.yy.1)
     */
    QString defaultRadioAddress();

    /**
     * Returns the address of the roboRIO (e.g roboRIO-xxyy.local)
     */
    QString defaultRobotAddress();

    /**
     * Downloads information about the roboRIO, the PCM and PDP firmware
     * information using a FTP connection.
     *
     * This function is called the first time we receive a packet from the
     * roboRIO
     */
    void downloadRobotInformation();

    /**
     * Reads joystick input and generates a packet that will be sent to
     * the robot
     */
    QByteArray generateJoystickData();

    /**
     * Generates a timezone section data to be sent to the robot when needed
     */
    QByteArray generateTimezoneData();

    /**
     * Reads and interprets packets received from the robot.
     */
    void readRobotData (QByteArray data);

    /**
     * Returns the value used to represent the input the control \a mode
     */
    char getControlCode (DS_ControlMode mode);

    /**
     * Returns the control mode represented by the input \a byte
     */
    DS_ControlMode getControlMode (char byte);

    /**
     * Returns the value used to represent the input \a alliance
     */
    char getAllianceCode (DS_Alliance alliance);

    /**
     * Returns the size of the input \a joystick structure
     * Used for encoding the joystick input and sending it to the robot
     */
    int getJoystickSize (DS_Joystick* joystick);

    /**
     * Reads and interprets the files that contain information about the
     * roboRIO, the PCM and PDP firmware versions
     */
    void onDownloadFinished (QNetworkReply* reply);
};

#endif

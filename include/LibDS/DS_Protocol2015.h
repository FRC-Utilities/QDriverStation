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

#include "DS_Protocol.h"

/**
 * \class DS_Protocol2015
 *
 * Implements the 2015 communication protocol.
 */
class LIB_DS_DECL DS_Protocol2015 : public DS_Protocol {
    Q_OBJECT

  public:
    explicit DS_Protocol2015();

  public slots:
    /**
     * Resets the internal values, disables the robot and begins
     * searching for it over the network
     */
    void reset();

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
     * Returns the address of the roboRIO (e.g roboRIO-xxyy.local) or,
     * if applicable, the custom address
     */
    QString robotAddress();

    /**
     * Returns the address of the robot radio (e.g 10.xx.yy.1)
     */
    QString radioAddress();

    /**
     * Downloads information about the roboRIO, the PCM and PDP firmware
     * information using a FTP connection.
     *
     * This function is called the first time we receive a packet from the
     * roboRIO
     */
    void downloadRobotInformation();

    /**
     * Generates a client packet, which is later sent to the robot.
     * If the control mode is in TeleOp, we also send joystick input data
     */
    QByteArray generateClientPacket();

    /**
     * Reads joystick input and generates a packet that will be sent to
     * the robot
     */
    QByteArray generateJoystickData();

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

  private:
    /**
     * Used to generate the ping data for each client packet
     */
    int m_index;

    /**
     * Used for sending specific instructions to the roboRIO, such as restarting
     * the robot code or rebooting the system
     */
    int m_status;

    /**
     * Used for downloading and analyzing robot information through a FTP
     * connection with the roboRIO
     */
    QNetworkAccessManager m_manager;

  private slots:
    /**
     * Changes the address of the roboRIO and looks
     * for the robot address in order to get its IP
     */
    void onRobotAddressChanged (QString address);

    /**
     * Gets the IP of the roboRIO when we finish looking for
     * its mDNS address of the roboRIO
     */
    void onLookupFinished (QHostInfo info);

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

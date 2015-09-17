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

#ifndef _DRIVER_STATION_SENDER_H
#define _DRIVER_STATION_SENDER_H

#include "Common.h"

#include <QByteArray>
#include <QUdpSocket>
#include <QHostAddress>

/**
 * Represents a control packet, which tells the robot:
 *     - The control mode to use
 *     - The alliance and position to use
 *     - The packet count (generated automatically)
 *     - The status (e.g: reboot or just be stay still)
 */
struct DS_ControlPacket {
    DS_Status status;
    DS_ControlMode mode;
    DS_Alliance alliance;
};

/**
 * \class DS_Sender
 */
class DS_Sender
{
public:
    /**
     * @internal
     * Main entry point of the class
     */
    explicit DS_Sender();

    /**
     * Resets the packet count index, it must be called everytime the
     * connection with the robot is reset
     */
    void resetIndex();

    /**
     * Sends a generated packet to the robot that contains the desired robot
     * status, its control mode, alliance and position.
     */
    void send (DS_ControlPacket packet, QString host);

    /**
     * Generates a 6-byte datagram based on the input data in the packet.
     */
    QByteArray generateControlPacket (DS_ControlPacket packet);

private:
    int m_index;
    QUdpSocket m_socket;
};

#endif /* _DRIVER_STATION_SENDER_H */

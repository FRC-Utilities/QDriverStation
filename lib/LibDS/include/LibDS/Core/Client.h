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

#ifndef _LDS_CLIENT_H
#define _LDS_CLIENT_H

#include "LibDS/Core/Common.h"

/**
 * \class DS_Client
 *
 * Sends and receives data between the robot and the Driver Station.
 * The class "redirects" the received data from the robot to the current
 * protocol that is in use by the Driver Station and vice-versa.
 */
class LIB_DS_DECL DS_Client : public QObject
{
    Q_OBJECT

public:
    explicit DS_Client (QObject* parent);

public slots:
    /**
     * Sends a the input \a data to the FMS
     */
    void sendToFms (QByteArray data);

    /**
     * Sends a the input \a data to the robot
     */
    void sendToRobot (QByteArray data);

    /**
     * Changes the port in which we receive the FMS packets
     */
    void setFmsInputPort (int port);

    /**
     * Changes the port in which we send the FMS packets
     */
    void setFmsOutputPort (int port);

    /**
     * Changes the port in where we receive robot packets
     */
    void setRobotInputPort (int port);

    /**
     * Changes the port in which we send the DS->Robot packets
     */
    void setRobotOutputPort (int port);

    /**
     * Changes the address where we send the packets to
     */
    void setRobotAddress (QString address);

signals:
    /**
     * Emitted when the client receives a packet from the FMS
     */
    void fmsPacketReceived (QByteArray);

    /**
     * Emitted when the client receives a packet from the robot
     */
    void robotPacketReceived (QByteArray);

private slots:
    /**
     * Reads the received data and sends it to the \c DriverStation
     */
    void readFmsPacket();

    /**
     * Reads the received data and sends it to the \c DriverStation
     */
    void readRobotPacket();

private:
    /**
     * The port in which we send data to the FMS
     */
    int m_fmsPort;

    /**
     * The port in which we send data to the robot
     */
    int m_robotPort;

    /**
     * The address of the robot
     */
    QString m_robotAddress;

    /**
     * We send data to the robot through this socket
     */
    QUdpSocket m_robotSender;

    /**
     * We send data to the FMS through this port
     */
    QTcpSocket m_fmsSender;

    /**
     * We receive data from the robot through this socket
     */
    QTcpSocket m_fmsReceiver;

    /**
     * We receive data from the robot through this socket
     */
    QUdpSocket m_robotReceiver;
};

#endif
